#ifndef SRC_POINT_MASS_H_
#define SRC_POINT_MASS_H_

#include <glm.hpp>

class PointMass {
  public:
    PointMass();
    PointMass(glm::vec2 position, glm::vec2 velocity, float mass, float damping);
    ~PointMass();
    void applyForce(glm::vec2 force);
    void applyImpulse(glm::vec2 force);
    void update(float delta_time);
    glm::vec2 position() { return position_; }
    glm::vec2 velocity() { return velocity_; }

  private:
    glm::vec2 position_, velocity_, force_, impulse_;
    float mass_, damping_;
};

#endif  // SRC_POINT_MASS_H_
