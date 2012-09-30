#include "particle.h"

#include <gtc/type_ptr.hpp>
#include <cstddef>

#include "circles.h"
#include "transform2D.h"

ParticleDrawer::ParticleDrawer()
  : program_(NULL) {}

ParticleDrawer::~ParticleDrawer() {}

void ParticleDrawer::init(vector<Particle> *particles) {
  particles_ = particles;
  glm::vec2 positions[4] = {glm::vec2(-1.0f, -1.0f), glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(-1.0f, 1.0f)};
  glm::vec2 tex_coords[4] = {glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)};

  program_ = Renderer::instance().getProgram("particles");
  glGenVertexArrays(1, &array_object_);
  glGenBuffers(2, quad_buffer_objects_);
  glGenBuffers(1, &particle_buffer_object_);

  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, quad_buffer_objects_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
  GLint handle = program_->attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, quad_buffer_objects_[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
  handle = program_->attributeHandle("tex_coords");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, particle_buffer_object_);
  handle = program_->attributeHandle("color");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, color));
  glVertexAttribDivisor(handle, 1);
  handle = program_->attributeHandle("translate");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, position));
  glVertexAttribDivisor(handle, 1);

  modelview_handle_ = program_->uniformHandle("modelview");
  size_handle_ = program_->uniformHandle("size");
  glActiveTexture(GL_TEXTURE0);
  texture_handle_ = Renderer::instance().getTexture("textures/particle.dds");
  glUniform1i(program_->uniformHandle("color_texture"), 0);
}

void ParticleDrawer::draw(glm::mat3 transform) {
  if (!particles_->empty()) {
    program_->use();
    glm::mat3 modelview = transform * transform_;
    glUniformMatrix3fv(modelview_handle_, 1, GL_FALSE, glm::value_ptr(modelview));
    glUniform1fv(size_handle_, 1, &kParticleRadius);
    glBindBuffer(GL_ARRAY_BUFFER, particle_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particles_->size(), &(particles_->at(0)), GL_DYNAMIC_DRAW);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_handle_);
    glBindVertexArray(array_object_);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, particles_->size());
  }
}
