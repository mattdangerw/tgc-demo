#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include "engine/entity.h"
#include "engine/shape_group.h"

class EventManager : public Entity {
  public:
    EventManager();
    ~EventManager();
    void init();
    void update(float delta_time);
};

#endif  // EVENT_MANAGER_H_
