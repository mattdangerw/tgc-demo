#include "game_state/entity.h"

#include "game_state/state.h"

Entity::Entity() : initialized_(false) {
  theState().updater.add(this);
}

Entity::~Entity() {
  theState().updater.remove(this);
}

void Entity::init() {
  initialized_ = true;
}
