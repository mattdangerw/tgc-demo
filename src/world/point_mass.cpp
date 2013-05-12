#include "game_state/point_mass.h"

PointMass::PointMass()
  : position_(0.0f),
    velocity_(0.0f),
    mass_(1.0f),
    damping_(0.0f),
    force_(0.0f),
    impulse_(0.0f) {}

PointMass::PointMass(glm::vec2 position, glm::vec2 velocity, float mass, float damping)
  : position_(position),
    velocity_(velocity),
    mass_(mass),
    damping_(damping),
    force_(0.0f),
    impulse_(0.0f) {}

PointMass::~PointMass() {}

void PointMass::applyForce(glm::vec2 force) {
  force_ += force;
}

void PointMass::applyImpulse(glm::vec2 impulse) {
  impulse_ += impulse;
}

// Step the point mass forward. Uses verlet for time integration, which should be stable enough
// unless the framerate drops dramatically.
void PointMass::update(float delta_time) {
  if (delta_time == 0.0f) {
    force_ = glm::vec2();
    return;
  }
  // Add impulse over one time step. If things get unstable this could be it.
  force_ += impulse_ / delta_time;
  // Add damping force.
  force_ -= damping_ * velocity_;
  // Calculate new position.
  glm::vec2 new_position = position_ + velocity_ * delta_time + force_ * delta_time * delta_time / mass_;
  // Recalculate velocity from the change in position.
  velocity_ = (new_position - position_) / delta_time;
  position_ = new_position;
  // Clear the forces for next update.
  force_ = glm::vec2();
  impulse_ = glm::vec2();
}
