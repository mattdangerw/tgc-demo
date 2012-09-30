#include "game.h"

#include <random>
#include <time.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm.hpp>

#include "renderer.h"

// Where we want the character to be position horizantally on the screen 0 to 1.
static const float kCharacterScreenX = 0.5f;

Game::Game()
    : state_(WALKING),
      last_state_(WALKING),
      leave_game_(false),
      left_down_(false),
      right_down_(false),
      space_pressed_(false) {}

Game::~Game() {}

void Game::init(int width, int height) {
  srand((unsigned)time(0));

  Renderer::instance().init(width, height);
  ground_.init();
  entities_.push_back(&ground_);
  cloud_manager_.init(&ground_);
  entities_.push_back(&cloud_manager_);
  character_.init(&ground_);
  entities_.push_back(&character_);
  thought_bubble_.init(&character_);
  entities_.push_back(&thought_bubble_);
  particle_system_.init(&thought_bubble_);
  entities_.push_back(&particle_system_);
  triggerable_manager_.init(&character_, &particle_system_);
  entities_.push_back(&triggerable_manager_);
  vector<float> parents;
  parents.push_back(0.5f);
  parents.push_back(0.65f);
  crowds_[0].init(&character_, &ground_, parents, 0.07f, 0.5f, 2.0f, 0.6f);
  entities_.push_back(crowds_);
  vector<float> kids;
  kids.push_back(1.0f);
  kids.push_back(1.05f);
  kids.push_back(1.08f);
  kids.push_back(1.1f);
  kids.push_back(1.12f);
  crowds_[1].init(&character_, &ground_, kids, 0.02f, 0.0f, 0.4f, 1.2f);
  entities_.push_back(crowds_ + 1);
  
  last_frame_time_ = static_cast<float>(glfwGetTime());
}

void Game::update() {
  float now = static_cast<float>(glfwGetTime());
  float delta_time = now - last_frame_time_;

  character_.setInput(left_down_, right_down_, space_pressed_);

  for (vector<GameEntity *>::iterator it = entities_.begin(); it != entities_.end(); it++) {
    (*it)->update(delta_time, &state_);
  }

  //if (state_ == EXPLODING && last_state_ != EXPLODING) {
  //  int test = 0;
  //}
  
  // Position the camera so our character is at kCharacterScreenX.
  // But make sure not to scroll off level.
  Renderer &renderer = Renderer::instance();
  float window_width = renderer.windowWidth();
  float left_of_screen = glm::clamp(character_.position().x - kCharacterScreenX * window_width, 0.0f, ground_.width() - window_width);
  renderer.setLeftOfWindow(left_of_screen);
  space_pressed_ = false;
  last_frame_time_ = now;
  last_state_ = state_;
}

void Game::draw() {
  Renderer::instance().draw();
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
  Renderer::instance().resize(width, height);
}
