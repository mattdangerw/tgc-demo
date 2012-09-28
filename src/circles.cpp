#include "circles.h"

#include <gtc/type_ptr.hpp>

#include "transform2D.h"

CircleDrawer::CircleDrawer()
  : transform_(glm::mat4(1.0f)),
    color_(glm::vec4(1.0f)),
    program_(NULL) {}

CircleDrawer::~CircleDrawer() {}

void CircleDrawer::init(vector<Circle> *circles) {
  circles_ = circles;
  glm::vec2 square[4] = {glm::vec2(-1.0f, -1.0f), glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(-1.0f, 1.0f)};

  program_ = Renderer::instance().getProgram("circles");
  glGenVertexArrays(1, &array_object_);
  glGenBuffers(2, buffer_objects_);
  
  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_objects_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
  GLint handle = program_->attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  handle = program_->attributeHandle("tex_coords");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  modelview_handle_ = program_->uniformHandle("modelview");
  color_handle_ = program_->uniformHandle("color");
}

void CircleDrawer::draw(glm::mat3 transform) {
  program_->use();
  glm::mat3 modelview = transform * transform_;
  glUniform4fv(color_handle_, 1, glm::value_ptr(color_));
  for (vector<Circle>::iterator it = circles_->begin(); it != circles_->end(); ++it) {
    glm::mat3 circle_transform(1.0f);
    circle_transform = translate2D(circle_transform, it->center);
    circle_transform = scale2D(circle_transform, glm::vec2(it->radius));
    glUniformMatrix3fv(modelview_handle_, 1, GL_FALSE, glm::value_ptr(modelview * circle_transform));
    glBindVertexArray(array_object_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }
}