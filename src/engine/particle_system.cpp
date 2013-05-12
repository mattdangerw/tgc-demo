#include "engine/particle_system.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

#include "util/random.h"

static const float kParticleLifetime = 1.2f;
static const float kParticleAlphaDecay = 0.5f;

Emitter::Emitter()
  : visible_(false),
    position_(0.0f),
    color_(0.0f),
    current_source_(0),
    current_dest_(1),
    num_particles_(0) {}

Emitter::~Emitter() {}

void Emitter::init(int num_particles) {
  num_particles_ = num_particles;
  glGenVertexArrays(2, array_objects_);
  glGenBuffers(2, buffer_objects_);
  //glGenTransformFeedbacks(2, transform_feedbacks_);

  Particle *particles = new Particle[num_particles];
  float age = 0.0f;
  for (int i = 0; i < num_particles; ++i) {
    Particle &particle = particles[i];
    particle.position = glm::vec3();
    particle.velocity = 0.02f * randomDirection3D();
    particle.color = glm::vec4();
    particle.age = (kParticleLifetime * i) / (num_particles - 1);
    particle.visible = 0.0f;
  }

  for (int i = 0; i < 2; i++) {
    glBindVertexArray(array_objects_[i]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_objects_[i]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * num_particles, particles, GL_DYNAMIC_DRAW);
    // VAO varyings.
    GLuint handle = theEngine().attributeHandle("position");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, position));
    handle = theEngine().attributeHandle("velocity");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, velocity));
    handle = theEngine().attributeHandle("color");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, color));
    handle = theEngine().attributeHandle("age");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, age));
    handle = theEngine().attributeHandle("visible");
    glEnableVertexAttribArray(handle);
    glVertexAttribPointer(handle, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, visible));
    // Transform feedback init.
    //glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transform_feedbacks_[i]);
    //glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer_objects_[i]);
  }
  delete particles;
  
  theEngine().useProgram("particle_feedback");
  glUniform1f(theEngine().uniformHandle("alpha_decay"), kParticleAlphaDecay);
  glUniform1f(theEngine().uniformHandle("lifetime"), kParticleLifetime);
}

void Emitter::update(float delta_time) {
  theEngine().useProgram("particle_feedback");
  glUniform3fv(theEngine().uniformHandle("emitter_position"), 1, glm::value_ptr(position_));
  glUniform4fv(theEngine().uniformHandle("emitter_color"), 1, glm::value_ptr(color_));
  glUniform1f(theEngine().uniformHandle("emitter_visible"), visible_ ? 1.0f : 0.0f);
  glUniform1f(theEngine().uniformHandle("delta_time"), delta_time);

  //glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transform_feedbacks_[current_dest_]);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer_objects_[current_dest_]);
  glBindVertexArray(array_objects_[current_source_]);

  glEnable(GL_RASTERIZER_DISCARD);
  glBeginTransformFeedback(GL_POINTS);
  glDrawArrays(GL_POINTS, 0, num_particles_);
  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);

  current_source_ = (current_source_ + 1) % 2;
  current_dest_ = (current_dest_ + 1) % 2;
}

void Emitter::drawArray() {
  glBindVertexArray(array_objects_[current_source_]);
  glDrawArrays(GL_POINTS, 0, num_particles_);
}

ParticleSystem::ParticleSystem() {
  projection_ = glm::perspective(35.0f, 1.0f, 0.1f, 100.0f);
  inverse_projection_ = glm::inverse(projection_);
}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::init(int num_emitters) {
  texture_handle_ = theEngine().getTexture("content/textures/particle.dds");
  emitters_.resize(num_emitters);
  for (int i = 0; i < num_emitters; ++i) {
    emitters_[i].init(300);
    emitters_by_depth_.push_back(i);
  }
  theEngine().useProgram("particle_draw");
  glUniform1f(theEngine().uniformHandle("particle_radius"), 0.012f);
  glUniform3fv(theEngine().uniformHandle("camera_position"), 1, glm::value_ptr(glm::vec3(0.0f)));
}

void ParticleSystem::update(float delta_time) {
  for (vector<Emitter>::iterator it = emitters_.begin(); it != emitters_.end(); ++it) {
    it->update(delta_time);
  }
}

void ParticleSystem::draw() {
  sortDepthIndex();
  theEngine().useProgram("particle_draw");
  glUniformMatrix4fv(theEngine().uniformHandle("transform3D"), 1, GL_FALSE, 
    glm::value_ptr(projection_ * transform3D_));
  glUniformMatrix3fv(theEngine().uniformHandle("transform2D"), 1, GL_FALSE, 
    glm::value_ptr(theEngine().rootNode()->fullTransform() * transform2D_));
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_handle_);
  for (vector<int>::iterator it = emitters_by_depth_.begin(); it != emitters_by_depth_.end(); ++it) {
    emitters_[*it].drawArray();
  }
}

struct DepthSortFunctor {
  bool operator() (const int left, const int right) {
    return emitters->at(left).position().z < emitters->at(right).position().z;
  }
  vector<Emitter> *emitters;
};

void ParticleSystem::sortDepthIndex() {
  DepthSortFunctor functor;
  functor.emitters = &emitters_;
  std::sort(emitters_by_depth_.begin(), emitters_by_depth_.end(), functor);
}
