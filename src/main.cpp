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

int GLFWCALL windowCloseCallback() {
  game->prepareToQuit();
  return GL_TRUE;
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
  glfwSetWindowCloseCallback(windowCloseCallback);

  // Parse command-line options. And init game.
  init(argc, argv, width, height);
  // Main loop.
  int frame = 0;
  float lastTime = static_cast<float>(glfwGetTime());
  float updateTime = 0.0f;
  while (game->stillRunning()) {
    // Pring the frame rate every once and a while.
    float currTime = static_cast<float>(glfwGetTime());
    frame++;
    if (frame % 500 == 0) {
      printf("FPS: %f\n", 500.0f / (currTime - lastTime));
      lastTime = currTime;
    }
    // Update and draw the game.
    game->update();

    updateTime += static_cast<float>(glfwGetTime()) - currTime;
    if (frame % 500 == 0) {
      printf("Update time per frame: %f. Keep less than 0.01\n", updateTime / 500.0f);
      updateTime = 0.0f;
    }

    game->draw();
    glfwSwapBuffers();
  }
  // Terminate GLFW
  glfwTerminate();
  delete game;
  return 0;
}
