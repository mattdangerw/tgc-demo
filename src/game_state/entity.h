#ifndef ENTITY_H_
#define ENTITY_H_

class Entity {
  public:
    Entity();
    ~Entity();
    void init();
    bool initialized() { return initialized_; }
    virtual void update(float delta_time) = 0;
  private:
    bool initialized_;
};

#endif  // ENTITY_H_