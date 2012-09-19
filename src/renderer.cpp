#include "Renderer.h"
#include "GL/glfw.h"

Renderer::Renderer() {
  left_of_window_ = 0.0f;
}

Renderer::~Renderer() {}

void Renderer::init(int width, int height) {
  aspect_ = static_cast<float>(width)/height;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, aspect_, 0.0f, 1.0f, 0.0f, 1.0f);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Renderer::draw() {
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslatef(-left_of_window_, 0.0f, 0.0f);
  vector<Drawable *>::iterator it;
  for (it = to_draw_.begin(); it != to_draw_.end(); ++it) {
    (*it)->draw();
  }
}

void Renderer::addDrawable(Drawable *object) {
  to_draw_.push_back(object);
}

void Renderer::removeDrawable(Drawable *object) {
  // TODO: remove object from to_draw.
}

void Renderer::resize(int width, int height) {
  glViewport(0, 0, width, height);
}
