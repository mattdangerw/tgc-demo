#include "circles.h"

#include <glm/gtc/type_ptr.hpp>

#include "transform2D.h"

CircleDrawer::CircleDrawer()
  : colored_program_(NULL),
    textured_program_(NULL),
    delta_radius_(0.0f),
    with_texture_(false) {}

CircleDrawer::~CircleDrawer() {}

void CircleDrawer::init(vector<Circle> *circles) {
  circles_ = circles;
  glm::vec2 square[4] = {glm::vec2(-1.0f, -1.0f), glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(-1.0f, 1.0f)};

  colored_program_ = Renderer::instance().getProgram("circles");
  textured_program_ = Renderer::instance().getProgram("circles_screen_textured");
  glGenVertexArrays(1, &array_object_);
  glGenBuffers(1, &buffer_object_);
  
  glBindVertexArray(array_object_);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_object_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
  GLint handle = colored_program_->attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  handle = colored_program_->attributeHandle("bezier_coord");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void CircleDrawer::addScreenSpaceTexture(string texture_filename) {
  with_texture_ = true;
  texture_handle_ = Renderer::instance().getTexture(texture_filename);
}

void CircleDrawer::drawHelper(glm::mat3 view, bool asOccluder) {
  glEnable(GL_DEPTH_TEST);
  Program *program;
  if (!asOccluder && with_texture_) {
    program = textured_program_;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_handle_);
  } else {
    program = colored_program_;
  }
  program->use();
  if (asOccluder) glUniform4fv(program->uniformHandle("color"), 1, glm::value_ptr(glm::vec4(0.0f)));
  for (vector<Circle>::iterator it = circles_->begin(); it != circles_->end(); ++it) {
    glm::mat3 circle_transform(1.0f);
    circle_transform = translate2D(circle_transform, it->center);
    circle_transform = scale2D(circle_transform, glm::vec2(it->radius + delta_radius_));
    glUniformMatrix3fv(program->uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(modelview(view) * circle_transform));
    if (!asOccluder && !with_texture_) {
      glUniform4fv(program->uniformHandle("color"), 1, glm::value_ptr(it->color));
    }
    glBindVertexArray(array_object_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }
  glDisable(GL_DEPTH_TEST);
}
