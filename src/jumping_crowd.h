#ifndef SRC_JUMPING_CROWD_H_
#define SRC_JUMPING_CROWD_H_

#include <glm.hpp>
#include <vector>

#include "circles.h"
#include "character.h"
#include "ground.h"
#include "game_entity.h"

using std::vector;

class JumpingCrowd : public GameEntity {
  public:
    JumpingCrowd();
    ~JumpingCrowd();
    void init(Character *character, Ground *Ground, vector<float> positions, float size, float jump_time_min, float jump_time_max, float jump_velocity);
    void update(float delta_time, GameState *state);
  private:
    float jumpTime();
    Ground *ground_;
    Character *character_;
    float start_, end_, time_min_, time_max_, initial_jump_velocity_;
    vector<Circle> crowd_;
    CircleDrawer drawer_;
    vector<float> time_on_ground_, ground_height_, time_till_next_jump_, velocity_;
    vector<bool> jumping_;
};

#endif  // SRC_JUMPING_CROWD_H_
