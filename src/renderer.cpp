#include "renderer.h"

#include <GL/glew.h>
#include <gli.hpp>
#include <gli/gtx/gl_texture2d.hpp>
#include <algorithm>

#include "transform2D.h"

Renderer::Renderer() {
  left_of_window_ = 0.0f;
}

void Renderer::init(int width, int height) {
  aspect_ = static_cast<float>(width)/height;

  // OpenGL settings.
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glEnable(GL_MULTISAMPLE);
  //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  
  // Load shaders.
  Shader general_vert, textured_frag, colored_frag, minimal_frag, quadric_frag, circles_frag,
    particles_vert, particles_frag;
  general_vert.load("src/shaders/general.vert", GL_VERTEX_SHADER);
  textured_frag.load("src/shaders/textured.frag", GL_FRAGMENT_SHADER);
  Program &textured = programs_["textured"];
  textured.addShader(&general_vert);
  textured.addShader(&textured_frag);
  textured.link();
  colored_frag.load("src/shaders/colored.frag", GL_FRAGMENT_SHADER);
  Program &colored = programs_["colored"];
  colored.addShader(&general_vert);
  colored.addShader(&colored_frag);
  colored.link();
  minimal_frag.load("src/shaders/minimal.frag", GL_FRAGMENT_SHADER);
  Program &minimal = programs_["minimal"];
  minimal.addShader(&general_vert);
  minimal.addShader(&minimal_frag);
  minimal.link();
  quadric_frag.load("src/shaders/quadric_anti_aliased.frag", GL_FRAGMENT_SHADER);
  Program &quadric = programs_["quadric"];
  quadric.addShader(&general_vert);
  quadric.addShader(&quadric_frag);
  quadric.link();
  circles_frag.load("src/shaders/circles_anti_aliased.frag", GL_FRAGMENT_SHADER);
  Program &circles = programs_["circles"];
  circles.addShader(&general_vert);
  circles.addShader(&circles_frag);
  circles.link();
  particles_vert.load("src/shaders/particles.vert", GL_VERTEX_SHADER);
  particles_frag.load("src/shaders/particles.frag", GL_FRAGMENT_SHADER);
  Program &particles = programs_["particles"];
  particles.addShader(&particles_vert);
  particles.addShader(&particles_frag);
  particles.link();
}

bool compareDrawables(const Drawable *left, const Drawable *right) {
  return left->displayPriority() < right->displayPriority();
}

void Renderer::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // 2D rendering modelview
  glm::mat3 view(1.0f);
  view = translate2D(view, glm::vec2(-1.0f, -1.0f));
  view = scale2D(view, glm::vec2(2.0f/aspect_, 2.0f));
  view = translate2D(view, glm::vec2(-left_of_window_, 0.0f));

  std::sort(to_draw_.begin(), to_draw_.end(), compareDrawables);
  vector<Drawable *>::iterator it;
  for (it = to_draw_.begin(); it != to_draw_.end(); ++it) {
    (*it)->draw(view);
  }
}

void Renderer::addDrawable(Drawable *object) {
  to_draw_.push_back(object);
}

void Renderer::removeDrawable(Drawable *object) {
  vector<Drawable *>::iterator it;
  for (it = to_draw_.begin(); it != to_draw_.end(); ++it) {
    if (*it == object) {
      to_draw_.erase(it);
      return;
    }
  }
}

void Renderer::resize(int width, int height) {
  glViewport(0, 0, width, height);
}

Program *Renderer::getProgram(string name) {
  if (programs_.count(name) == 0) return NULL;
  return &programs_[name];
}

GLuint Renderer::getTexture(string filename) {
  if (textures_.count(filename) == 0) {
    textures_[filename] = gli::createTexture2D(filename);
  }
  return textures_[filename];
}
