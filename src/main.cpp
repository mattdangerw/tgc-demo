#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glfw.h>

#include "game.h"
#include "util/settings.h"

static Game *game = NULL;

void cleanupAndExit(int exit_code) {
  glfwTerminate();
  if (game != NULL) delete game;
  if (exit_code != 0) std::getchar();
  exit(exit_code);
}

// Handle keyboard events.
void GLFWCALL keyboardCallback(int key, int action) {
  game->handleKeyboardEvent(key, action);
}

int GLFWCALL windowCloseCallback() {
  game->prepareToQuit();
  return GL_TRUE;
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
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // This is really for multisampling not FSAA but whatevs, we still need it.
  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 8);
  bool fullscreen = getSetting("fullscreen").getBoolean();
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
  if (!glfwOpenWindow(width, height, 0, 0, 0, 0, 24, 8, screen_mode)) {
    fprintf(stderr, "Failed to open GLFW window.\n");
    cleanupAndExit(1);
  }
  int major, minor, rev;
  glfwGetGLVersion(&major, &minor, &rev);
  fprintf(stderr, "OpenGL version: %d.%d.%d\n", major, minor, rev);
  
  // Init glew. We need experimental for a core profile till glew fixes a bug...
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  // Glew init spawns an error sometimes. This clears the GL error state for our own use.
  glGetError();
  if (GLEW_OK != err) {
    fprintf(stderr, "GLEW error: %s\n", glewGetErrorString(err));
    cleanupAndExit(1);
  }
  if (!GLEW_VERSION_3_3) {
    fprintf(stderr, "OpenGL 3.3 is not supported.\n");
    cleanupAndExit(1);
  }

  // GLFW options.
  glfwEnable(GLFW_KEY_REPEAT);
  glfwSwapInterval(1);
  // Set callback functions.
  glfwSetKeyCallback(keyboardCallback);
  glfwSetWindowCloseCallback(windowCloseCallback);

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
    //glFinish();
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
    glfwSwapBuffers();
  }
  cleanupAndExit(0);
  return 0;
}
