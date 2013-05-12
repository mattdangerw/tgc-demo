#ifndef SRC_POINT_MASS_H_
#define SRC_POINT_MASS_H_

#include <glm/glm.hpp>

class PointMass {
  public:
    PointMass();
    PointMass(glm::vec2 position, glm::vec2 velocity, float mass, float damping);
    ~PointMass();
    // Apply a force to the point mass (forces are cleared after each update).
    void applyForce(glm::vec2 force);
    // Add an impulse which will be added in entirely in the next time step. So be careful with this one.
    void applyImpulse(glm::vec2 impulse);
    void update(float delta_time);
    // Screw physics. Move this where you want.
    void setPosition(glm::vec2 position) { position_ = position; }
    glm::vec2 position() { return position_; }
    glm::vec2 velocity() { return velocity_; }

  private:
    glm::vec2 position_, velocity_, force_, impulse_;
    float mass_, damping_;
};

#endif  // SRC_POINT_MASS_H_
