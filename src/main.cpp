#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glfw.h>

#include "game.h"

static Game *game = NULL;
static bool fullscreen = true;

void cleanupAndExit(int exit_code) {
  glfwTerminate();
  if(game!=NULL) delete game;
  exit(exit_code);
}

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
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 8);
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
    fprintf(stderr, "Failed to open GLFW window.\n");
    cleanupAndExit(1);
  }
  // Init glew.
  GLenum err = glewInit();
  if (GLEW_OK != err)  {
    fprintf(stderr, "GLEW error: %s\n", glewGetErrorString(err));
    cleanupAndExit(1);
  }
  if (!GLEW_VERSION_3_3) {
    fprintf(stderr, "OpenGL 3.3 is not supported.\n");
    cleanupAndExit(1);
  }
  // Make the main game object.
  game = new Game();

  // GLFW options.
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
  cleanupAndExit(0);
  return 0;
}
