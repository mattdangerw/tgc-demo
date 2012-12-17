#ifndef BIRDS_H_
#define BIRDS_H_

#include "game_state/entity.h"
#include "render/shape_group.h"

class BirdManager : public Entity {
  public:
    BirdManager();
    ~BirdManager();
    void init();
    void update(float delta_time);
  private:
    ShapeGroup bird_;
    float x;
};

#endif  // BIRDS_H_
