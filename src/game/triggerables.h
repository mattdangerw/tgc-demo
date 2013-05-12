#ifndef SRC_TRIGGERABLE_H_
#define SRC_TRIGGERABLE_H_

#include <string>
#include <glm/glm.hpp>

#include "game/character.h"
#include "game/ideas.h"
#include "game/game_entity.h"

using std::string;

class Triggerable {
  public:
    Triggerable();
    ~Triggerable();
    void init(float trigger_point, float animation_duration, GameState trigger_state, int emitters_to_add);
    void update(float delta_time);
    float triggerPoint() { return trigger_point_; }
    float animationDuration() { return animation_duration_; }
    bool doneAnimating() { return !animating_; }
    bool halfDone() { return time_animating_ > animation_duration_ / 2.0f; }
    void trigger();
    GameState triggerState() { return trigger_state_; }
    int emittersToAdd() { return emitters_to_add_; }

  private:
    GameState trigger_state_;
    glm::vec3 color_;
    float trigger_point_;
    float animation_duration_;
    float time_animating_;
    bool animating_;
    int emitters_to_add_;
};

class TriggerableManager : public GameEntity {
  public:
    TriggerableManager();
    ~TriggerableManager();
    void init(Character *character, IdeaManager *idea_manager);
    void update(float delta_time, GameState *state);

  private:
    vector<Triggerable *> triggerables_;
    unsigned int current_triggerable_, emitters_to_add_;
    Character *character_;
    IdeaManager *idea_manager_;
    bool emitted_;
};

#endif  // SRC_TRIGGERABLE_H_
