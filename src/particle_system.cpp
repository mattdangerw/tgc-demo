#include "particle_system.h"

#include <cmath>
#include <stdlib.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

#include "transform2D.h"
#include "random.h"

static const int kMaxParticles = 10000;
static const float kNearZBoundary = -2.2f;
static const float kFarZBoundary = -4.0f;

static inline glm::vec3 projectPoint(glm::vec3 point) {
  glm::vec4 projected = Renderer::instance().projection() * glm::vec4(point, 1.0);
  return glm::vec3(projected.x, projected.y, projected.z) / projected.w;
}

static inline glm::vec3 unProjectPoint(glm::vec3 point) {
  glm::vec4 unprojected = Renderer::instance().inverseProjection() * glm::vec4(point, 1.0);
  return glm::vec3(unprojected.x, unprojected.y, unprojected.z) / unprojected.w;
}

void EmitterTrack::addDestination(glm::vec3 control, glm::vec3 destination, float time) {
  controls_.push_back(control);
  destinations_.push_back(destination);
  times_.push_back(time);
}

glm::vec3 EmitterTrack::step(float delta_time) {
  if (done()) return start_;
  current_time_+=delta_time;
  float segment_time = times_[segment_];
  glm::vec3 control = controls_[segment_];
  glm::vec3 destination = destinations_[segment_];
  if (current_time_ > segment_time) {
    current_time_ = 0.0f;
    ++segment_;
    start_ = destination;
    return destination;
  }
  float t = current_time_ / segment_time;
  glm::vec3 midpoint1, midpoint2;
  midpoint1 = glm::mix(start_, control, t);
  midpoint2 = glm::mix(control, destination, t);
  return glm::mix(midpoint1, midpoint2, t);
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
  Renderer::instance().addDrawable3D(&drawer_);
}

void ParticleSystem::addEmitters(int num_emitters) {
  for(int i = 0; i < num_emitters; ++i) {
    Emitter to_add;
    to_add.color = glm::vec4(randomColor(), 0.4f);
    to_add.position = glm::vec3(0.0f, 0.0f, kFarZBoundary);
    to_add.velocity = glm::vec3(randomDirection() * 0.2f, 0.08f);
    to_add.particles_per_second = 150.0f;
    to_add.leftover_from_last_frame = 0.0f;
    to_add.heat = 3.0f;
    emitters_by_depth_.push_back(emitters_.size());
    emitters_.push_back(to_add);
  }
}

void ParticleSystem::update(float delta_time, GameState *state) {
  updateEmitters(delta_time);
  updateParticles(delta_time);
}

void ParticleSystem::updateEmitters(float delta_time) {
  if (targets_) {
    for (size_t i = 0; i < emitters_.size(); ++i) {
      Emitter &emitter = emitters_[i];
      emitter.time_till_escape-=delta_time;
    }
  }
  for (size_t i = 0; i < emitters_.size(); ++i) {
    Emitter &emitter = emitters_[i];
    if (targets_ && emitter.time_till_escape < 0.0f) {
      if (!emitter.track.done()) {
        emitter.position = emitter.track.step(delta_time);
        addParticles(i, delta_time);
      }
    } else {
      emitter.position += emitter.velocity * delta_time;
      emitter.heat = glm::max(1.0f, emitter.heat - 4.0f * delta_time);
      glm::vec3 projected_position = projectPoint(emitter.position);
      glm::vec2 position2D(projected_position.x, projected_position.y);
      glm::vec2 velocity2D(emitter.velocity.x, emitter.velocity.y);
      if (thought_bubble_->collideEmitter(&position2D, &velocity2D)) {
        emitter.position = unProjectPoint(glm::vec3(position2D.x, position2D.y, projected_position.z));
        emitter.velocity = glm::vec3(velocity2D.x, velocity2D.y, emitter.velocity.z);
      }
      if (emitter.position.z > kNearZBoundary) {
        emitter.position.z = kNearZBoundary;
        emitter.velocity.z *= -1.0f;
      } else if (emitter.position.z < kFarZBoundary) {
        emitter.position.z = kFarZBoundary;
        emitter.velocity.z *= -1.0f;
      }
      addParticles(i, delta_time);
    }
  }
}

void ParticleSystem::updateParticles(float delta_time) {
  int render_data_index = 0;
  sortDepthIndex();
  for (vector<int>::iterator depth_iter = emitters_by_depth_.begin(); depth_iter != emitters_by_depth_.end(); ++depth_iter) {
    int emitter_index = *depth_iter;
    list<Particle> &particle_list = emitter_particles_[emitter_index];
    for (list<Particle>::iterator it = particle_list.begin(); it != particle_list.end();) {
      if (it->age > it->lifetime) {
        it = particle_list.erase(it);
      } else {
        glm::vec3 old_position = it->position;
        it->position += it->velocity * delta_time;
        it->age += delta_time;
        it->color.a -= delta_time * it->alpha_decay;
        render_data_[render_data_index].color = it->color;
        render_data_[render_data_index].position = it->position;
        ++it;
        ++render_data_index;
      }
    }
  }
  drawer_.sendParticles(render_data_, render_data_index);
  drawer_.setTransform2D(translate2D(glm::mat3(1.0f), thought_bubble_->center()));
}

void ParticleSystem::addParticles(int emitter_index, float delta_time) {
  Emitter &emitter = emitters_[emitter_index];
  float float_number_to_add = emitter.particles_per_second * emitter.heat * delta_time + emitter.leftover_from_last_frame;
  int number_to_add = static_cast<int>(float_number_to_add);
  emitter.leftover_from_last_frame = float_number_to_add - number_to_add;
  for (int i = 0; i < number_to_add; ++i) {
    // TODO: get that line back in.
    //if (particles_.size() > kMaxParticles) return;
    Particle to_add;
    to_add.color = emitter.color;
    to_add.color.a*=emitter.heat;
    to_add.alpha_decay = 0.6f + (emitter.heat - 1.0f) * 0.8f;
    to_add.position = emitter.position;
    to_add.velocity = fastRandomDirection3D() * 0.02f;
    to_add.lifetime = 1.5f;
    to_add.age = 0;
    emitter_particles_[emitter_index].push_back(to_add);
  }
}

struct DepthSortFunctor {
  bool operator() (const int left, const int right) {
    return emitters->at(left).position.z < emitters->at(right).position.z;
  }
  vector<Emitter> *emitters;
};

void ParticleSystem::sortDepthIndex() {
  DepthSortFunctor functor;
  functor.emitters = &emitters_;
  std::sort(emitters_by_depth_.begin(), emitters_by_depth_.end(), functor);
}

void ParticleSystem::setTargets(vector<Target> &targets) {
  targets_ = true;
  float time_till_escape = 0.0f;
  float delta_escape = 1.0f;
  for (size_t i = 0; i < emitters_.size(); ++i) {
    Emitter &emitter = emitters_[i];
    emitter.track.setStart(emitter.position);
    glm::vec3 control = glm::vec3(0.0f, 0.0f, -1.7f);
    glm::vec3 destination = glm::vec3(0.0, 0.0, -1.1f);
    emitter.track.addDestination(control, destination, 1.0f);
    control = glm::vec3(0.0f, 0.0f, -0.7f);
    if (i < targets.size()) {
      Target target = targets[i];
      // Particle system coordinate system is centered on the thought bubble.
      // Transform target to that space.
      destination = unProjectPoint(glm::vec3(target.position - thought_bubble_->center(), 0.81f));
      target_to_emitter_[target.id] = i;
      emitter.time_till_escape = time_till_escape;
      time_till_escape+=delta_escape;
      delta_escape*=0.85f;
    } else {
      Renderer &renderer = Renderer::instance();
      glm::vec2 randomScreenSpace(renderer.getLeftOfWindow() + randomFloat(0.0f, 1.0f) * renderer.windowWidth(),
        randomFloat(0.0f, 1.0f));
      // Transform to particle sys coordinate system.
      destination = unProjectPoint(glm::vec3(randomScreenSpace - thought_bubble_->center(), 0.81f));
      // Release at random time over the interval of real targets.
      emitter.time_till_escape = randomFloat(0.0f, time_till_escape - delta_escape);
    }
    emitter.track.addDestination(control, destination, 1.5f);
    //emitter.midway = glm::mix(emitter.start, emitter.target, 0.8f);
    //emitter.midway += randomDirection3D() * 0.1f;
    //emitter.time_in_flight = 0.0f;
    //emitter.time_to_target = glm::distance(emitter.start, emitter.target) * 3.0f;
  }
}

bool ParticleSystem::targetWasHit(Target target) {
  if (target_to_emitter_.count(target.id) > 0) {
    Emitter &emitter = emitters_[target_to_emitter_[target.id]];
    return emitter.track.done();
  }
  return true;
}
