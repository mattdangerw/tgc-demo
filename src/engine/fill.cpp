#include "engine/fill.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/engine.h"
#include "util/transform2D.h"

static glm::mat3 calcModelview(Entity *entity) {
  glm::vec2 min, max;
  entity->extent(&min, &max);
  glm::mat3 modelview(1.0f);
  modelview = translate2D(modelview, min);
  modelview = scale2D(modelview, glm::vec2(max - min));
  return entity->fullTransform() * modelview;
}

static void fillWithColor(Entity *entity, glm::vec4 color) {
  theEngine().useProgram("minimal");
  glUniform4fv(theEngine().uniformHandle("color"), 1, glm::value_ptr(color));
  glUniformMatrix3fv(theEngine().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(calcModelview(entity)));
  theEngine().drawUnitQuad();
}

void Fill::fillInOccluder(Entity *entity) {
  fillWithColor(entity, glm::vec4(glm::vec3(entity->occluderColor()), 1.0f));
}

void ColoredFill::fillIn(Entity *entity) {
  fillWithColor(entity, color_);
}

TexturedFill::TexturedFill()
  : shadowed_(false),
    texture_handle_(0),
    texture_scale_(1.0f),
    color_multiplier_(1.0f),
    color_addition_(0.0f) {}

void TexturedFill::fillIn(Entity *entity) {
  glm::vec2 min, max, scale;
  entity->extent(&min, &max);
  scale = max - min;

  string program = shadowed_ ? "textured_with_shadows" : "textured";
  theEngine().useProgram(program);
  glUniformMatrix3fv(theEngine().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(calcModelview(entity)));
  glUniform4fv(theEngine().uniformHandle("color_mul"), 1, glm::value_ptr(color_multiplier_));
  glUniform4fv(theEngine().uniformHandle("color_add"), 1, glm::value_ptr(color_addition_));
  glUniform2fv(theEngine().uniformHandle("tex_scale"), 1, glm::value_ptr(scale * texture_scale_));
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_handle_);
  theEngine().drawUnitQuad();
}
