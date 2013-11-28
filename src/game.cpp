#include "game.h"

#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
  
#ifdef _DEBUG
  // Check for any bad GL calls. I think this needs GL to flush all it's
  // commands, so only do it when debugging.
  checkForGLError();
#endif
  last_frame_time_ = static_cast<float>(glfwGetTime());
}

void Game::update() {
  float now = static_cast<float>(glfwGetTime());
  float delta_time = now - last_frame_time_;
  last_frame_time_ = now;

  theEngine().update(delta_time);

  theWorld().character.setInput(left_down_, right_down_, space_pressed_);
  space_pressed_ = false;

#ifdef _DEBUG
  // Check for any bad GL calls.
  checkForGLError();
#endif
}

void Game::draw() {
  theEngine().draw();
#ifdef _DEBUG
  // Check for any bad GL calls.
  checkForGLError();
#endif
}

bool Game::stillRunning() {
  return !leave_game_;
}

void Game::handleKeyboardEvent(int key, int action) {
  switch (key) {
    case GLFW_KEY_ESCAPE:
      if (action == GLFW_PRESS) leave_game_ = true;
      break;
    case GLFW_KEY_LEFT:
      left_down_ = action != GLFW_RELEASE;
      if (left_down_ && right_down_) right_down_ = false;
      break;
    case GLFW_KEY_RIGHT:
      right_down_ = action != GLFW_RELEASE;
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
