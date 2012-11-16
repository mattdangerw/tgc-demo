#include "game_state/tree.h"

#include "util/transform2D.h"

Tree::Tree() {}

Tree::~Tree() {}

void Tree::init() {
  trunk_quad_.init();
  trunk_quad_.useTexture("content/textures/bark.dds");
  trunk_quad_.setTextureScale(glm::vec2(0.2f));
  trunk_shape_.init("content/paths/trunk.path", &trunk_quad_);
  trunk_shape_.setParent(this);
  trunk_shape_.setIsVisible(false);
  leaves_quad_.init();
  leaves_quad_.useTexture("content/textures/leaves.dds");
  leaves_quad_.setTextureScale(glm::vec2(0.2f));
  leaves_shape_.init("content/paths/leaves.path", &leaves_quad_);
  leaves_shape_.setParent(this);
  leaves_shape_.setIsVisible(false);
  glm::mat3 transform(1.0f);
  transform = translate2D(transform, glm::vec2(-0.25f, 0.8f));
  transform = scale2D(transform, glm::vec2(0.8f));
  leaves_shape_.setRelativeTransform(transform);
}

void Tree::setColor(glm::vec4 color){
  trunk_quad_.useTexture("content/textures/leaves.dds");
  trunk_quad_.setColorMask(glm::vec4(0.9f, 0.7f, 0.4f, 1.0f));
  leaves_quad_.useTexture("content/textures/bark.dds");
  leaves_quad_.setColorMask(color);
}

void Tree::draw() {
  trunk_shape_.draw();
  leaves_shape_.draw();
}

void Tree::drawOccluder() {
  trunk_shape_.drawOccluder();
  leaves_shape_.drawOccluder();
}
