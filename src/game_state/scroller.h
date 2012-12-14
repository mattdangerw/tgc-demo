#ifndef SRC_SCROLLER_H_
#define SRC_SCROLLER_H_

#include "game_state/entity.h"
#include "render/path_shape.h"
#include "render/shape_group.h"
#include "render/quad.h"

class Scroller : public Entity {
  public:
    Scroller();
    ~Scroller();
    void init();
    void update(float delta_time);
};

#endif  // SRC_SCROLLER_H_
