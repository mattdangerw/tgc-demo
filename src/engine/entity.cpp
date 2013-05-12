#include "engine/entity.h"

#include "game/state.h"

Entity::Entity() : is_active_(false) {
  theState().updater.add(this);
}

Entity::~Entity() {
  theState().updater.remove(this);
}

