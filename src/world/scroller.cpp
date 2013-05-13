#include "world/scroller.h"

#include <glm/glm.hpp>

#include "world/world.h"
#include "engine/engine.h"
#include "util/settings.h"
#include "util/transform2D.h"

Scroller::Scroller()
  : character_screen_x_(0.0f), 
    scroll_(0.0f) {}

Scroller::~Scroller() {}

void Scroller::init() {
  character_screen_x_ = getSetting("player_screen_x").getFloat();
}

void Scroller::update(float delta_time) {
  float window_width = theEngine().windowWidth();
  scroll_ = glm::clamp(theWorld().character.position().x - character_screen_x_ * window_width, 0.0f, theWorld().ground.width() - window_width);
  glm::mat3 transform(1.0f);
  transform = translate2D(transform, glm::vec2(-scroll_, 0.0f));
  theWorld().setRelativeTransform(transform);
}