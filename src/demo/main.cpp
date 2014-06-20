#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "util/settings.h"
#include "util/error.h"

static Game *game = NULL;

void cleanupAndExit(int exit_code) {
  glfwTerminate();
  if (game != NULL) delete game;
  if (exit_code != 0) std::getchar();
  exit(exit_code);
}

// Handle keyboard events.
static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  game->handleKeyboardEvent(key, action);
}

static void windowCloseCallback(GLFWwindow* window) {
  game->prepareToQuit();
}

int main(int argc, char *argv[]) {
  // Make the main game object.
  game = new Game();
  game->init(width, height);  
  while (game->stillRunning()) {
    game->draw();
  }
  return 0;
}
