#include "game_state/tree.h"

#include "util/transform2D.h"

Tree::Tree() {}

Tree::~Tree() {}

void Tree::init() {
  trunk_quad_.init("content/textures/bark.dds", glm::vec2(0.2f));
  trunk_shape_.init("content/paths/trunk.path", &trunk_quad_);
  trunk_shape_.setParent(this);
  leaves_quad_.init("content/textures/leaves.dds", glm::vec2(0.2f));
  leaves_shape_.init("content/paths/leaves.path", &leaves_quad_);
  leaves_shape_.setParent(this);
  glm::mat3 transform(1.0f);
  transform = translate2D(transform, glm::vec2(-0.25f, 0.8f));
  transform = scale2D(transform, glm::vec2(0.8f));
  leaves_shape_.setRelativeTransform(transform);
  lockChildren();
}

void Tree::setColor(glm::vec4 color){
  trunk_quad_.useTexture("content/textures/leaves.dds");
  trunk_quad_.setColorMask(glm::vec4(0.9f, 0.7f, 0.4f, 1.0f));
  leaves_quad_.useTexture("content/textures/bark.dds");
  leaves_quad_.setColorMask(color);
}
