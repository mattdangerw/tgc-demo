#include "Renderer.h"

#include <GL/glew.h>

Renderer::Renderer() {
  left_of_window_ = 0.0f;
}

Renderer::~Renderer() {}

void Renderer::init(int width, int height) {
  aspect_ = static_cast<float>(width)/height;
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // Load shaders.
  Shader textured_vert, minimal_frag, quadric_frag, textured_frag;
  textured_vert.load("src/shaders/textured.vert", GL_VERTEX_SHADER);
  textured_frag.load("src/shaders/textured.frag", GL_FRAGMENT_SHADER);
  texture_program.addShader(&textured_vert);
  texture_program.addShader(&textured_frag);
  texture_program.link();
  minimal_frag.load("src/shaders/minimal.frag", GL_FRAGMENT_SHADER);
  minimal_program.addShader(&textured_vert);
  minimal_program.addShader(&minimal_frag);
  minimal_program.link();
  quadric_frag.load("src/shaders/quadric_anti_aliased.frag", GL_FRAGMENT_SHADER);
  quadric_program.addShader(&textured_vert);
  quadric_program.addShader(&quadric_frag);
  quadric_program.link();
}

void Renderer::draw() {
  glClear(GL_COLOR_BUFFER_BIT);
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
