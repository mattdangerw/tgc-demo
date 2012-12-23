#ifndef ENTITY_H_
#define ENTITY_H_

class Entity {
  public:
    Entity();
    ~Entity();
    void setIsActive(bool active) { is_active_ = active; }
    bool isActive() { return is_active_; }
    virtual void update(float delta_time) = 0;
  private:
    bool is_active_;
};

#endif  // ENTITY_H_