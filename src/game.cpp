#include "game.h"
#include "GL/glfw.h"

Game::Game() {
  leave_game = false;
}

Game::~Game() {}

void Game::init(int width, int height) {
  renderer.init(width, height);
  ground.init(&renderer);
}

void Game::update() {
}

void Game::draw() {
  renderer.draw();
}

bool Game::stillRunning() {
  return !leave_game;
}

void Game::handleKeyboardEvent(int key, int action) {
  if (action != GLFW_PRESS && action != GLFW_RELEASE) return;
  switch (key) {
    case GLFW_KEY_ESC:
      leave_game = true;
      break;
    case GLFW_KEY_LEFT:
      left_down = action == GLFW_PRESS;
      break;
    case GLFW_KEY_RIGHT:
      right_down = action == GLFW_PRESS;
      break;
    case GLFW_KEY_SPACE:
      // We only care about space bar the first frame it is pressed.
      if (action == GLFW_PRESS) space_pressed = true;
      break;
    default:
      return;
  }
}

void Game::resize(int width, int height) {
  renderer.resize(width, height);
}
