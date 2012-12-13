#ifndef UPDATER_H_
#define UPDATER_H_

#include <set>

#include "game_state/entity.h"

using std::set;

class Updater{
  public:
    Updater();
    ~Updater();
    void add(Entity *entity);
    void remove(Entity *entity);
    void updateAll(float delta_time);
  private:
    set<Entity *> entities_;
};

#endif  // UPDATER_H_