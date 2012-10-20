#include "path_shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <sstream>

#include "transform2D.h"

// Forward declare out exit point.
void cleanupAndExit(int exit_code);

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

PathShape::PathShape() : dynamic_(false) {}

PathShape::~PathShape() {}

void PathShape::init(string filename, Quad *fill, bool fit_fill, bool dynamic) {
  vector<PathVertex> vertices;
  FILE *file_pointer = fopen(filename.c_str(), "r");
  if (file_pointer == NULL) {
    fprintf(stderr, "Path file %s not found.\n", filename.c_str());
    cleanupAndExit(1);
  }
  char line[64];
  while (fgets(line, 64, file_pointer) != NULL) {
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
  init(vertices, fill, fit_fill, dynamic);
}

void PathShape::init(const vector<PathVertex> &vertices, Quad *fill, bool fit_fill, bool dynamic) {
  dynamic_ = dynamic;
  quadric_program_ = Renderer::instance().getProgram("quadric");
  minimal_program_ = Renderer::instance().getProgram("minimal");
  fill_ = fill;

  if (fit_fill) fitFillQuad(vertices);
  updates_by_index_ = vector<vector<glm::vec2 *> >(vertices.size());
  for (unsigned int i = 0; i < vertices.size(); ++i) {
    PathVertexType type = vertices[i].type;
    if (type == ON_PATH) {
      solid_vertices_.push_back(vertices[i].position);
      updates_by_index_[i].push_back(&solid_vertices_.back());
    } else if (type == QUADRIC) {
      quadric_vertices_.push_back(vertices[i-1].position);
      updates_by_index_[i-1].push_back(&quadric_vertices_.back());
      quadric_vertices_.push_back(vertices[i].position);
      updates_by_index_[i].push_back(&quadric_vertices_.back());
      quadric_vertices_.push_back(vertices[i+1].position);
      updates_by_index_[i+1].push_back(&quadric_vertices_.back());
      pushBezierCoords();
    } else if (type == CUBIC) {
      cubicToQuadrics(vertices[i-1].position, vertices[i].position, vertices[i+1].position, vertices[i+2].position);
      // Skip the next vertex its the other cubic control.
      i++;
    }
  }
  createVAOs();
}

void PathShape::fitFillQuad(const vector<PathVertex> &vertices) {
  glm::vec2 min, max = vertices[0].position;
  for (vector<PathVertex>::const_iterator it = vertices.begin(); it != vertices.end(); ++it) {
    glm::vec2 position = it->position;
    min = glm::min(position, min);
    max = glm::max(position, max);
  }
  fill_->setCorners(min, max);
}

void PathShape::cubicToQuadrics(glm::vec2 start, glm::vec2 control1, glm::vec2 control2, glm::vec2 end) {
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
  if (!degenerate) {
    if (intersects) {
      quadric_vertices_.push_back(start);
      quadric_vertices_.push_back(new_control_point);
      quadric_vertices_.push_back(new_on_path);
      pushBezierCoords();
    }
    // TODO: try subdivide
  }
  // Second quadric
  intersectRays(end, control2, new_tangents[0], new_tangents[1], &degenerate, &intersects, &new_control_point);
  if (!degenerate) {
    if (intersects) {
      quadric_vertices_.push_back(new_on_path);
      quadric_vertices_.push_back(new_control_point);
      quadric_vertices_.push_back(end);
      pushBezierCoords();
    }
    // TODO: try subdivide
  }
  // Add a solid point at the new on path points
  solid_vertices_.push_back(new_on_path);
}

void PathShape::pushBezierCoords() {
  quadric_bezier_coords_.push_back(glm::vec2(0.0f, 0.0f));
  quadric_bezier_coords_.push_back(glm::vec2(0.5f, 0.0f));
  quadric_bezier_coords_.push_back(glm::vec2(1.0f, 1.0f));
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
    GLint handle = minimal_program_->attributeHandle("position");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  }

  // Set up the quadric triangles VAO
  if(quadric_vertices_.size() > 0) {
    glBindVertexArray(quadric_array_object_);
    glBindBuffer(GL_ARRAY_BUFFER, quadric_buffers_[0]);
    if (dynamic_) {
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * quadric_vertices_.size(), NULL, GL_STREAM_DRAW);
    } else {
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * quadric_vertices_.size(), &quadric_vertices_[0], GL_STATIC_DRAW);
    }
    GLint handle = quadric_program_->attributeHandle("position");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, quadric_buffers_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * quadric_bezier_coords_.size(), &quadric_bezier_coords_[0], GL_STATIC_DRAW);
    handle = quadric_program_->attributeHandle("tex_coord");
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

void PathShape::updateVertexPosition(int i, glm::vec2 position) {
  for (vector<glm::vec2 *>::iterator it = updates_by_index_[0].begin(); it != updates_by_index_[0].end(); ++it) {
    **it = position;
  }
}

void PathShape::drawHelper(glm::mat3 view, bool asOccluder) {
  // Send position buffers if dynamic.
  if (dynamic_) {
    glBindBuffer(GL_ARRAY_BUFFER, quadric_buffers_[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * quadric_vertices_.size(), &quadric_vertices_[0], GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, solid_vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * solid_vertices_.size(), &solid_vertices_[0], GL_STREAM_DRAW);
  }

  // Ready stencil drawing.
  glEnable(GL_STENCIL_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 0, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);

  // Draw solid and quadric triangles, inverting the stencil each time.
  if (solid_vertices_.size() > 0) {
    minimal_program_->use();
    setModelviewUniform(minimal_program_, view);
    glBindVertexArray(solid_array_object_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, solid_vertices_.size());
  }

  if (quadric_vertices_.size() > 0) {
    glEnable(GL_DEPTH_TEST);
    quadric_program_->use();
    setModelviewUniform(quadric_program_, view);
    glBindVertexArray(quadric_array_object_);
    glDrawArrays(GL_TRIANGLES, 0, quadric_vertices_.size());
    glDisable(GL_DEPTH_TEST);
  }

  // Draw a quad over the whole shape and test with stencil.
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilFunc(GL_EQUAL, 1, 1);
  glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
  if (asOccluder) {
    fill_->drawOcclude(modelview(view));
  } else {
    fill_->draw(modelview(view));
  }
  glDisable(GL_STENCIL_TEST);
}
