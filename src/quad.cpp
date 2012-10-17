#include "quad.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Quad::Quad() : program_ (NULL), occluder_color_(0.0f, 0.0f, 0.0f, 1.0f) {
  vertices_[0] = glm::vec2(0.0f, 0.0f);
  vertices_[1] = glm::vec2(1.0f, 0.0f);
  vertices_[2] = glm::vec2(1.0f, 1.0f);
  vertices_[3] = glm::vec2(0.0f, 1.0f);
}

void Quad::init() {
  program_ = Renderer::instance().getProgram("minimal");
  glGenVertexArrays(1, &array_object_);
  glGenBuffers(1, &buffer_object_);
  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
  GLint handle = program_->attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
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

void Quad::draw(glm::mat3 view) {
  program_->use();
  setModelviewUniform(program_, view);
  glBindVertexArray(array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Quad::drawOcclude(glm::mat3 view) {
  program_->use();
  glUniform4fv(program_->uniformHandle("color"), 1, glm::value_ptr(occluder_color_));
  Quad::draw(view);
}

TexturedQuad::TexturedQuad()
  : color_mask_(glm::vec4(1.0f)),
    tex_scale_(1.0f),
    shadowed_(false) {
  tex_coords_[0] = glm::vec2(0.0f, 0.0f);
  tex_coords_[1] = glm::vec2(1.0f, 0.0f);
  tex_coords_[2] = glm::vec2(1.0f, 1.0f);
  tex_coords_[3] = glm::vec2(0.0f, 1.0f);
}

TexturedQuad::~TexturedQuad() {}

void TexturedQuad::init(string texture_file) {
  textured_program_ = Renderer::instance().getProgram("textured");
  shadowed_program_ = Renderer::instance().getProgram("textured_with_shadows");
  Quad::init();

  glActiveTexture(GL_TEXTURE0);
  texture_handle_ = Renderer::instance().getTexture(texture_file);
  glGenBuffers(1, &texture_buffer_);
  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords_), tex_coords_, GL_STATIC_DRAW);
  GLuint handle = textured_program_->attributeHandle("tex_coord");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void TexturedQuad::setCorners(glm::vec2 min, glm::vec2 max) {
  Quad::setCorners(min, max);
  glm::vec2 size = tex_scale_ * (max - min);
  tex_coords_[0] = glm::vec2(0.0f, 0.0f);
  tex_coords_[1] = glm::vec2(size.x, 0.0f);
  tex_coords_[2] = glm::vec2(size.x, size.y);
  tex_coords_[3] = glm::vec2(0.0f, size.y);
  glBindBuffer(GL_ARRAY_BUFFER, texture_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords_), tex_coords_, GL_STATIC_DRAW);
}

void TexturedQuad::draw(glm::mat3 view) {
  Program *program = shadowed_ ? shadowed_program_ : textured_program_;
  program->use();
  setModelviewUniform(program, view);
  setColorMaskUniform(program, color_mask_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_handle_);
  glBindVertexArray(array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

ColoredQuad::ColoredQuad() : color_(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) {}

ColoredQuad::~ColoredQuad() {}

void ColoredQuad::init() {
  program_ = Renderer::instance().getProgram("colored");
  Quad::init();
  color_handle_ = program_->uniformHandle("color");
}

void ColoredQuad::draw(glm::mat3 view) {
  program_->use();
  setModelviewUniform(program_, view);
  glUniform4fv(color_handle_, 1, glm::value_ptr(color_));
  glBindVertexArray(array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
