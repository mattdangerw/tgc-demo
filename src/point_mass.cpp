#include "point_mass.h"

PointMass::PointMass() {}

PointMass::PointMass(glm::vec2 position, glm::vec2 velocity, float mass, float damping) {
  position_ = position;
  velocity_ = velocity;
  mass_ = mass;
  damping_ = damping;
  force_ = glm::vec2();
}

PointMass::~PointMass() {}

void PointMass::applyForce(glm::vec2 force) {
  force_ += force;
}

// Step the point mass forward. Uses verlet for time integration, which should be stable enough.
void PointMass::update(float delta_time) {
  if (delta_time == 0.0f) {
    force_ = glm::vec2();
    return;
  }
  // Add damping force.
  force_ -= damping_ * velocity_;
  // Calculate new position.
  glm::vec2 new_position = position_ + velocity_ * delta_time + force_ * delta_time * delta_time / mass_;
  // Recalculate velocity from the change in position.
  velocity_ = (new_position - position_) / delta_time;
  position_ = new_position;
  // Clear the forces for next update.
  force_ = glm::vec2();
}
