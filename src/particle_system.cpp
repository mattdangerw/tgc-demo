#include "particle_system.h"

#include <cmath>
#include <GL/glew.h>
#include <gtc/type_ptr.hpp>

#include "transform2D.h"

static inline float randomFloat(float min, float max) {
  return min + rand()/(RAND_MAX/(max - min));
}

static inline glm::vec2 randomDirection() {
  glm::vec2 direction = glm::vec2(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f));
  return glm::normalize(direction);
}

ParticleSystem::ParticleSystem() {}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::init(ThoughtBubble *thought_bubble) {
  thought_bubble_ = thought_bubble;
  drawer_.init();
  drawer_.setDisplayPriority(3);
  Renderer::instance().addParticles(&drawer_);
}

void ParticleSystem::addEmitters(int num_particles) {
  for(int i = 0; i < num_particles; i++) {
    Emitter to_add;
    to_add.color = glm::vec4(randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), 0.4f);
    to_add.position = glm::vec2();
    to_add.velocity = randomDirection() * 0.4f;
    to_add.particles_per_second = 150.0f;
    to_add.leftover_from_last_frame = 0.0f;
    emitters_.push_back(to_add);
  }
}

void ParticleSystem::update(float delta_time, GameState *state) {
  for (list<Emitter>::iterator it = emitters_.begin(); it != emitters_.end(); ++it) {
    glm::vec2 old_position = it->position;
    it->position += it->velocity * delta_time;
    thought_bubble_->collideEmitter(*it, old_position);
    addParticles(*it, delta_time);
  }
  ParticleDrawInfo *data = new ParticleDrawInfo[particles_.size()];
  int index = 0;
  for (list<Particle>::iterator it = particles_.begin(); it != particles_.end(); ++it) {
    if (it->age > it->lifetime) {
      it = particles_.erase(it);
    } else {
      glm::vec2 old_position = it->position;
      it->position += it->velocity * delta_time;
      it->age += delta_time;
      it->color.a -= delta_time * 0.6f;
      data[index].color = it->color;
      data[index].position = it->position;
    }
    index++;
  }
  drawer_.sendParticles(data, particles_.size());
  delete data;
  drawer_.setTransform(translate2D(glm::mat3(1.0f), thought_bubble_->center()));
}

void ParticleSystem::addParticles(Emitter &emitter, float delta_time) {
  float float_number_to_add = emitter.particles_per_second * delta_time + emitter.leftover_from_last_frame;
  int number_to_add = static_cast<int>(float_number_to_add);
  emitter.leftover_from_last_frame = float_number_to_add - number_to_add;
  for (int i = 0; i < number_to_add; i++) {
    Particle to_add;
    to_add.color = emitter.color;
    to_add.position = emitter.position;
    to_add.velocity = randomDirection() * 0.02f;
    to_add.lifetime = 1.0f;
    to_add.age = 0;
    particles_.push_back(to_add);
  }
}