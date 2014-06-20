#ifndef SRC_GAME_ENTITY_H_
#define SRC_GAME_ENTITY_H_

#include <glm/glm.hpp>
#include <vector>

using std::vector;

class GameEntity;

struct Target {
  glm::vec2 position;
  int id;
  GameEntity *entity;
};

enum GameState {
  WALKING,
  TRIGGERING,
  TRIGGERING_JUMPING,
  PRE_EXPLODING,
  EXPLODING,
  ENDING
};

class GameEntity {
  public:
    virtual ~GameEntity() {}
    // Make the GL calls to draw this object.
    virtual void update(float delta_time, GameState *state) = 0;
    virtual void getTargets(vector<Target> *targets) {}
    virtual void colorTarget(Target target) {}
};

#endif  // SRC_GAME_ENTITY_H_
