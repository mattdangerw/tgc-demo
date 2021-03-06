#include "world/character.h"

#include <GL/glew.h>

#include "world/world.h"
#include "util/settings.h"
#include "util/transform2D.h"
#include "util/random.h"

static const float kHeightAboveGround = 0.0f;
static const float kGravity = -8.0f;
static const float kInitialJumpVelocity = 1.2f;

Character::Character()
  : position_(),
    time_on_ground_(0.0f),
    time_till_next_jump_(0) {}

Character::~Character() {}

void Character::init() {
  fill_.init(glm::vec4(glm::vec3(0.0f), 1.0f));
  circle_.init();
  circle_.setRadius(0.0f);
  circle_.setParent(this);
  circle_.setDisplayPriority(5.0f);
  circle_.setIsVisible(true);
  circle_.setFill(&fill_);

  position_.x = getSetting("player_width").getFloat();
  updateY(0.0f);
  updateCircle();
}

void Character::setInput(bool left_down, bool right_down, bool space_pressed) {
  left_down_ = left_down;
  right_down_ = right_down;
  space_pressed_ = space_pressed;
}

void Character::update(float delta_time) {
  // Update x position.
  if (left_down_) moveLeft(delta_time);
  if (right_down_) moveRight(delta_time);
  // Udpate y postion.
  if (space_pressed_) this->jump();
  updateY(delta_time);
  updateCircle();
}

void Character::updateCircle() {
  float newRadius = (getSetting("player_width").getFloat() + getSetting("player_growth_rate").getFloat() * position_.x);
  if (circle_.radius() < newRadius) {
    circle_.setRadius(newRadius);
  }
  glm::vec2 center = position_;
  center.y += circle_.radius();
  circle_.setCenter(center);
}

void Character::moveLeft(float delta_time) {
  position_.x -= getSetting("player_speed").getFloat() * delta_time;
  if (position_.x < 0.0f) {
    position_.x = 0.0f;
  }
}

void Character::moveRight(float delta_time) {
  position_.x += getSetting("player_speed").getFloat() * delta_time;
  float level_width = theWorld().ground.width();
  if (position_.x + circle_.radius() > level_width) {
    position_.x = level_width - circle_.radius();
  }
}

void Character::jump() {
  if (is_jumping_) return;
  is_jumping_ = true;
  jump_velocity_ = kInitialJumpVelocity;
  time_on_ground_ = 0.0f;
}

void Character::updateY(float delta_time) {
  float minimum_height = theWorld().ground.heightAt(position_.x) + kHeightAboveGround;
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

glm::vec2 Character::groundPosition() { 
  return glm::vec2(position_.x, theWorld().ground.heightAt(position_.x) + kHeightAboveGround);
}
