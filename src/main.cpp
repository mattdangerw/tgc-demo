#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>

#include "game.h"

static Game *game;
static bool fullscreen = false;

// Handle keyboard events.
void GLFWCALL keyboardCallback(int key, int action) {
  game->handleKeyboardEvent(key, action);
}

// Adjust window size.
void GLFWCALL resizeCallback(int width, int height) {
  game->resize(width, height);
}

// Handle input and inititialize OpenGL.
static void init(int argc, char *argv[], int width, int height) {
  // Parse args someday?
  game->init(width, height);
}

int main(int argc, char *argv[]) {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    exit(1);
  }
  int screen_mode = fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW;
  int width, height;
  if (fullscreen) {
    // Gets native resolution of monitor.
    GLFWvidmode mode;
    glfwGetDesktopMode(&mode);
    width = mode.Width;
    height = mode.Height;
  } else {
    width = 800;
    height = 600;
  }
  if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 16, 0, screen_mode)) {
    fprintf(stderr, "Failed to open GLFW window\n");
    glfwTerminate();
    exit(1);
  }
  game = new Game();

  glfwEnable(GLFW_KEY_REPEAT);
  glfwSwapInterval(1);
  // Set callback functions.
  glfwSetWindowSizeCallback(resizeCallback);
  glfwSetKeyCallback(keyboardCallback);

  // Parse command-line options. And init game;
  init(argc, argv, width, height);
  // Main loop.
  while (game->stillRunning()) {
    game->update();
    game->draw();
    glfwSwapBuffers();
  }
  // Terminate GLFW
  glfwTerminate();
  return 0;
}
