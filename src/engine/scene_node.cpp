#include "engine/scene_node.h"

#include <algorithm>

SceneNode::SceneNode()
  : relative_transform_(1.0f),
    priority_(0.0f),
    is_occluder_(true),
    occluder_color_(0.0f),
    is_3D_stencil_(false),
    is_visible_(true),
    is_locking_(false),
    is_locked_(false),
    parent_(NULL) {}

SceneNode::~SceneNode() {
  vector<SceneNode *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    (*it)->parent_ = NULL;
  }
  if (parent_ != NULL) {
    parent_->removeChild(this);
  }
}

bool SceneNode::onScreen() {
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

void SceneNode::setParent(SceneNode *parent) {
  if (parent_ != NULL) parent_->removeChild(this);
  parent_ = parent;
  if (parent_ != NULL) parent_->addChild(this);
}

void SceneNode::lockChildren() {
  is_locking_ = true;
  vector<SceneNode *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    (*it)->lock(this);
  }
}

void SceneNode::lock(SceneNode *locking_ancestor) {
  locking_ancestor_ = locking_ancestor;
  is_locked_ = true;
  vector<SceneNode *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    (*it)->lock(locking_ancestor);
  }
}

struct PrioritySortFunctor {
  bool operator() (const SceneNode *left, const SceneNode *right) {
    return left->displayPriority() < right->displayPriority();
  }
};

void SceneNode::getSortedVisibleDescendants(vector<SceneNode *> *drawables) {
  // Get and sort all descendants.
  vector<SceneNode *> sorted_drawables;
  vector<SceneNode *> non_locked;
  getNonLockedDescendants(&non_locked);
  std::stable_sort(non_locked.begin(), non_locked.end(), PrioritySortFunctor());
  vector<SceneNode *>::iterator it;
  for (it = non_locked.begin(); it != non_locked.end(); ++it) {
    if ((*it)->isLocking()) { 
      vector<SceneNode *> sub_tree;
      (*it)->getDescendants(&sub_tree);
      std::stable_sort(sub_tree.begin(), sub_tree.end(), PrioritySortFunctor());
      sorted_drawables.insert(sorted_drawables.end(), sub_tree.begin(), sub_tree.end());
    } else {
      sorted_drawables.push_back(*it);
    }
  }
  // Cull out offsceen nodes.
  for (it = sorted_drawables.begin(); it != sorted_drawables.end(); ++it) {
    if ((*it)->onScreen()) drawables->push_back(*it);
  }
}

void SceneNode::getDescendants(vector<SceneNode *> *drawables) {
  drawables->push_back(this);
  vector<SceneNode *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    (*it)->getDescendants(drawables);
  }
}

void SceneNode::getNonLockedDescendants(vector<SceneNode *> *drawables) {
  drawables->push_back(this);
  if (this->isLocking()) return;
  vector<SceneNode *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    (*it)->getNonLockedDescendants(drawables);
  }
}

glm::mat3 SceneNode::fullTransform() {
  if (parent_ == NULL) return relative_transform_;
  return parent_->fullTransform() * relative_transform_;
}

// These conditions for visible, occluding etc. seem kinda stupid. Change them?
bool SceneNode::isVisible() {
  if (isLocked()) return is_visible_ && locking_ancestor_->isVisible();
  return is_visible_;
}

bool SceneNode::isOccluder() {
  if (isLocked()) return is_occluder_ && is_visible_ && locking_ancestor_->isOccluder();
  return is_occluder_ && is_visible_;
}

bool SceneNode::is3DStencil() {
  if (isLocked()) return is_3D_stencil_ && locking_ancestor_->isVisible();
  return is_3D_stencil_;
}

void SceneNode::addChild(SceneNode *child) {
  children_.push_back(child);
}

// Ineffecient, but unless we build a large and dynamic scene graph shouldn't matter.
void SceneNode::removeChild(SceneNode *child) {
  vector<SceneNode *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    if (*it == child) {
      children_.erase(it);
      return;
    }
  }
}
