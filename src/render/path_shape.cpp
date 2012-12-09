#include "render/path_shape.h"

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

static map<string, PathShapeData> loaded_shape_data;
static PathShapeData *loadIfNeeded(string filename) {
  if (loaded_shape_data.count(filename) == 0) {
    loaded_shape_data[filename].init(filename);
  }
  return &loaded_shape_data[filename];
}

PathShapeData::PathShapeData() : has_solids_(false), has_quadrics_(false) {}

PathShapeData::~PathShapeData() {}

void PathShapeData::init(string filename) {
  vector<PathVertex> vertices;
  readVertices(filename, &vertices);
  init(vertices);
}

void PathShapeData::init(const vector<PathVertex> &vertices) {
  findCorners(vertices);
  vector<glm::vec2> solids, quadrics, bezier_coords;
  prepVertices(vertices, &solids, &quadrics);
  makeBezierCoords(quadrics, &bezier_coords);
  // Set members.
  solids_size_ = solids.size();
  has_solids_ = solids_size_ > 0;
  quadrics_size_ = quadrics.size();
  has_quadrics_ = quadrics_size_ > 0;
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
}

void PathShapeData::corners(glm::vec2 *min, glm::vec2 *max) {
  *min = min_corner_;
  *max = max_corner_;
}

void PathShapeData::readVertices(string filename, vector<PathVertex> *vertices) {
  json_value &path_json = readFileToJSON(filename);
  for (int i = 0; i < path_json.getLength(); i++) {
    PathVertex vertex;
    const json_value &elem = path_json[i];
    int type = elem["type"].getInteger();
    if (type == 0) continue;
    vertex.type = (PathVertexType)type;
    vertex.position.x = elem["x"].getFloat();
    vertex.position.y = elem["y"].getFloat();
    vertices->push_back(vertex);
  }
  json_value_free(&path_json);
}

void PathShapeData::prepVertices(const vector<PathVertex> &vertices, vector<glm::vec2> *solids, vector<glm::vec2> *quadrics) {
  for (unsigned int i = 0; i < vertices.size(); ++i) {
    PathVertexType type = vertices[i].type;
    if (type == ON_PATH) {
      solids->push_back(vertices[i].position);
    } else if (type == QUADRIC) {
      quadrics->push_back(vertices[i-1].position);
      quadrics->push_back(vertices[i].position);
      quadrics->push_back(vertices[i+1].position);
    } else if (type == CUBIC) {
      cubicToQuadrics(vertices[i-1].position, vertices[i].position, vertices[i+1].position, vertices[i+2].position, solids, quadrics);
      // Skip the next vertex its the other cubic control.
      ++i;
    }
  }
}

void PathShapeData::findCorners(const vector<PathVertex> &vertices) {
  min_corner_ = vertices[0].position;
  max_corner_ = vertices[0].position;
  for (vector<PathVertex>::const_iterator it = vertices.begin(); it != vertices.end(); ++it) {
    glm::vec2 position = it->position;
    min_corner_ = glm::min(position, min_corner_);
    max_corner_ = glm::max(position, max_corner_);
  }
}

void PathShapeData::cubicToQuadrics(glm::vec2 start, glm::vec2 control1, glm::vec2 control2, glm::vec2 end,
  vector<glm::vec2> *solids, vector<glm::vec2> *quadrics) {
  // Approximate the cubic with two quadrics.
  // Find on path point with succesive midpoints.
  glm::vec2 midpoints[3];
  midpoints[0] = midpoint(start, control1);
  midpoints[1] = midpoint(control1, control2);
  midpoints[2] = midpoint(control2, end);
  glm::vec2 new_tangents[2];
  new_tangents[0] = midpoint(midpoints[0], midpoints[1]);
  new_tangents[1] = midpoint(midpoints[1], midpoints[2]);
  glm::vec2 new_on_path = midpoint(new_tangents[0], new_tangents[1]);
  // Calculate new control points preserving tangent lines.
  glm::vec2 new_control_point;
  bool degenerate, intersects;
  // First quadric
  intersectRays(start, control1, new_tangents[1], new_tangents[0], &degenerate, &intersects, &new_control_point);
  quadrics->push_back(start);
  if (!degenerate && intersects) {
    quadrics->push_back(new_control_point);
  } else {
    quadrics->push_back(midpoint(start, new_on_path));
  }
  quadrics->push_back(new_on_path);
  // Second quadric
  intersectRays(end, control2, new_tangents[0], new_tangents[1], &degenerate, &intersects, &new_control_point);
  quadrics->push_back(new_on_path);
  if (!degenerate && intersects) {
    quadrics->push_back(new_control_point);
  } else {
    quadrics->push_back(midpoint(new_on_path, end));
  }
  quadrics->push_back(end);
  // Add a solid point at the new on path points
  solids->push_back(new_on_path);
}

void PathShapeData::makeBezierCoords(const vector<glm::vec2> &quadrics, vector<glm::vec2> *bezier_coords) {
  for (size_t i = 0; i < quadrics.size()/3 ; i++) {
    bezier_coords->push_back(glm::vec2(0.0f, 0.0f));
    bezier_coords->push_back(glm::vec2(0.5f, 0.0f));
    bezier_coords->push_back(glm::vec2(1.0f, 1.0f));
  }
}

PathShape::PathShape() : animated_(false), from_file_(false) {}

PathShape::~PathShape() {
  if (!from_file_) delete data_;
}

void PathShape::init(const vector<PathVertex> &vertices, Quad *fill) {
  from_file_ = false;
  data_ = new PathShapeData();
  data_->init(vertices);
  glm::vec2 min, max;
  data_->corners(&min, &max);
  initHelper(fill, min, max);
}

void PathShape::init(string filename, Quad *fill) {
  from_file_ = true;
  data_ = loadIfNeeded(filename);
  glm::vec2 min, max;
  data_->corners(&min, &max);
  initHelper(fill, min, max);
}

void PathShape::init(const vector<NamedFile> &frames, Quad *fill, Animator *animator) {
  from_file_ = true;
  animated_ = true;

  glm::vec2 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min());
  for (vector<NamedFile>::const_iterator it = frames.begin(); it != frames.end(); ++it) {
    PathShapeData *data = loadIfNeeded(it->file);
    glm::vec2 frame_min, frame_max;
    data->corners(&frame_min, &frame_max);
    min = glm::min(frame_min, min);
    max = glm::max(frame_max, max);
    frames_[it->name] = data;
  }
  data_ = frames_.begin()->second;
  animator_ = animator;
  initHelper(fill, min, max);
}

void PathShape::initHelper(Quad *fill, glm::vec2 min, glm::vec2 max) {
  fill_ = fill;
  fill_->setParent(this);
  fill_->setCorners(min, max);
  fill_->setIsVisible(false);
  createVAOs();
}

// Prepare GL to draw paths and build indices for each bezier level.
void PathShape::createVAOs() {
  // Set up the solid path traingles VAO
  if (data_->hasSolidVertices()) {
    glGenVertexArrays(1, &solid_array_object_);
    glBindVertexArray(solid_array_object_);
    if (animated_) {
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("position"));
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("lerp_position1"));
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("lerp_position2"));
    } else {
      glBindBuffer(GL_ARRAY_BUFFER, data_->solidBufferObject());
      GLuint handle = Renderer::instance().attributeHandle("position");
      glEnableVertexAttribArray(handle);
      glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
  }

  // Set up the quadric triangles VAO
  if (data_->hasQuadricVertices()) {
    glGenVertexArrays(1, &quadric_array_object_);
    glBindVertexArray(quadric_array_object_);
    if (animated_) {
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("position"));
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("lerp_position1"));
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("lerp_position2"));
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("bezier_coord"));      
    } else {
      // Set up the quadric vertices vertex buffer
      glBindBuffer(GL_ARRAY_BUFFER, data_->quadricBufferObject());
      GLuint handle = Renderer::instance().attributeHandle("position");
      glEnableVertexAttribArray(handle);
      glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

      // Pass in the bezier texture coords.
      glBindBuffer(GL_ARRAY_BUFFER, data_->bezierCoordsBufferObject());
      handle = Renderer::instance().attributeHandle("bezier_coord");
      glEnableVertexAttribArray(handle);
      glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
  }
}

void PathShape::bindKeyframeBuffers() {
  string keyframe_names[3];
  animator_->currentKeyState(keyframe_names, lerp_ts_);

  PathShapeData *keyframe1 = frames_[keyframe_names[0]];
  PathShapeData *keyframe2 = frames_[keyframe_names[1]];
  PathShapeData *keyframe3 = frames_[keyframe_names[2]];
  if (data_->hasSolidVertices()) {
    glBindVertexArray(solid_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->solidBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("position"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe2->solidBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("lerp_position1"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe3->solidBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("lerp_position2"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }
  if (data_->hasQuadricVertices()) {
    glBindVertexArray(quadric_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->quadricBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("position"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe2->quadricBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("lerp_position1"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe3->quadricBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("lerp_position2"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->bezierCoordsBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("bezier_coord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }
}

void PathShape::setOccluderColor(float color) {
  fill_->setOccluderColor(color);
}

// For now just give the quad extent as width and height. This is the maximum extent the shape can have on screen.
float PathShape::width() {
  glm::vec2 min, max;
  fill_->getCorners(&min, &max);
  return max.x - min.x;
}

float PathShape::height() {
  glm::vec2 min, max;
  fill_->getCorners(&min, &max);
  return max.y - min.y;
}

void PathShape::drawHelper(bool asOccluder) {
  if (animated_) bindKeyframeBuffers();

  // Ready stencil drawing.
  glEnable(GL_STENCIL_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 0, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);

  // Draw solid and quadric triangles, inverting the stencil each time.
  if (data_->hasSolidVertices()) {
    if (animated_) {
      Renderer::instance().useProgram("minimal_animated");
      glUniform1f(Renderer::instance().uniformHandle("lerp_t1"), lerp_ts_[0]);
      glUniform1f(Renderer::instance().uniformHandle("lerp_t2"), lerp_ts_[1]);
    } else {
      Renderer::instance().useProgram("minimal");
    }
    glUniformMatrix3fv(Renderer::instance().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
    glBindVertexArray(solid_array_object_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, data_->solidVerticesSize());
  }

  if (data_->hasQuadricVertices()) {
    if (animated_) {
      Renderer::instance().useProgram("quadric_animated");
      glUniform1f(Renderer::instance().uniformHandle("lerp_t1"), lerp_ts_[0]);
      glUniform1f(Renderer::instance().uniformHandle("lerp_t2"), lerp_ts_[1]);
    } else {
      Renderer::instance().useProgram("quadric");
    }
    glEnable(GL_DEPTH_TEST);
    glUniformMatrix3fv(Renderer::instance().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
    glBindVertexArray(quadric_array_object_);
    glDrawArrays(GL_TRIANGLES, 0, data_->quadricVerticesSize());
    glDisable(GL_DEPTH_TEST);
  }

  // Draw a quad over the whole shape and test with stencil.
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilFunc(GL_EQUAL, 1, 1);
  glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
  if (asOccluder) {
    fill_->drawOccluder();
  } else {
    fill_->draw();
  }
  glDisable(GL_STENCIL_TEST);
}
