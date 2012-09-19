#include "cloud.h"
#include "GL/glfw.h"

Cloud::Cloud() {}

Cloud::~Cloud() {}

void Cloud::init(Renderer *renderer) {
  renderer_ = renderer;
  renderer_->addDrawable(this);
}

void Cloud::update(float delta_time) {
  position_.x -= velocity_ * delta_time;
}

void Cloud::xExtent(float *x_begin, float *x_end) {
  *x_begin = position_.x;
  *x_end = position_.x + size_;
}

void Cloud::draw() {
  glColor3f(0.5f, 0.5f, 0.5f);
  glRectf(position_.x, position_.y, position_.x + size_, position_.y + size_);
}
