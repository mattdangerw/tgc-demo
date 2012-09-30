#include "tree.h"

#include "transform2D.h"

Tree::Tree() {}

Tree::~Tree() {}

void Tree::init() {
  trunk_quad_.init("textures/bark.dds");
  trunk_quad_.setTextureScale(glm::vec2(0.2f));
  trunk_shape_.init("paths/trunk.path", &trunk_quad_, true, false);
  leaves_quad_.init("textures/leaves.dds");
  leaves_quad_.setTextureScale(glm::vec2(0.2f));
  leaves_shape_.init("paths/leaves.path", &leaves_quad_, true, false);
  glm::mat3 transform(1.0f);
  transform = translate2D(transform, glm::vec2(-0.25f, 0.8f));
  transform = scale2D(transform, glm::vec2(0.8f));
  leaves_shape_.setTransform(transform);
}

void Tree::draw(glm::mat3 transform) {
  transform = transform * transform_;
  trunk_shape_.draw(transform);
  leaves_shape_.draw(transform);
}