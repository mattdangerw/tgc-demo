#include "engine/entity.h"

#include <algorithm>

Entity::Entity()
  : relative_transform_(1.0f),
    priority_(0.0f),
    is_occluder_(true),
    occluder_color_(0.0f),
    is_visible_(true),
    do_update_(true),
    fill_(NULL),
    parent_(NULL) {}

Entity::~Entity() {
  vector<Entity *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    (*it)->parent_ = NULL;
  }
  if (parent_ != NULL) {
    parent_->removeChild(this);
  }
}

bool Entity::onScreen() {
  glm::vec2 corners[4];
  extent(corners, corners + 1);
  corners[2] = glm::vec2(corners[0].x, corners[1].y);
  corners[3] = glm::vec2(corners[1].x, corners[0].y);
  glm::mat3 transform = fullTransform();
  glm::vec2 min_coords(std::numeric_limits<float>::max()), max_coords(-std::numeric_limits<float>::max());
  for (int i = 0; i < 4; ++i) {
    glm::vec3 transformed = transform * glm::vec3(corners[i], 1.0f);
    transformed.x /= transformed.z;
    transformed.y /= transformed.z;
    min_coords.x = std::min(transformed.x, min_coords.x);
    min_coords.y = std::min(transformed.y, min_coords.y);
    max_coords.x = std::max(transformed.x, max_coords.x);
    max_coords.y = std::max(transformed.y, max_coords.y);
  }
  return (max_coords.x > -1.0f && max_coords.y > -1.0f && min_coords.x < 1.0f && min_coords.y < 1.0f);
}

void Entity::setParent(Entity *parent) {
  if (parent_ != NULL) parent_->removeChild(this);
  parent_ = parent;
  if (parent_ != NULL) parent_->addChild(this);
}

void Entity::updateAll(float delta_time) {
  if (!doUpdate()) return;
  update(delta_time);
  vector<Entity *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    (*it)->updateAll(delta_time);
  }
}

struct PrioritySortFunctor {
  bool operator() (const Entity *left, const Entity *right) {
    return left->displayPriority() < right->displayPriority();
  }
};

void Entity::drawAll() {
  if (!isVisible()) return;
  if (onScreen()) this->draw();
  // Get and sort the children by priority.
  vector<Entity *> sorted_drawables = children_;
  std::stable_sort(sorted_drawables.begin(), sorted_drawables.end(), PrioritySortFunctor());
  vector<Entity *>::iterator it;
  for (it = sorted_drawables.begin(); it != sorted_drawables.end(); ++it) {
    (*it)->drawAll();
  }
}

void Entity::drawAllOccluders() {
  if (!isVisible() || !isOccluder()) return;
  if (onScreen()) this->drawOccluder();
  // Get and sort the children by priority.
  vector<Entity *> sorted_drawables = children_;
  std::stable_sort(sorted_drawables.begin(), sorted_drawables.end(), PrioritySortFunctor());
  vector<Entity *>::iterator it;
  for (it = sorted_drawables.begin(); it != sorted_drawables.end(); ++it) {
    (*it)->drawAllOccluders();
  }
}

glm::mat3 Entity::fullTransform() {
  if (parent_ == NULL) return relative_transform_;
  return parent_->fullTransform() * relative_transform_;
}

void Entity::addChild(Entity *child) {
  children_.push_back(child);
}

// Inefficient, but unless we build a large and dynamic scene graph shouldn't matter.
void Entity::removeChild(Entity *child) {
  vector<Entity *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    if (*it == child) {
      children_.erase(it);
      return;
    }
  }
}
