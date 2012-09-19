#ifndef SRC_TRIGGERABLE_H_
#define SRC_TRIGGERABLE_H_

#include <string>

#include "glm.hpp"
#include "renderer.h"
#include "character.h"
#include "particle_system.h"
#include "game_entity.h"

using std::string;

class Triggerable : Drawable {
  public:
    Triggerable();
    ~Triggerable();
    void init(Renderer *renderer, float trigger_point, float animation_duration);
    void update(float delta_time);
    float triggerPoint() { return trigger_point_; }
    float animationDuration() { return animation_duration_; }
    float doneAnimating() { return !animating_; }
    void trigger();
    void draw();
    string shadingGrounp() { return "default"; }

  private:
    Renderer *renderer_;
    glm::vec3 color_;
    float trigger_point_;
    float animation_duration_;
    float time_animating_;
    bool animating_;
};

class TriggerableManager : GameEntity {
  public:
    TriggerableManager();
    ~TriggerableManager();
    void init(Renderer *renderer, Character *character, ParticleSystem *particle_system);
    void update(float delta_time, GameState *state);

  private:
    vector<Triggerable *> triggerables_;
    unsigned int current_triggerable_;
    Character *character_;
    ParticleSystem *particle_system_;
};

#endif  // SRC_TRIGGERABLE_H_
