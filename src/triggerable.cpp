#include "triggerable.h"

#include "GL/glfw.h"
#include "gtc/type_ptr.hpp"

static const float kRenderWidth = 0.1f;
static const float kRenderHeight = 0.1f;
static const glm::vec3 kInactiveColor(0.8f, 0.8f, 0.8f);
static const glm::vec3 kActiveColor(0.2f, 0.2f, 0.2f);

Triggerable::Triggerable()
    : time_animating_(0.0f),
      color_(kInactiveColor),
      animating_(false) {}

Triggerable::~Triggerable() {}

void Triggerable::init(Renderer *renderer, float trigger_point, float animation_duration) {
  renderer_ = renderer;
  trigger_point_ = trigger_point;
  animation_duration_ = animation_duration;
  renderer_->addDrawable(this);
}

void Triggerable::update(float delta_time) {
  if (animating_) {
    time_animating_ += delta_time;
    if (time_animating_ > animation_duration_) {
      color_ = kActiveColor;
      animating_ = false;
    } else {
      color_ = glm::mix(kInactiveColor, kActiveColor, time_animating_ / animation_duration_);
    }
  }
}

void Triggerable::trigger() {
  animating_ = true;
}

void Triggerable::draw() {
  glColor3fv(glm::value_ptr(color_));
  glRectf(trigger_point_, 0.0f, trigger_point_ + kRenderWidth, kRenderHeight);
}
