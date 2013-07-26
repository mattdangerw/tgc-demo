#ifndef SRC_SCROLLER_H_
#define SRC_SCROLLER_H_

#include "engine/entity.h"
#include "engine/shape.h"
#include "engine/shape_group.h"
#include "engine/quad.h"

class Scroller : public Entity {
  public:
    Scroller();
    ~Scroller();
    void init();
    float getScroll() { return scroll_; }
    void update(float delta_time);

  private:
    float character_screen_x_;
    float scroll_;
};

#endif  // SRC_SCROLLER_H_
