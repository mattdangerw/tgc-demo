#include "particle_system.h"

#include <cmath>
#include <GL/glew.h>
#include <gtc/type_ptr.hpp>

static inline float randomFloat(float min, float max) {
  return min + rand()/(RAND_MAX/(max - min));
}

ParticleSystem::ParticleSystem() {}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::init(Renderer *renderer, ThoughtBubble *thought_bubble) {
  renderer_ = renderer;
  thought_bubble_ = thought_bubble;
}

void ParticleSystem::addParticles(int num_particles) {
  for(int i = 0; i < num_particles; i++) {
    Particle to_add;
    to_add.color = glm::vec3(randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f));
    to_add.position = glm::vec2();
    to_add.velocity = glm::vec2(randomFloat(-0.4f, 0.4f), randomFloat(-0.4f, 0.4f));
    particles_.push_back(to_add);
  }
}

void ParticleSystem::update(float delta_time, GameState *state) {
  vector<Particle>::iterator it;
  for (it = particles_.begin(); it != particles_.end(); ++it) {
    glm::vec2 old_position = it->position;
    it->position += it->velocity * delta_time;
    thought_bubble_->collideParticle(*it, old_position);
  }
}
