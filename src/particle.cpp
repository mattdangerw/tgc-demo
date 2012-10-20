#include "particle.h"

#include <glm/gtc/type_ptr.hpp>
#include <cstddef>

#include "circles.h"
#include "transform2D.h"

ParticleDrawer::ParticleDrawer()
  : program_(NULL), 
    num_particles_(true) {}

ParticleDrawer::~ParticleDrawer() {}

void ParticleDrawer::init() {
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
  handle = program_->attributeHandle("tex_coord");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, particle_buffer_object_);
  handle = program_->attributeHandle("color");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleDrawInfo), (void *)offsetof(ParticleDrawInfo, color));
  glVertexAttribDivisor(handle, 1);
  handle = program_->attributeHandle("translate");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleDrawInfo), (void *)offsetof(ParticleDrawInfo, position));
  glVertexAttribDivisor(handle, 1);

  program_->use();
  glUniform1fv(program_->uniformHandle("size"), 1, &kParticleRadius);
  // TODO: take these out in all files!
  glActiveTexture(GL_TEXTURE0);
  texture_handle_ = Renderer::instance().getTexture("textures/particle.dds");
  glUniform1i(program_->uniformHandle("color_texture"), 0);
}

void ParticleDrawer::sendParticles(ParticleDrawInfo *particles, int num_particles) {
  num_particles_ = num_particles;
  glBindBuffer(GL_ARRAY_BUFFER, particle_buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleDrawInfo) * num_particles, particles, GL_DYNAMIC_DRAW);
}

void ParticleDrawer::draw(glm::mat4 projection) {
  if (num_particles_ > 0) {
    program_->use();
    setMVPUniform(program_, projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_handle_);
    glBindVertexArray(array_object_);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, num_particles_);
  }
}
