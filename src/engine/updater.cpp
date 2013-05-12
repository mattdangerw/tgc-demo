#include "engine/updater.h"

Updater::Updater() {}

Updater::~Updater() {}

void Updater::add(Entity *entity) {
  entities_.insert(entity);
}

void Updater::remove(Entity *entity) {
  entities_.erase(entity);
}

void Updater::updateAll(float delta_time) {
  for(set<Entity *>::iterator it = entities_.begin(); it != entities_.end(); ++it) {
    if ((*it)->doUpdate()) (*it)->update(delta_time);
  }
}
