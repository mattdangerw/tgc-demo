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
      leave_game_(false),
      left_down_(false),
      right_down_(false),
      space_pressed_(false) {}

Game::~Game() {}

void Game::init(int width, int height) {
  srand((unsigned)time(0));

  Renderer::instance().init(width, height);
  ground_.init();
  cloud_manager_.init(&ground_);
  character_.init(&ground_);
  thought_bubble_.init(&character_);
  particle_system_.init(&thought_bubble_);
  triggerable_manager_.init(&character_, &particle_system_);
  
  last_frame_time_ = static_cast<float>(glfwGetTime());
}

void Game::update() {
  float now = static_cast<float>(glfwGetTime());
  float delta_time = now - last_frame_time_;

  character_.setInput(left_down_, right_down_, space_pressed_);

  character_.update(delta_time, &state_);
  thought_bubble_.update(delta_time, &state_);
  particle_system_.update(delta_time, &state_);
  cloud_manager_.update(delta_time, &state_);
  triggerable_manager_.update(delta_time, &state_);
  
  // Position the camera so our character is at kCharacterScreenX.
  // But make sure not to scroll off level.
  Renderer &renderer = Renderer::instance();
  float window_width = renderer.windowWidth();
  float left_of_screen = glm::clamp(character_.position().x - kCharacterScreenX * window_width, 0.0f, ground_.width() - window_width);
  renderer.setLeftOfWindow(left_of_screen);
  space_pressed_ = false;
  last_frame_time_ = now;
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
