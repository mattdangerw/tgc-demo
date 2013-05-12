#include "game/scroller.h"

#include <glm/glm.hpp>

#include "game/state.h"
#include "engine/engine.h"
#include "util/settings.h"

Scroller::Scroller() {}

Scroller::~Scroller() {}

void Scroller::init() {
  setDoUpdate(true);
}

void Scroller::update(float delta_time) {
  float window_width = theEngine().windowWidth();
  float character_screen_x = getSetting("player_screen_x").getFloat();
  float left_of_screen = glm::clamp(theState().character.position().x - character_screen_x * window_width, 0.0f, theState().ground.width() - window_width);
  theEngine().setLeftOfWindow(left_of_screen);
}