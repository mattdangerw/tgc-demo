#ifndef SRC_BACKGROUND_H_
#define SRC_BACKGROUND_H_

#include <glm/glm.hpp>

#include "game_state/game_entity.h"
#include "render/path_shape.h"
#include "render/shape_group.h"
#include "render/quad.h"

class Background {
  public:
    Background();
    ~Background();
    void init();
  private:
    // Drawables.
    Quad quad_;
};

#endif  // SRC_BACKGROUND_H_
