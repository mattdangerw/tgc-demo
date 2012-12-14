#include "game_state/scroller.h"

#include <glm/glm.hpp>

#include "game_state/state.h"
#include "render/renderer.h"
#include "util/settings.h"

Scroller::Scroller() {}

Scroller::~Scroller() {}

void Scroller::init() {
  Entity::init();
}

void Scroller::update(float delta_time) {
  float window_width = theRenderer().windowWidth();
  float character_screen_x = getSetting("player_screen_x").getFloat();
  float left_of_screen = glm::clamp(theState().character.position().x - character_screen_x * window_width, 0.0f, theState().ground.width() - window_width);
  theRenderer().setLeftOfWindow(left_of_screen);
}