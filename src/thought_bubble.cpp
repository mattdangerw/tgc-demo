#include "thought_bubble.h"

#include <cmath>
#include <GL/glew.h>

#include "renderer.h"
#include "transform2D.h"

static const float kCharacterDistance = 0.35f;
static const float kHorizantalLeeway = 0.15f;
static const float kBubbleSpringConstant = 120.0f;
static const float kCirclesSpringConstant = 200.0f;

//StretchyCircle::StretchyCircle(glm::vec2 center, float rest_radius) 
//  : center_(center),
//    rest_radius_(rest_radius) {
//  spring_mass_ = PointMass(glm::vec2(rest_radius, 0.0f), glm::vec2(), 1.0f, 30.0f);
//}
//
//float StretchyCircle::radius() {
//  return spring_mass_.position().x;
//}
//
//void StretchyCircle::addImpulse(glm::vec2 impulse) {
//  spring_mass_.applyImpulse(glm::vec2(glm::length(impulse), 0.0f));
//}
//
//void StretchyCircle::update(float delta_time) {
//  float spring_force = -kCirclesSpringConstant * (radius() - rest_radius_);
//  spring_mass_.applyForce(glm::vec2(spring_force, 0.0f));
//  spring_mass_.update(delta_time);
//}

ThoughtBubble::ThoughtBubble() {}

ThoughtBubble::~ThoughtBubble() {
  Renderer::instance().removeDrawable(&circle_drawer_);
}

void ThoughtBubble::init(Character *character) {
  character_ = character;
  // Model the thought bubble as a point mass which is pulled along with the character.
  mass_ = PointMass(anchorPoint(), glm::vec2(), 1.0f, 10.0f);

  // Create collection of circles that make the thought bubble.
  Circle circle;
  circle.center = glm::vec2(0.0f, -0.03f);
  circle.radius = 0.11f;
  circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  circle.center = glm::vec2(0.09f, 0.0f);
  circle.radius = 0.1f;
  circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  circle.center = glm::vec2(-0.09f, 0.0f);
  circle.radius = 0.1f;
  circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  circle.center = glm::vec2(0.05f, 0.05f);
  circle.radius = 0.1f;
  circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  circle.center = glm::vec2(-0.05f, 0.05f);
  circle.radius = 0.1f;
  circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);

  // A point mass is used to model the stretch of each circle.
  // Each mass is displaced in the x direction and held to the origin with a spring force.
  // The strech in the radius is given by the x coord of the mass.
  for (vector<Circle>::iterator it = circles_.begin(); it != circles_.end(); ++it) {
    stretch_masses_.push_back(PointMass(glm::vec2(), glm::vec2(), 1.0f, 30.0f));
  }

  // Ready the circle drawer.
  circle_drawer_.init(&circles_);
  circle_drawer_.setDisplayPriority(2);
  Renderer::instance().addDrawable(&circle_drawer_);
}

void ThoughtBubble::update(float delta_time, GameState *state) {
  // A simple spring keeps our point mass following the character.
  // We need to calculate forces on out point mass and update the physics sim.
  glm::vec2 spring_anchor, spring_force;
  if (*state == EXPLODING) {
    Renderer &renderer = Renderer::instance();
    spring_anchor = glm::vec2(renderer.getLeftOfWindow() + renderer.windowWidth()/2, 0.75f);
    spring_force = -5.0f * (mass_.position() - spring_anchor);
  } else {
    spring_anchor = anchorPoint();
    // Let the actual spring anchor drift horizantally a bit.
    spring_anchor.x += glm::clamp(mass_.position().x - spring_anchor.x, -kHorizantalLeeway, kHorizantalLeeway);
    spring_force = -kBubbleSpringConstant * (mass_.position() - spring_anchor);
  }
  mass_.applyForce(spring_force);
  mass_.update(delta_time);

  // Update our stretch masses. And assign new radius to circle based on stretch
  for (size_t i = 0; i < circles_.size(); ++i) {
    PointMass &mass = stretch_masses_[i];
    circles_[i].radius = rest_radii_[i] + mass.position().x;
    glm::vec2 spring_force(-kCirclesSpringConstant * mass.position().x, 0.0f);
    mass.applyForce(spring_force);
    mass.update(delta_time);
  }

  // Update drawable.
  circle_drawer_.setTransform(translate2D(glm::mat3(1.0f), glm::vec2(mass_.position())));
}

// Large method, lot of room for optimization. But will usually exit early so seems fine for < 1000 particles.
void ThoughtBubble::collideParticle(Particle &particle, glm::vec2 old_position) {
  // Check if the particle is inside any of the bubble circles. If it is we are fine.
  for (size_t i = 0; i < circles_.size(); ++i) {
    float intersect_radius = circles_[i].radius - kParticleRadius;
    glm::vec2 center_to_particle = particle.position - circles_[i].center;
    if (glm::dot(center_to_particle, center_to_particle) < intersect_radius * intersect_radius) {
      return;
    }
  }
  // Find one of the bubble circles the particle was in last time step, and collide with that.
  Circle *nearest_circle = NULL;
  float min_distance_to_circle = float('inf');
  for (size_t i = 0; i < circles_.size(); ++i) {
    float intersect_radius = circles_[i].radius - kParticleRadius;
    glm::vec2 center_to_old_particle = old_position - circles_[i].center;
    float signed_distance = glm::length(center_to_old_particle) - intersect_radius;
    if (signed_distance < 0.0f) {
      glm::vec2 old_velocity = particle.velocity;
      // This is hardly a correct reflection but assuming the change in loction is small should work fine.
      particle.velocity = glm::reflect(particle.velocity, glm::normalize(center_to_old_particle));
      // Don't bother to calculate actual bounced position. We will just roll back the position change and leave the new velocity.
      particle.position = old_position;
      // Add impulse from collision to the stretch mass.
      glm::vec2 impulse = (particle.velocity - old_velocity) * kParticleMass;
      // We only care about the length of impulse as our strech masses only displace in x
      stretch_masses_[i].applyImpulse(glm::vec2(glm::length(impulse), 0.0f));
      return;
    } else if (signed_distance < min_distance_to_circle) {
      min_distance_to_circle = signed_distance;
      nearest_circle = &circles_[i];
    }
  }
  // Even the old position is outside of the entire thought bubble.
  // This is the worst case meaning the bubble has actually moved out from under the particle (not vice versa).
  // We'll find the closest point to move it back inside.
  glm::vec2 old_velocity = particle.velocity;
  glm::vec2 intersection_normal = glm::normalize(old_position - nearest_circle->center);
  particle.velocity = glm::reflect(particle.velocity, intersection_normal);
  particle.position = intersection_normal * (nearest_circle->radius - kParticleRadius) + nearest_circle->center;
  glm::vec2 impulse = (particle.velocity - old_velocity) * kParticleMass;
}

glm::vec2 ThoughtBubble::anchorPoint() {
  return character_->position() + glm::vec2(0.0f, kCharacterDistance);
}
