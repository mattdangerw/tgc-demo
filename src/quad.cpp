#include "quad.h"

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Quad::Quad() : program_ (NULL), transform_(glm::mat3(1.0f)) {
  vertices_[0] = glm::vec2(0.0f, 0.0f);
  vertices_[1] = glm::vec2(1.0f, 0.0f);
  vertices_[2] = glm::vec2(1.0f, 1.0f);
  vertices_[3] = glm::vec2(0.0f, 1.0f);
}

void Quad::initPositionBuffer() {
  glGenVertexArrays(1, &array_object_);
  glGenBuffers(1, &buffer_object_);
  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
  GLint handle = program_->attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  modelview_handle_ = program_->uniformHandle("modelview");
}


void Quad::setCorners(glm::vec2 min, glm::vec2 max) {
  vertices_[0] = glm::vec2(min.x, min.y);
  vertices_[1] = glm::vec2(max.x, min.y);
  vertices_[2] = glm::vec2(max.x, max.y);
  vertices_[3] = glm::vec2(min.x, max.y);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
}

void Quad::getCorners(glm::vec2 *min, glm::vec2 *max) {
  *min = vertices_[0];
  *max = vertices_[2];
}

TexturedQuad::TexturedQuad() {
  tex_coords_[0] = glm::vec2(0.0f, 0.0f);
  tex_coords_[1] = glm::vec2(1.0f, 0.0f);
  tex_coords_[2] = glm::vec2(1.0f, 1.0f);
  tex_coords_[3] = glm::vec2(0.0f, 1.0f);
}

TexturedQuad::~TexturedQuad() {}

void TexturedQuad::init(string texture_file) {
  program_ = Renderer::instance().getProgram("textured");
  initPositionBuffer();

  glActiveTexture(GL_TEXTURE0);
  texture_handle_ = Renderer::instance().getTexture(texture_file);
  glGenBuffers(1, &texture_buffer_);
  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords_), tex_coords_, GL_STATIC_DRAW);
  GLuint handle = program_->attributeHandle("tex_coords");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  program_->use();
  glUniform1i(program_->uniformHandle("color_texture"), 0);
}

void TexturedQuad::draw(glm::mat3 transform) {
  program_->use();
  glActiveTexture(GL_TEXTURE0);
  glm::mat3 modelview = transform * transform_;
  glUniformMatrix3fv(modelview_handle_, 1, GL_FALSE, glm::value_ptr(modelview));
  glBindTexture(GL_TEXTURE_2D, texture_handle_);
  glBindVertexArray(array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

TiledTexturedQuad::TiledTexturedQuad() : color_mask_(glm::vec4(1.0f)), tex_scale_(1.0f) {
  tex_coords_[0] = glm::vec2(0.0f, 0.0f);
  tex_coords_[1] = glm::vec2(1.0f, 0.0f);
  tex_coords_[2] = glm::vec2(1.0f, 1.0f);
  tex_coords_[3] = glm::vec2(0.0f, 1.0f);
}

TiledTexturedQuad::~TiledTexturedQuad() {}

void TiledTexturedQuad::init(string texture_file) {
  program_ = Renderer::instance().getProgram("textured");
  initPositionBuffer();

  glActiveTexture(GL_TEXTURE0);
  texture_handle_ = Renderer::instance().getTexture(texture_file);
  glGenBuffers(1, &texture_buffer_);
  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords_), tex_coords_, GL_STATIC_DRAW);
  GLuint handle = program_->attributeHandle("tex_coords");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  program_->use();
  glUniform1i(program_->uniformHandle("color_texture"), 0);
}

void TiledTexturedQuad::setCorners(glm::vec2 min, glm::vec2 max) {
  Quad::setCorners(min, max);
  glm::vec2 size = tex_scale_ * (max - min);
  tex_coords_[0] = glm::vec2(0.0f, 0.0f);
  tex_coords_[1] = glm::vec2(max.x, 0.0f);
  tex_coords_[2] = glm::vec2(max.x, max.y);
  tex_coords_[3] = glm::vec2(0.0f, max.y);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords_), tex_coords_, GL_STATIC_DRAW);

  mask_handle_ = program_->uniformHandle("color_mask");
}

void TiledTexturedQuad::draw(glm::mat3 transform) {
  program_->use();
  glActiveTexture(GL_TEXTURE0);
  glm::mat3 modelview = transform * transform_;
  glUniformMatrix3fv(modelview_handle_, 1, GL_FALSE, glm::value_ptr(modelview));
  glUniform4fv(mask_handle_, 1, glm::value_ptr(color_mask_));
  glBindTexture(GL_TEXTURE_2D, texture_handle_);
  glBindVertexArray(array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

ColoredQuad::ColoredQuad() : color_(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) {}

ColoredQuad::~ColoredQuad() {}

void ColoredQuad::init() {
  program_ = Renderer::instance().getProgram("colored");
  initPositionBuffer();
  color_handle_ = program_->uniformHandle("color");
}

void ColoredQuad::draw(glm::mat3 transform) {
  program_->use();
  glm::mat3 modelview = transform * transform_;
  glUniformMatrix3fv(modelview_handle_, 1, GL_FALSE, glm::value_ptr(modelview));
  glUniform4fv(color_handle_, 1, glm::value_ptr(color_));
  glBindVertexArray(array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
