#include "ground.h"
#include "GL\glfw.h"

Ground::Ground() {
  renderer = NULL;
}

Ground::~Ground() {}

void Ground::init(Renderer *renderer) {
  this->renderer = renderer;
  points.push_back(glm::vec2(0.0f, 0.2f));
  points.push_back(glm::vec2(0.3f, 0.1f));
  points.push_back(glm::vec2(0.6f, 0.4f));
  points.push_back(glm::vec2(0.9f, 0.1f));
  renderer->addDrawable(this);
}

void Ground::draw() {
  glLineWidth(10);
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_LINE_STRIP);
    vector<glm::vec2>::iterator it;
    for (it = points.begin(); it != points.end(); ++it) {
      glVertex2fv(glm::value_ptr(*it));
    }
  glEnd();
  glFlush();
}

string Ground::shadingGrounp() {
  return "default";
}
