#include "particle_system.h"

#include <cmath>
#include <GL/glew.h>
#include <gtc/type_ptr.hpp>
#include <gtx/color_space.hpp>

#include "transform2D.h"

static inline float randomFloat(float min, float max) {
  return min + rand()/(RAND_MAX/(max - min));
}

static inline glm::vec2 randomDirection() {
  glm::vec2 direction = glm::vec2(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f));
  return glm::normalize(direction);
}

static inline glm::vec2 fastRandomDirection() {
  return glm::vec2(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f));
}

static const int kMaxParticles = 10000;

static inline glm::vec3 randomColor() {
  //float hue = randomFloat(0.0f, 180.0f);
  //float saturation = randomFloat(0.5f, 1.0f);
  //float value = randomFloat(0.5f, 1.0f);
  //glm::vec3 hsv(hue, saturation, value);
  //return glm::rgbColor(hsv);
  return glm::vec3(randomFloat(0.2f, 1.0f), randomFloat(0.2f, 1.0f), randomFloat(0.2f, 1.0f));
}

ParticleSystem::ParticleSystem() : targets_(false) {
  render_data_ = new ParticleDrawInfo[kMaxParticles];
}

ParticleSystem::~ParticleSystem() {
  delete render_data_;  
}

void ParticleSystem::init(ThoughtBubble *thought_bubble) {
  thought_bubble_ = thought_bubble;
  drawer_.init();
  drawer_.setDisplayPriority(3);
  Renderer::instance().addParticles(&drawer_);
}

void ParticleSystem::addEmitters(int num_emitters) {
  for(int i = 0; i < num_emitters; i++) {
    Emitter to_add;
    to_add.color = glm::vec4(randomColor(), 0.4f);
    to_add.position = glm::vec2();
    to_add.velocity = randomDirection() * 0.4f;
    to_add.particles_per_second = 150.0f;
    to_add.leftover_from_last_frame = 0.0f;
    to_add.heat = 3.0f;
    emitters_.push_back(to_add);
  }
}

void ParticleSystem::update(float delta_time, GameState *state) {
  if (targets_) {
    for (list<Emitter>::iterator it = emitters_.begin(); it != emitters_.end(); ++it) {
      if (it->time_in_flight < it->time_to_target) {
        float t = it->time_in_flight / it->time_to_target;
        glm::vec2 start_mid = glm::mix(it->start, it->midway, t);
        glm::vec2 mid_target = glm::mix(it->midway, it->target, t);
        it->position = glm::mix(start_mid, mid_target, t);
        it->time_in_flight += delta_time;
        addParticles(*it, delta_time);
      }
    }
  } else {
    for (list<Emitter>::iterator it = emitters_.begin(); it != emitters_.end(); ++it) {
      glm::vec2 old_position = it->position;
      it->position += it->velocity * delta_time;
      it->heat = glm::max(1.0f, it->heat - 4.0f * delta_time);
      thought_bubble_->collideEmitter(*it, old_position);
      addParticles(*it, delta_time);
    }
  }
  int index = 0;
  for (list<Particle>::iterator it = particles_.begin(); it != particles_.end();) {
    if (it->age > it->lifetime) {
      it = particles_.erase(it);
    } else {
      glm::vec2 old_position = it->position;
      it->position += it->velocity * delta_time;
      it->age += delta_time;
      it->color.a -= delta_time * it->alpha_decay;
      render_data_[index].color = it->color;
      render_data_[index].position = it->position;
      ++it;
      index++;
    }
  }
  drawer_.sendParticles(render_data_, particles_.size());
  drawer_.setTransform(translate2D(glm::mat3(1.0f), thought_bubble_->center()));
}

void ParticleSystem::addParticles(Emitter &emitter, float delta_time) {
  float float_number_to_add = emitter.particles_per_second * emitter.heat * delta_time + emitter.leftover_from_last_frame;
  int number_to_add = static_cast<int>(float_number_to_add);
  emitter.leftover_from_last_frame = float_number_to_add - number_to_add;
  for (int i = 0; i < number_to_add; i++) {
    if (particles_.size() > kMaxParticles) return;
    Particle to_add;
    to_add.color = emitter.color;
    to_add.color.a*=emitter.heat;
    to_add.alpha_decay = 0.6f + (emitter.heat - 1.0f) * 0.8f;
    to_add.position = emitter.position;
    to_add.velocity = fastRandomDirection() * 0.02f;
    to_add.lifetime = 1.5f;
    to_add.age = 0;
    particles_.push_back(to_add);
  }
}

void ParticleSystem::setTargets(vector<Target> &targets) {
  targets_ = true;
  size_t index = 0;
  for (list<Emitter>::iterator it = emitters_.begin(); it != emitters_.end(); ++it) {
    Emitter *emitter = &(*it);
    emitter->start = emitter->position;
    if (index < targets.size()) {
      Target target = targets[index];
      // Particle system coordinate system is centered on the thought bubble.
      // Transform target to that space.
      emitter->target = target.position - thought_bubble_->center();
      target_to_emitter_[target.id] = emitter;
    } else {
      Renderer &renderer = Renderer::instance();
      glm::vec2 randomScreenSpace(renderer.getLeftOfWindow() + randomFloat(0.0f, 1.0f) * renderer.windowWidth(),
        randomFloat(0.0f, 1.0f));
      // Transform to particle sys coordinate system.
      emitter->target = randomScreenSpace - thought_bubble_->center();
    }
    emitter->midway = glm::mix(emitter->start, emitter->target, 0.8f);
    emitter->midway += randomDirection() * 0.1f;
    emitter->time_in_flight = 0.0f;
    emitter->time_to_target = glm::distance(emitter->start, emitter->target) * 3.0f;
    index++;
  }
}

bool ParticleSystem::targetWasHit(Target target) {
  if (target_to_emitter_.count(target.id) > 0) {
    Emitter *emitter = target_to_emitter_[target.id];
    return emitter->time_in_flight > emitter->time_to_target;
  }
  return true;
}
