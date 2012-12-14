#include "game.h"

#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

#include "render/renderer.h"
#include "game_state/state.h"
#include "util/error.h"

// Where we want the character to be position horizantally on the screen 0 to 1.
static const float kCharacterScreenX = 0.5f;

Game::Game()
    : leave_game_(false),
      left_down_(false),
      right_down_(false),
      space_pressed_(false) {}

Game::~Game() {}

void Game::init(int width, int height) {
  srand((unsigned)time(0));

  theRenderer().init(width, height);
  theState().init();
  
  // Check for any bad GL calls. For debugging.
  checkForGLError();
  last_frame_time_ = static_cast<float>(glfwGetTime());
}

void Game::update() {
  float now = static_cast<float>(glfwGetTime());
  float delta_time = now - last_frame_time_;

  theState().updater.updateAll(delta_time);
  theState().character.setInput(left_down_, right_down_, space_pressed_);

  space_pressed_ = false;
  last_frame_time_ = now;

  // Check for any bad GL calls. For debugging.
  checkForGLError();
}

void Game::draw() {
  theRenderer().draw();
  // Check for any bad GL calls. For debugging.
  checkForGLError();
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
