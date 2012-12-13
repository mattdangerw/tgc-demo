#include "game_state/state.h"

State the_state;

State &theState() {
  return the_state;
}

void State::init() {
  cloud_manager.init();
}