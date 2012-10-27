#include "particle.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstddef>

#include "circles.h"
#include "transform2D.h"

ParticleDrawer::ParticleDrawer() : num_particles_(true) {
  projection_ = glm::perspective(35.0f, 1.0f, 0.1f, 100.0f);
  inverse_projection_ = glm::inverse(projection_);
}

ParticleDrawer::~ParticleDrawer() {}

void ParticleDrawer::init() {
  glm::vec2 positions[4] = {glm::vec2(-1.0f, -1.0f), glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(-1.0f, 1.0f)};
  glm::vec2 tex_coords[4] = {glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)};

  glGenVertexArrays(1, &array_object_);
  glGenBuffers(2, quad_buffer_objects_);
  glGenBuffers(1, &particle_buffer_object_);

  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, quad_buffer_objects_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
  GLint handle = Renderer::instance().attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, quad_buffer_objects_[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
  handle = Renderer::instance().attributeHandle("tex_coord");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, particle_buffer_object_);
  handle = Renderer::instance().attributeHandle("color");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleDrawInfo), (void *)offsetof(ParticleDrawInfo, color));
  glVertexAttribDivisor(handle, 1);
  handle = Renderer::instance().attributeHandle("translate");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleDrawInfo), (void *)offsetof(ParticleDrawInfo, position));
  glVertexAttribDivisor(handle, 1);

  Renderer::instance().useProgram("particles");
  glUniform1fv(Renderer::instance().uniformHandle("size"), 1, &kParticleRadius);
  // TODO: take these out in all files!
  texture_handle_ = Renderer::instance().getTexture("textures/particle.dds");
  glUniform1i(Renderer::instance().uniformHandle("color_texture"), 0);
}

void ParticleDrawer::sendParticles(ParticleDrawInfo *particles, int num_particles) {
  num_particles_ = num_particles;
  glBindBuffer(GL_ARRAY_BUFFER, particle_buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleDrawInfo) * num_particles, particles, GL_STREAM_DRAW);
}

void ParticleDrawer::draw() {
  if (num_particles_ > 0) {
    Renderer::instance().useProgram("particles");
    glUniformMatrix4fv(Renderer::instance().uniformHandle("transform3D"), 1, GL_FALSE, 
      glm::value_ptr(projection_ * transform3D_));
    glUniformMatrix3fv(Renderer::instance().uniformHandle("transform2D"), 1, GL_FALSE, 
      glm::value_ptr(Renderer::instance().rootNode()->fullTransform() * transform2D_));    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_handle_);
    glBindVertexArray(array_object_);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, num_particles_);
  }
}
