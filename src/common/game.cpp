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
  bool vsync = getSetting("vsync").getBoolean();
  if (vsync) {
    glfwSwapInterval(1);
  } else {
    glfwSwapInterval(0);
  }

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

  theEngine().init(width, height);
  theWorld().init();
  
  last_frame_time_ = static_cast<float>(glfwGetTime());
}

void Game::update() {
  float now = static_cast<float>(glfwGetTime());
  float delta_time = now - last_frame_time_;
  last_frame_time_ = now;
  theEngine().update(delta_time);
}

void Game::draw() {
  theEngine().draw();
}

void Game::handleKeyboardEvent(int key, int action) {}

bool Game::stillRunning() {
  return !leave_game_;
}

