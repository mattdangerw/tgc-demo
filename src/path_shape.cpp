#include "path_shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <sstream>
#include <stdlib.h>

#include "transform2D.h"

// Forward declare out exit point.
void cleanupAndExit(int exit_code);

static inline float randomFloat(float min, float max) {
  return min + rand()/(RAND_MAX/(max - min));
}

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

PathShape::PathShape() : dynamic_(false), time_(randomFloat(0.0f, 1.0f)) {}

PathShape::~PathShape() {}

void PathShape::init(const vector<PathVertex> &vertices, Quad *fill) {
  glm::vec2 min, max;
  corners(vertices, &min, &max);
  prepVertices(vertices, solid_vertices_, quadric_vertices_);
  initHelper(fill, min, max);
}

void PathShape::init(string filename, Quad *fill) {
  vector<PathVertex> vertices;
  readVertices(filename, vertices);
  glm::vec2 min, max;
  corners(vertices, &min, &max);
  prepVertices(vertices, solid_vertices_, quadric_vertices_);
  initHelper(fill, min, max);
}

void PathShape::init(vector<string> keyframe_files, vector<float> keyframe_durations, Quad *fill) {
  dynamic_ = true;
  
  solid_keys_.resize(keyframe_files.size());
  quadric_keys_.resize(keyframe_files.size());
  glm::vec2 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min());
  for (size_t i = 0; i < keyframe_files.size(); ++i) {
    vector<PathVertex> vertices;
    readVertices(keyframe_files[i], vertices);
    prepVertices(vertices, solid_keys_[i], quadric_keys_[i]);
    glm::vec2 frame_min, frame_max;
    corners(vertices, &frame_min, &frame_max);
    min = glm::min(frame_min, min);
    max = glm::max(frame_max, max);
  }
  solid_vertices_.resize(solid_keys_[0].size());
  quadric_vertices_.resize(quadric_keys_[0].size());

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

  initHelper(fill, min, max);
}

void PathShape::initHelper(Quad *fill, glm::vec2 min, glm::vec2 max) {
  fill_ = fill;
  fill_->setParent(this);
  fill_->setCorners(min, max);
  fill_->setIsVisible(false);
  createVAOs();
}

void PathShape::readVertices(string filename, vector<PathVertex> &vertices) {
  FILE *file_pointer = fopen(filename.c_str(), "r");
  if (file_pointer == NULL) {
    fprintf(stderr, "Path file %s not found.\n", filename.c_str());
    cleanupAndExit(1);
  }
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
    vertices.push_back(vertex);
  }
  fclose(file_pointer);
}

void PathShape::prepVertices(const vector<PathVertex> &vertices, vector<glm::vec2> &solids, vector<glm::vec2> &quadrics) {
  for (unsigned int i = 0; i < vertices.size(); ++i) {
    PathVertexType type = vertices[i].type;
    if (type == ON_PATH) {
      solids.push_back(vertices[i].position);
    } else if (type == QUADRIC) {
      quadrics.push_back(vertices[i-1].position);
      quadrics.push_back(vertices[i].position);
      quadrics.push_back(vertices[i+1].position);
    } else if (type == CUBIC) {
      cubicToQuadrics(vertices[i-1].position, vertices[i].position, vertices[i+1].position, vertices[i+2].position, solids, quadrics);
      // Skip the next vertex its the other cubic control.
      ++i;
    }
  }
}

void PathShape::corners(const vector<PathVertex> &vertices, glm::vec2 *min, glm::vec2 *max) {
  *min = vertices[0].position;
  *max = vertices[0].position;
  for (vector<PathVertex>::const_iterator it = vertices.begin(); it != vertices.end(); ++it) {
    glm::vec2 position = it->position;
    *min = glm::min(position, *min);
    *max = glm::max(position, *max);
  }
}

void PathShape::cubicToQuadrics(glm::vec2 start, glm::vec2 control1, glm::vec2 control2, glm::vec2 end,
  vector<glm::vec2> &solids, vector<glm::vec2> &quadrics) {
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
  quadrics.push_back(start);
  if (!degenerate && intersects) {
    quadrics.push_back(new_control_point);
  } else {
    quadrics.push_back(midpoint(start, new_on_path));
  }
  quadrics.push_back(new_on_path);
  // Second quadric
  intersectRays(end, control2, new_tangents[0], new_tangents[1], &degenerate, &intersects, &new_control_point);
  quadrics.push_back(new_on_path);
  if (!degenerate && intersects) {
    quadrics.push_back(new_control_point);
  } else {
    quadrics.push_back(midpoint(new_on_path, end));
  }
  quadrics.push_back(end);
  // Add a solid point at the new on path points
  solids.push_back(new_on_path);
}

void PathShape::makeBezierTexCoords(vector<glm::vec2> &bezier_tex_coords) {
  for (size_t i = 0; i < quadric_vertices_.size()/3 ; i++) {
    bezier_tex_coords.push_back(glm::vec2(0.0f, 0.0f));
    bezier_tex_coords.push_back(glm::vec2(0.5f, 0.0f));
    bezier_tex_coords.push_back(glm::vec2(1.0f, 1.0f));
  }
}

// Prepare GL to draw paths and build indices for each bezier level.
void PathShape::createVAOs() {
  // Solid path
  glGenVertexArrays(1, &solid_array_object_);
  glGenBuffers(1, &solid_vertex_buffer_);
  // Quadric path
  glGenVertexArrays(1, &quadric_array_object_);
  glGenBuffers(2, quadric_buffers_);

  // Set up the solid path traingles VAO
  if(solid_vertices_.size() > 0) {
    glBindVertexArray(solid_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, solid_vertex_buffer_);
    if (dynamic_) {
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * solid_vertices_.size(), NULL, GL_STREAM_DRAW);
    } else {
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * solid_vertices_.size(), &solid_vertices_[0], GL_STATIC_DRAW);
    }
    GLint handle = Renderer::instance().attributeHandle("position");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  // Set up the quadric triangles VAO
  if(quadric_vertices_.size() > 0) {
    // Set up the quadric vertices vertex buffer
    glBindVertexArray(quadric_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, quadric_buffers_[0]);
    if (dynamic_) {
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * quadric_vertices_.size(), NULL, GL_STREAM_DRAW);
    } else {
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * quadric_vertices_.size(), &quadric_vertices_[0], GL_STATIC_DRAW);
    }
    GLint handle = Renderer::instance().attributeHandle("position");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // Pass in the bezier texture coords.
    vector<glm::vec2> bezier_tex_coords;
    makeBezierTexCoords(bezier_tex_coords);
    glBindBuffer(GL_ARRAY_BUFFER, quadric_buffers_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * bezier_tex_coords.size(), &bezier_tex_coords[0], GL_STATIC_DRAW);
    handle = Renderer::instance().attributeHandle("bezier_coord");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }
}

void PathShape::setOccluderColor(glm::vec4 color) {
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
  }
  keyframes_mix_ = (time_ - last_key_time) / (next_key_time - last_key_time);

  for (size_t i = 0; i < quadric_vertices_.size(); i++) {
    quadric_vertices_[i] = glm::mix(quadric_keys_[last_keyframe_][i], quadric_keys_[next_keyframe_][i], keyframes_mix_);
  }
  for (size_t i = 0; i < solid_vertices_.size(); i++) {
    solid_vertices_[i] = glm::mix(solid_keys_[last_keyframe_][i], solid_keys_[next_keyframe_][i], keyframes_mix_);
  }
  glBindBuffer(GL_ARRAY_BUFFER, quadric_buffers_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * quadric_vertices_.size(), &quadric_vertices_[0], GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, solid_vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * solid_vertices_.size(), &solid_vertices_[0], GL_STREAM_DRAW);
}

void PathShape::drawHelper(bool asOccluder) {
  // Ready stencil drawing.
  glEnable(GL_STENCIL_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 0, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);

  // Draw solid and quadric triangles, inverting the stencil each time.
  if (solid_vertices_.size() > 0) {
    Renderer::instance().useProgram("minimal");
    glUniformMatrix3fv(Renderer::instance().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
    glBindVertexArray(solid_array_object_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, solid_vertices_.size());
  }

  if (quadric_vertices_.size() > 0) {
    glEnable(GL_DEPTH_TEST);
    Renderer::instance().useProgram("quadric");
    glUniformMatrix3fv(Renderer::instance().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
    glBindVertexArray(quadric_array_object_);
    glDrawArrays(GL_TRIANGLES, 0, quadric_vertices_.size());
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
