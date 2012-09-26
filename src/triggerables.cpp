#include "triggerables.h"

#include <algorithm>
#include <GL/glew.h>
#include <gtc/type_ptr.hpp>


Triggerable::Triggerable()
    : time_animating_(0.0f),
      animating_(false) {}

Triggerable::~Triggerable() {}

void Triggerable::init(Renderer *renderer, float trigger_point, float animation_duration) {
  renderer_ = renderer;
  trigger_point_ = trigger_point;
  animation_duration_ = animation_duration;
}

void Triggerable::update(float delta_time) {
  if (animating_) {
    time_animating_ += delta_time;
    if (time_animating_ > animation_duration_) {
      animating_ = false;
    } else {
      // TODO: update drawable
    }
  }
}

void Triggerable::trigger() {
  animating_ = true;
}

TriggerableManager::TriggerableManager()
    : current_triggerable_(0) {}

TriggerableManager::~TriggerableManager() {
  for (vector<Triggerable *>::iterator it = triggerables_.begin(); it != triggerables_.end(); ++it) {
    delete (*it);
  }
}

static bool sortTriggerables(Triggerable *a, Triggerable *b) {
  return a->triggerPoint() < b->triggerPoint();
}

void TriggerableManager::init(Renderer *renderer, Character *character, ParticleSystem *particle_system) {
  character_ = character;
  particle_system_ = particle_system;
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(renderer, 0.5f, 1.0f);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(renderer, 1.0f, 0.5f);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(renderer, 1.5f, 0.5f);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(renderer, 2.2f, 3.0f);
  std::sort(triggerables_.begin(), triggerables_.end(), sortTriggerables);
}

void TriggerableManager::update(float delta_time, GameState *state) {
  if (*state == TRIGGERING) {
    if (triggerables_[current_triggerable_]->doneAnimating()) {
      *state = WALKING;
      current_triggerable_++;
    }
  } else if (*state == WALKING && current_triggerable_ < triggerables_.size()) {
    Triggerable *to_check = triggerables_[current_triggerable_];
    if (character_->position().x > to_check->triggerPoint()) {
      to_check->trigger();
      if (current_triggerable_ == triggerables_.size() - 1) {
        *state = EXPLODING;
      } else {
        *state = TRIGGERING;
        particle_system_->addParticles(50);
      }
    }
  }

  vector<Triggerable *>::iterator it;
  for (it = triggerables_.begin(); it != triggerables_.end(); ++it) {
    (*it)->update(delta_time);
  }
}