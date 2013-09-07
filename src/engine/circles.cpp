#include "engine/circles.h"

#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

#include "util/transform2D.h"

Circle::Circle()
  : radius_(1.0f),
    center_(0.0f) {}

Circle::~Circle() {}

void Circle::extent(glm::vec2 *min, glm::vec2 *max) {
  *min = center_ - radius_;
  *max = center_ + radius_;
}

void Circle::draw() {
  drawHelper(false);
}

void Circle::drawOccluder() {
  drawHelper(true);
}

void Circle::drawHelper(bool occluder) {
  // Draw a circle to stencil  buffer
  glEnable(GL_STENCIL_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 0, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
  theEngine().useProgram("circles");
  glEnable(GL_DEPTH_TEST);

  glm::mat3 circle_transform(1.0f);
  circle_transform = translate2D(circle_transform, center_ - radius_);
  circle_transform = scale2D(circle_transform, glm::vec2(2 * radius_));
  glUniformMatrix3fv(theEngine().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform() * circle_transform));
  theEngine().drawUnitQuad();

  // Enable stencil test and fill in
  glDisable(GL_DEPTH_TEST);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
  glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
  if (occluder) {
    fill()->fillInOccluder(this);
  } else {
    fill()->fillIn(this);
  }
  glDisable(GL_STENCIL_TEST);
}
