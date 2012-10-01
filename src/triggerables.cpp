#include "triggerables.h"

#include <algorithm>
#include <GL/glew.h>
#include <gtc/type_ptr.hpp>


Triggerable::Triggerable()
    : time_animating_(0.0f),
      animating_(false) {}

Triggerable::~Triggerable() {}

void Triggerable::init(float trigger_point, float animation_duration, GameState trigger_state, int emitters_to_add) {
  trigger_point_ = trigger_point;
  animation_duration_ = animation_duration;
  trigger_state_ = trigger_state;
  emitters_to_add_ = emitters_to_add;
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
    : current_triggerable_(0), 
      emitters_to_add_(1) {}

TriggerableManager::~TriggerableManager() {
  for (vector<Triggerable *>::iterator it = triggerables_.begin(); it != triggerables_.end(); ++it) {
    delete (*it);
  }
}

static bool sortTriggerables(Triggerable *a, Triggerable *b) {
  return a->triggerPoint() < b->triggerPoint();
}

void TriggerableManager::init(Character *character, ParticleSystem *particle_system) {
  character_ = character;
  particle_system_ = particle_system;
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(2.1f, 2.0f, TRIGGERING_JUMPING, 1);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(3.1f, 1.0f, TRIGGERING_JUMPING, 2);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(4.2f, 2.0f, TRIGGERING, 2);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(5.58f, 2.0f, TRIGGERING_JUMPING, 3);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(7.0f, 4.0f, TRIGGERING, 2);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(8.4f, 2.0f, TRIGGERING, 4);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(9.91f, 4.0f, TRIGGERING, 5);
  std::sort(triggerables_.begin(), triggerables_.end(), sortTriggerables);
}

void TriggerableManager::update(float delta_time, GameState *state) {
  if (*state == TRIGGERING || *state == TRIGGERING_JUMPING) {
    if (!emitted_ && triggerables_[current_triggerable_]->halfDone()) {
      particle_system_->addEmitters(triggerables_[current_triggerable_]->emittersToAdd());
      emitted_ = true;
    }
    if (triggerables_[current_triggerable_]->doneAnimating()) {
      emitters_to_add_++;
      if (current_triggerable_ == triggerables_.size() - 1) {
        *state = PRE_EXPLODING;
      } else {
        *state = WALKING;
        current_triggerable_++;
      }
    }
  } else if (*state == WALKING && current_triggerable_ < triggerables_.size()) {
    Triggerable *to_check = triggerables_[current_triggerable_];
    if (character_->position().x > to_check->triggerPoint()) {
      to_check->trigger();
      emitted_ = false;
      *state = to_check->triggerState();
    }
  }

  vector<Triggerable *>::iterator it;
  for (it = triggerables_.begin(); it != triggerables_.end(); ++it) {
    (*it)->update(delta_time);
  }
}
