#ifndef SRC_SCROLLER_H_
#define SRC_SCROLLER_H_

#include "engine/entity.h"
#include "engine/path_shape.h"
#include "engine/shape_group.h"
#include "engine/quad.h"

class Scroller : public Entity {
  public:
    Scroller();
    ~Scroller();
    void init();
    void update(float delta_time);
};

#endif  // SRC_SCROLLER_H_
