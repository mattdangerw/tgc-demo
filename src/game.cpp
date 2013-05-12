#include "game.h"

#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

#include "engine/engine.h"
#include "world/world.h"
#include "util/error.h"

Game::Game()
    : leave_game_(false),
      left_down_(false),
      right_down_(false),
      space_pressed_(false) {}

Game::~Game() {}

void Game::init(int width, int height) {
  srand((unsigned)time(0));

  theEngine().init(width, height);
  theWorld().init();
  
  // Check for any bad GL calls. For debugging.
  checkForGLError();
  last_frame_time_ = static_cast<float>(glfwGetTime());
}

void Game::update() {
  float now = static_cast<float>(glfwGetTime());
  float delta_time = now - last_frame_time_;
  last_frame_time_ = now;

  theEngine().update(delta_time);

  theWorld().character.setInput(left_down_, right_down_, space_pressed_);
  space_pressed_ = false;

  // Check for any bad GL calls. For debugging.
  checkForGLError();
}

void Game::draw() {
  theEngine().draw();
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
