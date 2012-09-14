#include <stdlib.h>
#include <stdio.h>
#include <GL/glfw.h>

static bool running = true;

static void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_POLYGON);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.5f, -0.5f);
  glEnd();
  glFlush();
}

// Handle keyboard events.
void GLFWCALL keyboardCallback(int k, int action) {
  if (action != GLFW_PRESS) return;
  switch (k) {
    case GLFW_KEY_ESC:
      running = false;
      break;
    default:
      return;
  }
}

// Adjust window size.
void GLFWCALL reshapeCallback(int width, int height) {}

// Handle input and inititialize OpenGL.
static void init(int argc, char *argv[]) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char *argv[]) {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    exit(1);
  }
  // Open a fullscreen window.
  GLFWvidmode mode;
  glfwGetDesktopMode(&mode);
  if (!glfwOpenWindow(mode.Width, mode.Height, 0, 0, 0, 0, 16, 0, GLFW_FULLSCREEN)) {
    fprintf(stderr, "Failed to open GLFW window\n");
    glfwTerminate();
    exit(1);
  }
  glfwEnable(GLFW_KEY_REPEAT);
  glfwSwapInterval(1);
  // Parse command-line options.
  init(argc, argv);
  // Set callback functions.
  glfwSetWindowSizeCallback(reshapeCallback);
  glfwSetKeyCallback(keyboardCallback);
  // Main loop.
  while (running) {
    draw();
    glfwSwapBuffers();
  }
  // Terminate GLFW
  glfwTerminate();
  return 0;
}
