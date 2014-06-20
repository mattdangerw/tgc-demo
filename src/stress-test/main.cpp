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
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    exit(1);
  }
  printf("Welcome to the demo. Controls are quite simple--left/right arrows and space to play, escape to quit. Enjoy!\n");
  printf("Press enter to continue...");
  std::getchar();
  loadSettings("content/game_settings");

  // Demand a core profile.
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);
  glfwSwapInterval(1);

  int width, height;
  GLFWwindow* window = NULL;
  bool fullscreen = getSetting("fullscreen").getBoolean();
  if (fullscreen) {
    // Gets current resolution of monitor.
    const GLFWvidmode *video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    width = video_mode->width;
    height = video_mode->height;
    window = glfwCreateWindow(width, height, "Demo", glfwGetPrimaryMonitor(), NULL);
  } else {
    width = 800;
    height = 600;
    window = glfwCreateWindow(width, height, "Demo", NULL, NULL);
  }
  if (window == NULL) error("Failed to open GLFW window.\n");

  // Finish configuring GLEW.
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyboardCallback);
  glfwSetWindowCloseCallback(window, windowCloseCallback);
  
  // Init glew. We need experimental for a core profile till glew fixes a bug...
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err) error("GLEW error: %s\n", glewGetErrorString(err));
  if (!GLEW_VERSION_3_3) error("OpenGL 3.3 is not supported.\n");
  // Glew init spawns an error sometimes. This clears the GL error state for our own use.
  glGetError();
  
  int major, minor, rev;
  major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
  minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
  rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
  printf("OpenGL version: %d.%d.%d\n", major, minor, rev);

  // Make the main game object.
  game = new Game();
  game->init(width, height);  
  // Main loop.
  int frame = 0;
  int print_frequency = 500;
  float last_print_time = static_cast<float>(glfwGetTime());
  float time_updating = 0.0f, time_drawing = 0.0f;
  while (game->stillRunning()) {
    float frame_start_time = static_cast<float>(glfwGetTime());
    ++frame;

    // Update and draw the game.
    game->draw();
    // glFinish will hurt framerate but gives better estimate of the draw time.
    // glFinish();
    float frame_draw_time = static_cast<float>(glfwGetTime());
    time_drawing += frame_draw_time - frame_start_time;
    game->update();
    time_updating += static_cast<float>(glfwGetTime()) - frame_draw_time;
    
    // Print the frame rate every once and a while.
    if (frame % print_frequency == 0) {
      float time_elapsed = frame_start_time - last_print_time;
      printf("FPS: %f\n", print_frequency / (time_elapsed));
      last_print_time = frame_start_time;
      printf("Draw time per frame: %f.\n", time_drawing / print_frequency);
      time_drawing = 0.0f;
      printf("Update time per frame: %f.\n", time_updating / print_frequency);
      time_updating = 0.0f;
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  cleanupAndExit(0);
  return 0;
}
