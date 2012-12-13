#include "render/quad.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Quad::Quad() :
    color_(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
    color_mask_(glm::vec4(1.0f)),
    texture_scale_(1.0f),
    shadowed_(false),
    textured_(false) {
  vertices_[0] = glm::vec2(0.0f, 0.0f);
  vertices_[1] = glm::vec2(1.0f, 0.0f);
  vertices_[2] = glm::vec2(1.0f, 1.0f);
  vertices_[3] = glm::vec2(0.0f, 1.0f);

  tex_coords_[0] = glm::vec2(0.0f, 0.0f);
  tex_coords_[1] = glm::vec2(1.0f, 0.0f);
  tex_coords_[2] = glm::vec2(1.0f, 1.0f);
  tex_coords_[3] = glm::vec2(0.0f, 1.0f);
}

Quad::~Quad() {}

void Quad::init(glm::vec4 color) {
  initHelper();
  useColor(color);
}

void Quad::init(string texture_file, glm::vec2 texture_scale) {
  initHelper();
  useTexture(texture_file);
  setTextureScale(texture_scale);
}

void Quad::initHelper() {
  glGenVertexArrays(1, &array_object_);
  glBindVertexArray(array_object_);
  
  glGenBuffers(1, &position_buffer_object_);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
  GLuint handle = theRenderer().attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glGenBuffers(1, &tex_coord_buffer_object_);
  glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords_), tex_coords_, GL_STATIC_DRAW);
  handle = theRenderer().attributeHandle("tex_coord");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void Quad::extent(glm::vec2 *min, glm::vec2 *max) {
  *min = vertices_[0];
  *max = vertices_[2];
}

void Quad::setExtent(glm::vec2 min, glm::vec2 max) {
  vertices_[0] = glm::vec2(min.x, min.y);
  vertices_[1] = glm::vec2(max.x, min.y);
  vertices_[2] = glm::vec2(max.x, max.y);
  vertices_[3] = glm::vec2(min.x, max.y);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);

  glm::vec2 size = texture_scale_ * (max - min);
  tex_coords_[0] = glm::vec2(0.0f, 0.0f);
  tex_coords_[1] = glm::vec2(size.x, 0.0f);
  tex_coords_[2] = glm::vec2(size.x, size.y);
  tex_coords_[3] = glm::vec2(0.0f, size.y);
  glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords_), tex_coords_, GL_STATIC_DRAW);
}

void Quad::useTexture(string texture_file) {
  textured_ = true;
  texture_handle_ = theRenderer().getTexture(texture_file);
}

void Quad::useColor(glm::vec4 color) {
  textured_ = false;
  color_ = color;
}

void Quad::draw() {
  if (textured_) {
    string program = shadowed_ ? "textured_with_shadows" : "textured";
    theRenderer().useProgram(program);
    glUniform4fv(theRenderer().uniformHandle("color_mask"), 1, glm::value_ptr(color_mask_));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_handle_);
  } else {
    theRenderer().useProgram("minimal");
    glUniform4fv(theRenderer().uniformHandle("color"), 1, glm::value_ptr(color_));
  }
  glUniformMatrix3fv(theRenderer().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
  glBindVertexArray(array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Quad::drawOccluder() {
  theRenderer().useProgram("minimal");
  glm::vec4 occluder_vec(1.0f);
  occluder_vec.r = occluderColor();
  glUniform4fv(theRenderer().uniformHandle("color"), 1, glm::value_ptr(occluder_vec));
  glUniformMatrix3fv(theRenderer().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform()));
  glBindVertexArray(array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
