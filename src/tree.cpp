#include "tree.h"

#include "transform2D.h"

Tree::Tree() {}

Tree::~Tree() {}

void Tree::init() {
  trunk_quad_.init("textures/bark.dds");
  trunk_quad_.setTextureScale(glm::vec2(0.2f));
  trunk_shape_.init("paths/trunk.path", &trunk_quad_);
  leaves_quad_.init("textures/leaves.dds");
  leaves_quad_.setTextureScale(glm::vec2(0.2f));
  leaves_shape_.init("paths/leaves.path", &leaves_quad_);
  glm::mat3 transform(1.0f);
  transform = translate2D(transform, glm::vec2(-0.25f, 0.8f));
  transform = scale2D(transform, glm::vec2(0.8f));
  leaves_shape_.setTransform(transform);
}

void Tree::draw(glm::mat3 view) {
  view = view * transform();
  trunk_shape_.draw(view);
  leaves_shape_.draw(view);
}

void Tree::drawOcclude(glm::mat3 view) {
  view = view * transform();
  trunk_shape_.drawOcclude(view);
  leaves_shape_.drawOcclude(view);
}

void Tree::setColor(glm::vec4 color){
  trunk_quad_.init("textures/leaves.dds");
  trunk_quad_.setTextureScale(glm::vec2(0.2f));
  trunk_quad_.setColorMask(glm::vec4(0.9f, 0.7f, 0.4f, 1.0f));
  leaves_quad_.init("textures/bark.dds");
  leaves_quad_.setTextureScale(glm::vec2(0.2f));
  leaves_quad_.setColorMask(color);
}
