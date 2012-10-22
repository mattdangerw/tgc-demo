#include "circles.h"

#include <glm/gtc/type_ptr.hpp>

#include "transform2D.h"

CircleDrawer::CircleDrawer()
  : colored_program_(NULL),
    textured_program_(NULL),
    occluder_color_(0.0f, 0.0f, 0.0f, 1.0f),
    delta_radius_(0.0f),
    use_texture_(false),
    use_quad_(false) {}

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

void CircleDrawer::useScreenSpaceTexture(string texture_filename) {
  use_texture_ = true;
  texture_handle_ = Renderer::instance().getTexture(texture_filename);
}

void CircleDrawer::useQuad(Quad *quad) {
  use_quad_ = true;
  fill_ = quad;
}

void CircleDrawer::drawOcclude(glm::mat3 view) {
  colored_program_->use();
  glUniform4fv(colored_program_->uniformHandle("color"), 1, glm::value_ptr(glm::vec4(occluder_color_)));
  makeDrawCalls(view, colored_program_, false);
}

void CircleDrawer::draw(glm::mat3 view) {
  if (use_texture_) {
    drawWithScreenTexture(view);
  } else if (use_quad_) {
    drawWithQuad(view);
  } else {
    drawColored(view);
  }
}

void CircleDrawer::drawColored(glm::mat3 view) {
  colored_program_->use();
  makeDrawCalls(view, colored_program_, true);
}

void CircleDrawer::drawWithScreenTexture(glm::mat3 view) {
  textured_program_->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_handle_);
  makeDrawCalls(view, textured_program_, false);
}

void CircleDrawer::drawWithQuad(glm::mat3 view) {
  glEnable(GL_STENCIL_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 0, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
  colored_program_->use();
  makeDrawCalls(view, colored_program_, false);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
  glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
  fill_->draw(modelview(view));
  glDisable(GL_STENCIL_TEST);
}

void CircleDrawer::makeDrawCalls(glm::mat3 view, Program *program, bool sendColors) {
  glEnable(GL_DEPTH_TEST);
  for (vector<Circle>::iterator it = circles_->begin(); it != circles_->end(); ++it) {
    glm::mat3 circle_transform(1.0f);
    circle_transform = translate2D(circle_transform, it->center);
    circle_transform = scale2D(circle_transform, glm::vec2(it->radius + delta_radius_));
    glUniformMatrix3fv(program->uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(modelview(view) * circle_transform));
    if (sendColors) {
      glUniform4fv(program->uniformHandle("color"), 1, glm::value_ptr(it->color));
    }
    glBindVertexArray(array_object_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }
  glDisable(GL_DEPTH_TEST);
}
