#include "engine/shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <stdlib.h>

#include "util/json.h"
#include "util/error.h"
#include "util/read_file.h"
#include "util/random.h"
#include "util/transform2D.h"

static inline glm::vec2 midpoint(glm::vec2 a, glm::vec2 b) {
  return glm::mix(a, b, 0.5f);
}

static void intersectRays(glm::vec2 a_src, glm::vec2 a_dest, glm::vec2 b_src, glm::vec2 b_dest, bool *degenerate, bool *intersects, glm::vec2 *intersection) {
  glm::vec2 diff = b_src - a_src;
  glm::vec2 a_dir = a_dest - a_src;
  glm::vec2 b_dir = b_dest - b_src;
  float determinant = b_dir.x * a_dir.y - b_dir.y * a_dir.x;
  *degenerate = determinant == 0;
  if (*degenerate) return;
  float u = (diff.y * b_dir.x - diff.x * b_dir.y) / determinant;
  float v = (diff.y * a_dir.x - diff.x * a_dir.y) / determinant;
  *intersects = (u > 0 && v > 0);
  if (*intersects) {
    *intersection = u * a_dir + a_src;
  }
}

static map<string, ShapeData> loaded_shape_data;
static ShapeData *loadIfNeeded(string filename) {
  if (loaded_shape_data.count(filename) == 0) {
    loaded_shape_data[filename].init(filename);
  }
  return &loaded_shape_data[filename];
}

ShapeData::ShapeData() : has_solids_(false), has_quadrics_(false) {}

ShapeData::~ShapeData() {}

void ShapeData::init(string filename) {
  vector<PathVertex> vertices;
  readVertices(filename, &vertices);
  init(vertices);
}

void ShapeData::init(const vector<PathVertex> &vertices) {
  findCorners(vertices);
  vector<glm::vec2> solids, quadrics, cubics, cubic_extras, bezier_coords;
  prepVertices(vertices, &solids, &quadrics, &cubics, &cubic_extras);
  makeBezierCoords(quadrics, &bezier_coords);
  // Set members.
  solids_size_ = solids.size();
  has_solids_ = solids_size_ > 0;
  quadrics_size_ = quadrics.size();
  has_quadrics_ = quadrics_size_ > 0;
  cubics_size_ = cubics.size();
  has_cubics_ = cubics_size_ > 0;
  // Send buffer data.
  if (has_solids_) {
    glGenBuffers(1, &solid_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, solid_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * solids.size(), &solids[0], GL_STATIC_DRAW);
  }
  if (has_quadrics_) {
    glGenBuffers(1, &quadric_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, quadric_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * quadrics.size(), &quadrics[0], GL_STATIC_DRAW);
    glGenBuffers(1, &bezier_coords_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, bezier_coords_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * bezier_coords.size(), &bezier_coords[0], GL_STATIC_DRAW);
  }
  if (has_cubics_) {
    glGenBuffers(1, &cubic_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, cubic_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * cubics.size(), &cubics[0], GL_STATIC_DRAW);
    glGenBuffers(1, &cubic_extra_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, cubic_extra_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * cubics.size(), &cubic_extras[0], GL_STATIC_DRAW);
  }
}

void ShapeData::extent(glm::vec2 *min, glm::vec2 *max) {
  *min = min_corner_;
  *max = max_corner_;
}

void ShapeData::readVertices(string filename, vector<PathVertex> *vertices) {
  json_value &path_json = readFileToJSON(filename);
  for (int i = 0; i < path_json.getLength(); i++) {
    PathVertex vertex;
    const json_value &elem = path_json[i];
    int type = elem["type"].getInteger();
    if (type == 0) continue;
    vertex.type = (PathVertexType)type;
    vertex.position = elem["pos"].getVec2();
    vertices->push_back(vertex);
  }
  json_value_free(&path_json);
}

void ShapeData::prepVertices(const vector<PathVertex> &vertices, vector<glm::vec2> *solids, vector<glm::vec2> *quadrics,
  vector<glm::vec2> *cubics, vector<glm::vec2> *cubic_extra) {
  for (unsigned int i = 0; i < vertices.size(); ++i) {
    PathVertexType type = vertices[i].type;
    if (type == ON_PATH) {
      solids->push_back(vertices[i].position);
    } else if (type == QUADRIC) {
      quadrics->push_back(vertices[i-1].position);
      quadrics->push_back(vertices[i].position);
      quadrics->push_back(vertices[i+1].position);
    } else if (type == CUBIC) {
      cubics->push_back(vertices[i-1].position);
      cubics->push_back(vertices[i].position);
      cubics->push_back(vertices[i+1].position);
      cubic_extra->push_back(vertices[i+2].position);
      cubic_extra->push_back(vertices[i+2].position);
      cubic_extra->push_back(vertices[i+2].position);
      // Skip the next vertex its the other cubic control.
      ++i;
    }
  }
}

void ShapeData::findCorners(const vector<PathVertex> &vertices) {
  min_corner_ = vertices[0].position;
  max_corner_ = vertices[0].position;
  for (vector<PathVertex>::const_iterator it = vertices.begin(); it != vertices.end(); ++it) {
    glm::vec2 position = it->position;
    min_corner_ = glm::min(position, min_corner_);
    max_corner_ = glm::max(position, max_corner_);
  }
}

void ShapeData::makeBezierCoords(const vector<glm::vec2> &quadrics, vector<glm::vec2> *bezier_coords) {
  for (size_t i = 0; i < quadrics.size()/3 ; i++) {
    bezier_coords->push_back(glm::vec2(0.0f, 0.0f));
    bezier_coords->push_back(glm::vec2(0.5f, 0.0f));
    bezier_coords->push_back(glm::vec2(1.0f, 1.0f));
  }
}

Shape::Shape() : animated_(false), from_file_(false) {}

Shape::~Shape() {
  if (!from_file_) delete data_;
}

void Shape::init(const vector<PathVertex> &vertices) {
  from_file_ = false;
  data_ = new ShapeData();
  data_->init(vertices);
  data_->extent(&min_, &max_);
  createVAOs();
}

void Shape::init(string filename) {
  from_file_ = true;
  data_ = loadIfNeeded(filename);
  data_->extent(&min_, &max_);
  createVAOs();
}

void Shape::init(const vector<NamedFile> &frames, Animator *animator) {
  from_file_ = true;
  animated_ = true;

  min_ = glm::vec2(std::numeric_limits<float>::max());
  max_ = glm::vec2(-std::numeric_limits<float>::max());
  for (vector<NamedFile>::const_iterator it = frames.begin(); it != frames.end(); ++it) {
    ShapeData *data = loadIfNeeded(it->file);
    glm::vec2 frame_min, frame_max;
    data->extent(&frame_min, &frame_max);
    min_ = glm::min(frame_min, min_);
    max_ = glm::max(frame_max, max_);
    frames_[it->name] = data;
  }
  data_ = frames_.begin()->second;
  animator_ = animator;
  createVAOs();
}

// Prepare GL to draw paths and build indices for each bezier level.
void Shape::createVAOs() {
  // Set up the solid path traingles VAO
  if (data_->hasSolidVertices()) {
    glGenVertexArrays(1, &solid_array_object_);
    glBindVertexArray(solid_array_object_);
    if (animated_) {
      glEnableVertexAttribArray(theEngine().attributeHandle("position"));
      glEnableVertexAttribArray(theEngine().attributeHandle("lerp_position1"));
      glEnableVertexAttribArray(theEngine().attributeHandle("lerp_position2"));
    } else {
      glBindBuffer(GL_ARRAY_BUFFER, data_->solidBufferObject());
      GLuint handle = theEngine().attributeHandle("position");
      glEnableVertexAttribArray(handle);
      glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
  }

  // Set up the quadric triangles VAO
  if (data_->hasQuadricVertices()) {
    glGenVertexArrays(1, &quadric_array_object_);
    glBindVertexArray(quadric_array_object_);
    if (animated_) {
      glEnableVertexAttribArray(theEngine().attributeHandle("position"));
      glEnableVertexAttribArray(theEngine().attributeHandle("lerp_position1"));
      glEnableVertexAttribArray(theEngine().attributeHandle("lerp_position2"));
      glEnableVertexAttribArray(theEngine().attributeHandle("bezier_coord"));
    } else {
      // Set up the quadric vertices vertex buffer
      glBindBuffer(GL_ARRAY_BUFFER, data_->quadricBufferObject());
      GLuint handle = theEngine().attributeHandle("position");
      glEnableVertexAttribArray(handle);
      glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

      // Pass in the bezier texture coords.
      glBindBuffer(GL_ARRAY_BUFFER, data_->bezierCoordsBufferObject());
      handle = theEngine().attributeHandle("bezier_coord");
      glEnableVertexAttribArray(handle);
      glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
  }

  // Set up the cubic triangles VAO
  if (data_->hasCubicVertices()) {
    glGenVertexArrays(1, &cubic_array_object_);
    glBindVertexArray(cubic_array_object_);
    if (animated_) {
      glEnableVertexAttribArray(theEngine().attributeHandle("position"));
      glEnableVertexAttribArray(theEngine().attributeHandle("lerp_position1"));
      glEnableVertexAttribArray(theEngine().attributeHandle("lerp_position2"));
      glEnableVertexAttribArray(theEngine().attributeHandle("extra_point"));
      glEnableVertexAttribArray(theEngine().attributeHandle("lerp_extra_point1"));
      glEnableVertexAttribArray(theEngine().attributeHandle("lerp_extra_point2"));
    } else {
      // Set up the quadric vertices vertex buffer
      glBindBuffer(GL_ARRAY_BUFFER, data_->cubicBufferObject());
      GLuint handle = theEngine().attributeHandle("position");
      glEnableVertexAttribArray(handle);
      glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

      // Pass in the bezier texture coords.
      glBindBuffer(GL_ARRAY_BUFFER, data_->cubicExtraBufferObject());
      handle = theEngine().attributeHandle("extra_point");
      glEnableVertexAttribArray(handle);
      glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
  }
}

void Shape::bindKeyframeBuffers() {
  string keyframe_names[3];
  animator_->currentState(keyframe_names, lerp_ts_);

  ShapeData *keyframe1 = frames_[keyframe_names[0]];
  ShapeData *keyframe2 = frames_[keyframe_names[1]];
  ShapeData *keyframe3 = frames_[keyframe_names[2]];
  if (data_->hasSolidVertices()) {
    glBindVertexArray(solid_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->solidBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("position"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe2->solidBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("lerp_position1"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe3->solidBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("lerp_position2"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }
  if (data_->hasQuadricVertices()) {
    glBindVertexArray(quadric_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->quadricBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("position"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe2->quadricBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("lerp_position1"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe3->quadricBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("lerp_position2"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->bezierCoordsBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("bezier_coord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }
  if (data_->hasCubicVertices()) {
    glBindVertexArray(cubic_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->cubicBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("position"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe2->cubicBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("lerp_position1"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe3->cubicBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("lerp_position2"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->cubicExtraBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("extra_point"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe2->cubicExtraBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("lerp_extra_point1"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe3->cubicExtraBufferObject());
    glVertexAttribPointer(theEngine().attributeHandle("lerp_extra_point2"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }
}

void Shape::drawHelper(bool asOccluder) {
  if (animated_) bindKeyframeBuffers();

  // Ready stencil drawing.
  glEnable(GL_STENCIL_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 0, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);

  // Draw solid and quadric triangles, inverting the stencil each time.
  if (data_->hasSolidVertices()) {
    if (animated_) {
      theEngine().useProgram("minimal_animated");
      glUniform1f(theEngine().uniformHandle("lerp_t1"), lerp_ts_[0]);
      glUniform1f(theEngine().uniformHandle("lerp_t2"), lerp_ts_[1]);
    } else {
      theEngine().useProgram("minimal");
    }
    glUniform4fv(theEngine().uniformHandle("color"), 1, glm::value_ptr(glm::vec4(1.0f)));
    glUniformMatrix3fv(theEngine().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
    glBindVertexArray(solid_array_object_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, data_->solidVerticesSize());
  }

  if (data_->hasQuadricVertices()) {
    if (animated_) {
      theEngine().useProgram("quadric_animated");
      glUniform1f(theEngine().uniformHandle("lerp_t1"), lerp_ts_[0]);
      glUniform1f(theEngine().uniformHandle("lerp_t2"), lerp_ts_[1]);
    } else {
      theEngine().useProgram("quadric");
    }
    glEnable(GL_DEPTH_TEST);
    glUniformMatrix3fv(theEngine().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
    glBindVertexArray(quadric_array_object_);
    glDrawArrays(GL_TRIANGLES, 0, data_->quadricVerticesSize());
    glDisable(GL_DEPTH_TEST);
  }

  if (data_->hasCubicVertices()) {
    if (animated_) {
      theEngine().useProgram("cubic_animated");
      glUniform1f(theEngine().uniformHandle("lerp_t1"), lerp_ts_[0]);
      glUniform1f(theEngine().uniformHandle("lerp_t2"), lerp_ts_[1]);
    } else {
      theEngine().useProgram("cubic");
    }
    glEnable(GL_DEPTH_TEST);
    glUniformMatrix3fv(theEngine().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
    glBindVertexArray(cubic_array_object_);
    glDrawArrays(GL_TRIANGLES, 0, data_->cubicVerticesSize());
    glDisable(GL_DEPTH_TEST);
  }

  // Draw a quad over the whole shape and test with stencil.
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilFunc(GL_EQUAL, 1, 1);
  glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
  if (asOccluder) {
    fill()->fillInOccluder(this);
  } else {
    fill()->fillIn(this);
  }
  glDisable(GL_STENCIL_TEST);
}
