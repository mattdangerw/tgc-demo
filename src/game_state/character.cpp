#include "game_state/character.h"

#include <GL/glew.h>

#include "util/transform2D.h"
#include "util/random.h"

#ifdef _DEBUG
static const float kSpeed = 5.0f;
#else
static const float kSpeed = 0.4f;
#endif
static const float kPlayerWidth = 0.008f;
static const float kGrowthRate = 0.004f;
static const float kHeightAboveGround = 0.0f;
static const float kGravity = -8.0f;
static const float kInitialJumpVelocity = 1.2f;

Character::Character()
  : position_(),
    time_on_ground_(0.0f),
    time_till_next_jump_(0) {}

Character::~Character() {}

void Character::init(Ground *ground) {
  circle_vector_.push_back(Circle());
  circle_ = &circle_vector_[0];
  circle_->radius = 0.001f;
  drawer_.init(&circle_vector_);
  drawer_.setParent(theRenderer().rootNode());
  drawer_.setDisplayPriority(5.0f);
  drawer_.setIsVisible(true);
  ground_ = ground;
  position_.x = kPlayerWidth;
  updateY(0.0f);
  updateCircle();
}

void Character::setInput(bool left_down, bool right_down, bool space_pressed) {
  left_down_ = left_down;
  right_down_ = right_down;
  space_pressed_ = space_pressed;
}

void Character::update(float delta_time, GameState *state) {
  if (*state == WALKING) {
    // Update x position.
    if (left_down_) moveLeft(delta_time);
    if (right_down_) moveRight(delta_time);
    // Udpate y postion.
    if (space_pressed_) this->jump();
  }
  if (*state == TRIGGERING_JUMPING || *state == ENDING) {
    if (time_on_ground_ > time_till_next_jump_) {
      this->jump();
      time_till_next_jump_ = randomFloat(0.0f, 0.2f);
    }
  }
  updateY(delta_time);
  updateCircle();
}

void Character::updateCircle() {
  float newRadius = (kPlayerWidth + kGrowthRate * position_.x);
  if (circle_->radius < newRadius) {
    circle_->radius = newRadius;
  }
  circle_->center = position_;
  circle_->center.y += circle_->radius;
}

void Character::moveLeft(float delta_time) {
  position_.x -= kSpeed * delta_time;
  if (position_.x < 0.0f) {
    position_.x = 0.0f;
  }
}

void Character::moveRight(float delta_time) {
  position_.x += kSpeed * delta_time;
  float level_width = ground_->width();
  if (position_.x + kPlayerWidth > level_width) {
    position_.x = level_width - kPlayerWidth;
  }
}

void Character::jump() {
  if (is_jumping_) return;
  is_jumping_ = true;
  jump_velocity_ = kInitialJumpVelocity;
  time_on_ground_ = 0.0f;
}

void Character::updateY(float delta_time) {
  float minimum_height = ground_->heightAt(position_.x) + kHeightAboveGround;
  if (is_jumping_) {
    position_.y += jump_velocity_ * delta_time;
    jump_velocity_ += kGravity * delta_time;
    if (position_.y < minimum_height) {
      position_.y = minimum_height;
      is_jumping_ = false;
    }
  } else {
    position_.y = minimum_height;
    time_on_ground_ += delta_time;
  }
}

void Character::getTargets(vector<Target> *targets) {
  Target target;
  target.position = circle_->center;
  target.entity = this;
  target.id = targets->size();
  targets->push_back(target);
}

void Character::colorTarget(Target target) {
  circle_->color = glm::vec4(0.65f, 0.47f, 0.0f, 1.0f);
}

glm::vec2 Character::groundPosition() { 
  return glm::vec2(position_.x, ground_->heightAt(position_.x) + kHeightAboveGround);
}
