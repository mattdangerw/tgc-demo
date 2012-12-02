#include "render/scene_node.h"

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

void SceneNode::getSortedDescendants(vector<SceneNode *> *drawables) {
  vector<SceneNode *> non_locked;
  getNonLockedDescendants(&non_locked);
  std::stable_sort(non_locked.begin(), non_locked.end(), PrioritySortFunctor());
  vector<SceneNode *>::iterator it;
  for (it = non_locked.begin(); it != non_locked.end(); ++it) {
    if ((*it)->isLocking()) { 
      vector<SceneNode *> sub_tree;
      (*it)->getDescendants(&sub_tree);
      std::stable_sort(sub_tree.begin(), sub_tree.end(), PrioritySortFunctor());
      drawables->insert(drawables->end(), sub_tree.begin(), sub_tree.end());
    } else {
      drawables->push_back(*it);
    }
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
