#include "ground.h"

#include <GL/glew.h>
#include <gtc/type_ptr.hpp>

Ground::Ground() {
  renderer_ = NULL;
}

Ground::~Ground() {}

void Ground::init(Renderer *renderer) {
  this->renderer_ = renderer;
  // TODO: load from file or something.
  points_.push_back(glm::vec2(0.0f, 0.2f));
  points_.push_back(glm::vec2(0.3f, 0.1f));
  points_.push_back(glm::vec2(0.6f, 0.4f));
  points_.push_back(glm::vec2(0.9f, 0.1f));
  points_.push_back(glm::vec2(1.4f, 0.2f));
  points_.push_back(glm::vec2(1.8f, 0.3f));
  points_.push_back(glm::vec2(2.4f, 0.3f));
}

float Ground::width() {
  return points_.back().x;
}

float Ground::heightAt(float x) {
  if (x < 0.0f || x > width()) return 0.0f;
  size_t index;
  for (index = 0; index < points_.size() - 1; index++) {
    if (points_[index+1].x > x) break;
  }
  glm::vec2 left = points_[index], right = points_[index+1];
  return glm::mix(left.y, right.y, (x - left.x) / (right.x - left.x));
}
