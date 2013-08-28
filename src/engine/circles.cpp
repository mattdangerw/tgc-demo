#include "engine/circles.h"

#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

#include "util/transform2D.h"

CircleDrawer::CircleDrawer()
  : delta_radius_(0.0f) {}

CircleDrawer::~CircleDrawer() {}

void CircleDrawer::init(vector<Circle> *circles) {
  circles_ = circles;
  glm::vec2 square[4] = {glm::vec2(-1.0f, -1.0f), glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(-1.0f, 1.0f)};

  glGenVertexArrays(1, &array_object_);
  glGenBuffers(1, &buffer_object_);
  glBindVertexArray(array_object_);
  
  glBindBuffer(GL_ARRAY_BUFFER, buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
  GLuint handle = theEngine().attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  handle = theEngine().attributeHandle("bezier_coord");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void CircleDrawer::extent(glm::vec2 *min, glm::vec2 *max) {
  *min = glm::vec2(std::numeric_limits<float>::max());
  *max = glm::vec2(-std::numeric_limits<float>::max());
  for (vector<Circle>::iterator it = circles_->begin(); it != circles_->end(); ++it) {
    min->x = std::min(it->center.x - it->radius, min->x);
    min->y = std::min(it->center.y - it->radius, min->y);
    max->x = std::max(it->center.x + it->radius, min->x);
    max->y = std::max(it->center.y + it->radius, min->y);
  }
}

// void CircleDrawer::useScreenSpaceTexture(string texture_filename) {
//   use_texture_ = true;
//   texture_handle_ = theEngine().getTexture(texture_filename);
// }

void CircleDrawer::draw() {
  drawHelper(false);
}

void CircleDrawer::drawOccluder() {
  drawHelper(true);
}

void CircleDrawer::drawHelper(bool occluder) {
  glEnable(GL_STENCIL_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 0, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
  theEngine().useProgram("circles");
  glEnable(GL_DEPTH_TEST);
  for (vector<Circle>::iterator it = circles_->begin(); it != circles_->end(); ++it) {
    glm::mat3 circle_transform(1.0f);
    circle_transform = translate2D(circle_transform, it->center);
    circle_transform = scale2D(circle_transform, glm::vec2(it->radius + delta_radius_));
    glUniformMatrix3fv(theEngine().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform() * circle_transform));
    glBindVertexArray(array_object_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }
  glDisable(GL_DEPTH_TEST);
  // Fill in
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
