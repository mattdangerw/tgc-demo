#include "path_shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <sstream>
#include <stdlib.h>

#include "error.h"
#include "random.h"
#include "transform2D.h"

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
  vector<glm::vec2> solids, quadrics, bezier_coords;
  prepVertices(vertices, &solids, &quadrics);
  makeBezierCoords(quadrics, &bezier_coords);
  // Set members.
  solids_size_ = solids.size();
  has_solids_ = solids_size_ > 0;
  quadrics_size_ = quadrics.size();
  has_quadrics_ = quadrics_size_ > 0;
  // Sends buffer data.
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

void ShapeData::corners(glm::vec2 *min, glm::vec2 *max) {
  *min = min_corner_;
  *max = max_corner_;
}

void ShapeData::readVertices(string filename, vector<PathVertex> *vertices) {
  FILE *file_pointer = fopen(filename.c_str(), "r");
  if (file_pointer == NULL) error("Path file %s not found.\n", filename.c_str());
  char line[128];
  while (fgets(line, 128, file_pointer) != NULL) {
    PathVertex vertex;
    int type;
    std::istringstream stream(line);
    stream >> type;
    if (type == 0) continue;
    vertex.type = (PathVertexType)type;
    stream >> vertex.position.x;
    stream >> vertex.position.y;
    vertices->push_back(vertex);
  }
  fclose(file_pointer);
}

void ShapeData::prepVertices(const vector<PathVertex> &vertices, vector<glm::vec2> *solids, vector<glm::vec2> *quadrics) {
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

void ShapeData::findCorners(const vector<PathVertex> &vertices) {
  min_corner_ = vertices[0].position;
  max_corner_ = vertices[0].position;
  for (vector<PathVertex>::const_iterator it = vertices.begin(); it != vertices.end(); ++it) {
    glm::vec2 position = it->position;
    min_corner_ = glm::min(position, min_corner_);
    max_corner_ = glm::max(position, max_corner_);
  }
}

void ShapeData::cubicToQuadrics(glm::vec2 start, glm::vec2 control1, glm::vec2 control2, glm::vec2 end,
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

void ShapeData::makeBezierCoords(const vector<glm::vec2> &quadrics, vector<glm::vec2> *bezier_coords) {
  for (size_t i = 0; i < quadrics.size()/3 ; i++) {
    bezier_coords->push_back(glm::vec2(0.0f, 0.0f));
    bezier_coords->push_back(glm::vec2(0.5f, 0.0f));
    bezier_coords->push_back(glm::vec2(1.0f, 1.0f));
  }
}

PathShape::PathShape() : dynamic_(false), time_(randomFloat(0.0f, 1.0f)), from_file_(false) {}

PathShape::~PathShape() {}

void PathShape::init(const vector<PathVertex> &vertices, Quad *fill) {
  from_file_ = false;
  data_ = new ShapeData();
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

void PathShape::init(const vector<string> &keyframe_files, const vector<float> &keyframe_durations, Quad *fill) {
  from_file_ = true;
  dynamic_ = true;
  
  glm::vec2 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min());
  for (vector<string>::const_iterator it = keyframe_files.begin(); it != keyframe_files.end(); ++it) {
    ShapeData *data = loadIfNeeded(*it);
    glm::vec2 frame_min, frame_max;
    data->corners(&frame_min, &frame_max);
    min = glm::min(frame_min, min);
    max = glm::max(frame_max, max);
    keyframes_.push_back(data);
    data_ = data;
  }
  initHelper(fill, min, max);

  assert(keyframe_files.size() == keyframe_durations.size());
  keyframe_times_.push_back(0.0f);
  float t = 0.0f;
  for (size_t i = 0; i < keyframe_durations.size() - 1; ++i) {
    t += keyframe_durations[i];
    keyframe_times_.push_back(t);
  }
  animation_duration_ = t + keyframe_durations.back();
  last_keyframe_ = 0;
  next_keyframe_ = 1;
  time_ = randomFloat(0.0f, animation_duration_);
  bindKeyframeBuffers();
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
    if (dynamic_) {
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("key1"));
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("key2"));
    } else {
      glBindBuffer(GL_ARRAY_BUFFER, data_->solidBufferObject());
      GLint handle = Renderer::instance().attributeHandle("position");
      glEnableVertexAttribArray(handle);
      glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
  }

  // Set up the quadric triangles VAO
  if (data_->hasQuadricVertices()) {
    glGenVertexArrays(1, &quadric_array_object_);
    glBindVertexArray(quadric_array_object_);
    if (dynamic_) {
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("key1"));
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("key2"));      
      glEnableVertexAttribArray(Renderer::instance().attributeHandle("bezier_coord"));      
    } else {
      // Set up the quadric vertices vertex buffer
      glBindBuffer(GL_ARRAY_BUFFER, data_->quadricBufferObject());
      GLint handle = Renderer::instance().attributeHandle("position");
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
  if (!dynamic_) return;
  ShapeData *keyframe1 = keyframes_[last_keyframe_];
  ShapeData *keyframe2 = keyframes_[next_keyframe_];
  if (data_->hasSolidVertices()) {
    glBindVertexArray(solid_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->solidBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("key1"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe2->solidBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("key2"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }
  if (data_->hasQuadricVertices()) {
    glBindVertexArray(quadric_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, keyframe1->quadricBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("key1"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, keyframe2->quadricBufferObject());
    glVertexAttribPointer(Renderer::instance().attributeHandle("key2"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

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

void PathShape::animate(float delta_time) {
  if (!dynamic_) return;
  time_+=delta_time;
  float last_key_time = keyframe_times_[last_keyframe_];
  float next_key_time = keyframe_times_[next_keyframe_];
  if (next_keyframe_ == 0) {
    next_key_time = animation_duration_;
  }
  if (next_key_time <= time_) {
    last_keyframe_ = next_keyframe_;
    last_key_time = next_key_time;
    if (last_keyframe_ == 0) {
      last_key_time = 0.0f;
      time_ -= animation_duration_;
    }
    next_keyframe_ = (next_keyframe_ + 1) % keyframe_times_.size();
    next_key_time = keyframe_times_[next_keyframe_];
    if (next_keyframe_ == 0) {
      next_key_time = animation_duration_;
    }
    bindKeyframeBuffers();
  }
  keyframes_mix_ = (time_ - last_key_time) / (next_key_time - last_key_time);
}

void PathShape::drawHelper(bool asOccluder) {
  // Ready stencil drawing.
  glEnable(GL_STENCIL_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 0, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);

  // Draw solid and quadric triangles, inverting the stencil each time.
  if (data_->hasSolidVertices()) {
    if (dynamic_) {
      Renderer::instance().useProgram("minimal_animated");
      glUniform1f(Renderer::instance().uniformHandle("key_mix1"), keyframes_mix_);
    } else {
      Renderer::instance().useProgram("minimal");
    }
    glUniformMatrix3fv(Renderer::instance().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
    glBindVertexArray(solid_array_object_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, data_->solidVerticesSize());
  }

  if (data_->hasQuadricVertices()) {
    if (dynamic_) {
      Renderer::instance().useProgram("quadric_animated");
      glUniform1f(Renderer::instance().uniformHandle("key_mix1"), keyframes_mix_);
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
