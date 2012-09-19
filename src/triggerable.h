#ifndef SRC_TRIGGERABLE_H_
#define SRC_TRIGGERABLE_H_

#include <string>

#include "glm.hpp"
#include "renderer.h"

using std::string;

class Triggerable : Drawable {
  public:
    Triggerable();
    ~Triggerable();
    void init(Renderer *renderer, float trigger_point, float animation_duration);
    void update(float delta_time);
    float triggerPoint() { return trigger_point_; }
    float animationDuration() { return animation_duration_; }
    float doneAnimating() { return !animating_; }
    void trigger();
    void draw();
    string shadingGrounp() { return "default"; }

  private:
    Renderer *renderer_;
    glm::vec3 color_;
    float trigger_point_;
    float animation_duration_;
    float time_animating_;
    bool animating_;
};

#endif  // SRC_TRIGGERABLE_H_
