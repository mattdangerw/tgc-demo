#ifndef SRC_BACKGROUND_H_
#define SRC_BACKGROUND_H_

#include <glm/glm.hpp>

#include "engine/shape.h"
#include "engine/shape_group.h"
#include "engine/entity.h"
#include "engine/quad.h"

class Background : public Entity {
  public:
    Background();
    ~Background();
    void init();
  private:
    // Drawables.
    Quad quad_;
};

#endif  // SRC_BACKGROUND_H_
