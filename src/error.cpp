#include "error.h"

#include <cstdio>
#include <GL/glew.h>
#include <string>

using std::string;

// Forward declare our exit point.
void cleanupAndExit(int exit_code);

void error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  cleanupAndExit(1);
}

void warning(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
}

void checkForGLError() {
  GLenum error_code = glGetError();
  string message;
  switch (error_code) {
    case GL_NO_ERROR:
      return;
    case GL_INVALID_ENUM:
      message = "GL_INVALID_ENUM error.";
      break;
    case GL_INVALID_VALUE:
      message = "GL_INVALID_VALUE error.";
      break;
    case GL_INVALID_OPERATION:
      message = "GL_INVALID_OPERATION error.";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      message = "GL_INVALID_FRAMEBUFFER_OPERATION error.";
      break;
    case GL_OUT_OF_MEMORY:
      message = "GL_OUT_OF_MEMORYN error.";
      break;
    default:
      message = "Unknown GL error.";
  }
  error(message.c_str());
  //warning(message.c_str());
}