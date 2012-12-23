#include "game_state/entity.h"

#include "game_state/state.h"

Entity::Entity() : is_active_(false) {
  theState().updater.add(this);
}

Entity::~Entity() {
  theState().updater.remove(this);
}

