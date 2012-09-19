#include "character.h"
#include "GL/glfw.h"

static const float kSpeed = 0.4f;
static const float kPlayerWidth = 0.02f;
static const float kHeightAboveGround = 0.01f;
static const float kGravity = -10.0f;
static const float kInitialJumpVelocity = 1.2f;

Character::Character() {
  renderer_ = NULL;
  position_ = glm::vec2();
}

Character::~Character() {}

void Character::init(Renderer *renderer, Ground *ground) {
  renderer_ = renderer;
  ground_ = ground;
  position_.x = 0.0f;
  updateY(0.0f);
  renderer_->addDrawable(this);
}

void Character::update(float delta_time, bool move_left, bool move_right, bool jump) {
  // Update x position.
  if (move_left) moveLeft(delta_time);
  if (move_right) moveRight(delta_time);
  // Udpate y postion.
  if (jump) this->jump();
  update(delta_time);
}

void Character::update(float delta_time) {
  updateY(delta_time);
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
}

void Character::updateY(float delta_time) {
  float minimum_height = ground_->heightAt(position_.x + kPlayerWidth/2) + kHeightAboveGround;
  if (is_jumping_) {
    position_.y += jump_velocity_ * delta_time;
    jump_velocity_ += kGravity * delta_time;
    if (position_.y < minimum_height) {
      position_.y = minimum_height;
      is_jumping_ = false;
    }
  } else {
    position_.y = minimum_height;
  }
}

void Character::draw() {
  glColor3f(0.7f, 0.7f, 0.7f);
  glRectf(position_.x, position_.y, position_.x + kPlayerWidth, position_.y + kPlayerWidth);
}
