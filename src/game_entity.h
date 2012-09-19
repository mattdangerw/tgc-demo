#ifndef SRC_GAME_ENTITY_H_
#define SRC_GAME_ENTITY_H_

enum GameState {
  WALKING,
  TRIGGERING,
  EXPLODING
};

class GameEntity {
  public:
    virtual ~GameEntity() {}
    // Make the GL calls to draw this object.
    virtual void update(float delta_time, GameState *state) = 0;
};

#endif  // SRC_GAME_ENTITY_H_
