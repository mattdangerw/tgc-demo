#include "world/birds.h"

#include "util/transform2D.h"

BirdManager::BirdManager() {}

BirdManager::~BirdManager() {}

void BirdManager::init() {
  bird_.init("bird.group");
  bird_.animator().queueAnimation("fly");
  bird_.setParent(this);
  x = 2.0;
  glm::mat3 shape_transform = scale2D(translate2D(glm::mat3(1.0f), glm::vec2(x, 0.5f)), glm::vec2(0.2f));
  bird_.setRelativeTransform(shape_transform);
}

void BirdManager::update(float delta_time) {
  x -= 0.1 * delta_time;
  glm::mat3 shape_transform = scale2D(translate2D(glm::mat3(1.0f), glm::vec2(x, 0.5f)), glm::vec2(0.2f));
  bird_.setRelativeTransform(shape_transform);
}