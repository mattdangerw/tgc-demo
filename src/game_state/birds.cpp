#include "game_state/birds.h"

#include "util/transform2D.h"

BirdManager::BirdManager() {}

BirdManager::~BirdManager() {}

void BirdManager::init() {
  Entity::init();
  bird_.init("bird.group");
  bird_.animator().queueAnimation("fly");
  //bird_.animator().queueAnimation("slow_change");
  bird_.setParent(theRenderer().rootNode());
  x = 2.0;
}

void BirdManager::update(float delta_time) {
  bird_.animator().update(delta_time);
  x -= delta_time;
  glm::mat3 shape_transform = scale2D(translate2D(glm::mat3(1.0f), glm::vec2(x, 0.5f)), glm::vec2(0.2f));
  bird_.setRelativeTransform(shape_transform);
}