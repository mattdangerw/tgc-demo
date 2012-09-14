#include "Renderer.h"
#include "GL/glfw.h"

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::init(int width, int height) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::draw() {
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  vector<Drawable *>::iterator it;
  for (it = to_draw.begin(); it != to_draw.end(); ++it) {
    (*it)->draw();
  }
}

void Renderer::addDrawable(Drawable *object) {
  to_draw.push_back(object);
}

void Renderer::removeDrawable(Drawable *object) {
  // TODO: remove object from to_draw.
}

void Renderer::resize(int width, int height) {
  glViewport(0, 0, width, height);
}
