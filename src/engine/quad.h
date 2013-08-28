#ifndef SRC_QUAD_H_
#define SRC_QUAD_H_

#include <glm/glm.hpp>

#include "engine/entity.h"

class Quad : public Entity {
  public:
    Quad() : min_(0.0f), max_(1.0f) {}
    ~Quad() {}
    void init() {}
    void extent(glm::vec2 *min, glm::vec2 *max) { *min = min_; *max = max_; }
    void setExtent(glm::vec2 min, glm::vec2 max) { min_ = min; max_ = max; }
    void draw() { fill()->fillIn(this); }
    void drawOccluder() { fill()->fillInOccluder(this); }
  private:
    glm::vec2 min_, max_;
};

#endif  // SRC_QUAD_H_
