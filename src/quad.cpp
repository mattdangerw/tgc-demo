#include "quad.h"

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gli.hpp>
#include <gli/gtx/gl_texture2d.hpp>
#include <glm/gtc/type_ptr.hpp>

Quad::Quad() {
  tex_coords_[0] = glm::vec2(0.0f, 0.0f);
  tex_coords_[1] = glm::vec2(1.0f, 0.0f);
  tex_coords_[2] = glm::vec2(1.0f, 1.0f);
  tex_coords_[3] = glm::vec2(0.0f, 1.0f);
  vertices_[0] = glm::vec2(0.0f, 0.0f);
  vertices_[1] = glm::vec2(1.0f, 0.0f);
  vertices_[2] = glm::vec2(1.0f, 1.0f);
  vertices_[3] = glm::vec2(0.0f, 1.0f);
}

Quad::~Quad() {}

void Quad::init(Program *program, string texture_file) {
  program_ = program;
  glActiveTexture(GL_TEXTURE0);
  texture_handle_ = gli::createTexture2D(texture_file);
  
  glGenVertexArrays(1, &array_object_);
  glGenBuffers(2, buffers_);
  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, buffers_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
  GLint handle = program_->attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, buffers_[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords_), tex_coords_, GL_STATIC_DRAW);
  handle = program_->attributeHandle("tex_coords");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  program_->use();
  glUniform1i(program_->uniformHandle("color_texture"), 0);
  modelview_handle_ = program_->uniformHandle("modelview");
}

void Quad::setCorners(glm::vec2 min, glm::vec2 max) {
  vertices_[0] = glm::vec2(min.x, min.y);
  vertices_[1] = glm::vec2(max.x, min.y);
  vertices_[2] = glm::vec2(max.x, max.y);
  vertices_[3] = glm::vec2(min.x, max.y);
  glBindBuffer(GL_ARRAY_BUFFER, buffers_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
}

void Quad::getCorners(glm::vec2 *min, glm::vec2 *max) {
  *min = vertices_[0];
  *max = vertices_[3];
}

void Quad::draw(glm::mat3 transform) {
  program_->use();
  glActiveTexture(GL_TEXTURE0);
  glUniformMatrix3fv(modelview_handle_, 1, GL_FALSE, glm::value_ptr(transform));
  glBindTexture(GL_TEXTURE_2D, texture_handle_);
  glBindVertexArray(array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
