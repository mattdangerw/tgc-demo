#include "game_state/jumping_crowd.h"

#include "util/random.h"

static const float kActivateDistance = 0.1f;
static const float kGravity = -8.0f;

JumpingCrowd::JumpingCrowd() {}

JumpingCrowd::~JumpingCrowd() {}

void JumpingCrowd::init(Character *character, Ground *ground, const vector<float> &x_positions, float size, float ground_time_min, float ground_time_max, float jump_velocity) {
  ground_ = ground;
  character_ = character;
  time_min_ = ground_time_min;
  time_max_ = ground_time_max;
  initial_jump_velocity_ = jump_velocity;
  start_ = end_ = x_positions[0];
  for (size_t i = 0; i < x_positions.size(); ++i) {
    Circle circle;
    float gray = randomFloat(0.12f, 0.38f);
    circle.color = glm::vec4(gray, gray, gray, 1.0f);
    circle.center.x = x_positions[i];
    circle.center.y = ground_->heightAt(circle.center.x) + size;
    circle.radius = size;
    if (circle.center.x < start_) start_ = circle.center.x;
    if (circle.center.x > end_) end_ = circle.center.x;
    crowd_.push_back(circle);
    time_on_ground_.push_back(0.0f);
    time_till_next_jump_.push_back(jumpTime());
    jumping_.push_back(false);
    velocity_.push_back(0.0f);
    ground_height_.push_back(circle.center.y);
  }
  // Ready drawable.
  drawer_.init(&crowd_);
  drawer_.setDisplayPriority(2);
}

void JumpingCrowd::update(float delta_time, GameState *state) {
  if (*state == EXPLODING || *state == PRE_EXPLODING) return;
  float character_x = character_->position().x;
  for (size_t i = 0; i < crowd_.size(); ++i) {
    if (jumping_[i]) {
      Circle *circle = &crowd_[i];
      circle->center.y += velocity_[i] * delta_time;
      velocity_[i] += kGravity * delta_time;
      if (circle->center.y < ground_height_[i]) {
        circle->center.y = ground_height_[i];
        jumping_[i] = false;
        time_till_next_jump_[i] = jumpTime();
        time_on_ground_[i] = 0.0f;
      }
    }
  }
  if(character_x > start_ - kActivateDistance && character_x < end_ + kActivateDistance) {
    for (size_t i = 0; i < crowd_.size(); ++i) {
      if (!jumping_[i]) {
        time_on_ground_[i]+=delta_time;
        if (time_on_ground_[i] > time_till_next_jump_[i]) {
          jumping_[i] = true;
          velocity_[i] = initial_jump_velocity_;
        }
      }
    }
  }
}

float JumpingCrowd::jumpTime() {
  return randomFloat(time_min_, time_max_);
}