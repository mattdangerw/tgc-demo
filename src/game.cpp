#include "game.h"

#include <random>
#include <time.h>
#include <algorithm>

#include "GL/glfw.h"
#include "glm.hpp"

// Where we want the character to be position horizantally on the screen 0 to 1.
static const float kCharacterScreenX = 0.2f;
static const int kNumClouds = 6;
static const float kCloudMinSize = 0.1f;
static const float kCloudMaxSize = 0.4f;
static const float kCloudMinY = 0.6f;
static const float kCloudMaxY = 1.2f;
static const float kCloudMinVelocity = 0.03f;
static const float kCloudMaxVelocity = 0.05f;
static const float kCloudOffscreen = 0.4f;

static inline float randomFloat(float min, float max) {
  return min + rand()/(RAND_MAX/(max - min));
}

Game::Game()
    : state_(WALKING),
      leave_game_(false),
      left_down_(false),
      right_down_(false),
      space_pressed_(false),
      left_of_screen_(0.0f),
      current_triggerable_(0) {}

Game::~Game() {
  for (vector<Triggerable *>::iterator it = triggerables_.begin(); it != triggerables_.end(); ++it) {
    delete (*it);
  }
  for (vector<Cloud *>::iterator it = clouds_.begin(); it != clouds_.end(); ++it) {
    delete (*it);
  }
}

void Game::init(int width, int height) {
  srand((unsigned)time(0));

  renderer_.init(width, height);
  ground_.init(&renderer_);
  initClouds();
  initTriggerables();
  character_.init(&renderer_, &ground_);
  thought_bubble_.init(&renderer_, &character_);
  particle_system_.init(&renderer_, &thought_bubble_);
  last_frame_time_ = static_cast<float>(glfwGetTime());
}

static bool sortTriggerables(Triggerable *a, Triggerable *b) {
  return a->triggerPoint() < b->triggerPoint();
}

void Game::initTriggerables() {
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(&renderer_, 0.5f, 1.0f);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(&renderer_, 1.0f, 0.5f);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(&renderer_, 1.5f, 0.5f);
  triggerables_.push_back(new Triggerable());
  triggerables_.back()->init(&renderer_, 2.2f, 3.0f);
  std::sort(triggerables_.begin(), triggerables_.end(), sortTriggerables);
}

void Game::initClouds() {
  for (int i = 0; i < kNumClouds; ++i) {
    Cloud *cloud = new Cloud();
    cloud->init(&renderer_);
    placeCloudRandomly(cloud);
    clouds_.push_back(cloud);
  }
}

void Game::placeCloudRandomly(Cloud *cloud) {
  cloud->setVelocity(randomFloat(kCloudMinVelocity, kCloudMaxVelocity));
  float size = randomFloat(kCloudMinSize, kCloudMaxSize);
  cloud->setSize(size);
  float y_position = randomFloat(kCloudMinY, kCloudMaxY - size);
  float min_x = left_of_screen_ - kCloudOffscreen;
  float max_x = left_of_screen_  + renderer_.windowWidth() + kCloudOffscreen - size;
  cloud->setPosition(glm::vec2(randomFloat(min_x, max_x), y_position));
}

void Game::placeCloudRandomlyOnLeft(Cloud *cloud) {
  cloud->setVelocity(randomFloat(kCloudMinVelocity, kCloudMaxVelocity));
  float size = randomFloat(kCloudMinSize, kCloudMaxSize);
  cloud->setSize(size);
  float y_position = randomFloat(kCloudMinY, kCloudMaxY - size);
  cloud->setPosition(glm::vec2(left_of_screen_ - kCloudOffscreen, y_position));
}

void Game::placeCloudRandomlyOnRight(Cloud *cloud) {
  cloud->setVelocity(randomFloat(kCloudMinVelocity, kCloudMaxVelocity));
  float size = randomFloat(kCloudMinSize, kCloudMaxSize);
  cloud->setSize(size);
  float y_position = randomFloat(kCloudMinY, kCloudMaxY - size);
  cloud->setPosition(glm::vec2(left_of_screen_  + renderer_.windowWidth() + kCloudOffscreen - size, y_position));
}

void Game::update() {
  float now = static_cast<float>(glfwGetTime());
  float delta_time = now - last_frame_time_;
  if (state_ == TRIGGERING) {
    if (triggerables_[current_triggerable_]->doneAnimating()) {
      state_ = WALKING;
      current_triggerable_++;
    }
  }
  if (state_ == WALKING) {
    character_.update(delta_time, left_down_, right_down_, space_pressed_);
  } else {
    character_.update(delta_time);
  }
  if (state_ == EXPLODING) {
    thought_bubble_.moveTowardsCenter(delta_time);
    particle_system_.update(delta_time);
  } else {
    thought_bubble_.update(delta_time);
    particle_system_.update(delta_time);
    updateTriggerables(delta_time);
    updateClouds(delta_time);
  }
  // Position the camera so our character is at kCharacterScreenX.
  // But make sure not to scroll off level.
  left_of_screen_ = glm::clamp(character_.position().x - kCharacterScreenX, 0.0f, ground_.width() - renderer_.windowWidth());
  renderer_.setCameraScroll(left_of_screen_);
  space_pressed_ = false;
  last_frame_time_ = now;
}

void Game::updateTriggerables(float delta_time) {
  if (state_ == WALKING && current_triggerable_ < triggerables_.size()) {
    Triggerable *to_check = triggerables_[current_triggerable_];
    if (character_.position().x > to_check->triggerPoint()) {
      to_check->trigger();
      if (current_triggerable_ == triggerables_.size() - 1) {
        state_ = EXPLODING;
      } else {
        state_ = TRIGGERING;
        particle_system_.addParticles(5);
      }
    }
  }
  vector<Triggerable *>::iterator it;
  for (it = triggerables_.begin(); it != triggerables_.end(); ++it) {
    (*it)->update(delta_time);
  }
}

void Game::updateClouds(float delta_time) {
  vector<Cloud *>::iterator it;
  for (it = clouds_.begin(); it != clouds_.end(); ++it) {
    Cloud *cloud = *it;
    float x_begin, x_end;
    cloud->xExtent(&x_begin, &x_end);
    if (x_end < left_of_screen_ - kCloudOffscreen) {
      placeCloudRandomlyOnRight(cloud);
    } else if (x_begin > left_of_screen_ + renderer_.windowWidth() + kCloudOffscreen - cloud->size()) {
      placeCloudRandomlyOnLeft(cloud);
    }
    cloud->update(delta_time);
  }
}

void Game::draw() {
  renderer_.draw();
}

bool Game::stillRunning() {
  return !leave_game_;
}

void Game::handleKeyboardEvent(int key, int action) {
  if (action != GLFW_PRESS && action != GLFW_RELEASE) return;
  switch (key) {
    case GLFW_KEY_ESC:
      leave_game_ = true;
      break;
    case GLFW_KEY_LEFT:
      left_down_ = action == GLFW_PRESS;
      if (left_down_ && right_down_) right_down_ = false;
      break;
    case GLFW_KEY_RIGHT:
      right_down_ = action == GLFW_PRESS;
      if (left_down_ && right_down_) left_down_ = false;
      break;
    case GLFW_KEY_SPACE:
      // We only care about space bar the first frame it is pressed.
      if (action == GLFW_PRESS) space_pressed_ = true;
      break;
    default:
      return;
  }
}

void Game::resize(int width, int height) {
  renderer_.resize(width, height);
}
