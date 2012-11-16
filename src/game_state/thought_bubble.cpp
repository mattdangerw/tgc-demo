#include "game_state/thought_bubble.h"

#include <cmath>
#include <limits>
#include <GL/glew.h>

#include "render/renderer.h"
#include "util/transform2D.h"

static const float kCharacterDistance = 0.35f;
static const float kHorizantalLeeway = 0.18f;
static const float kBubbleSpringConstant = 120.0f;
// For collisions
const float kIdeaRadius = .012f;
const float kIdeaMass = .25f;

void SubBubble::init(vector<Circle> *circles, float texture_scale, float darkness) {
  outer_fill_.init();
  outer_fill_.useTexture("content/textures/thought_bubble.dds");
  outer_fill_.setTextureScale(glm::vec2(texture_scale));
  outer_fill_.setCorners(glm::vec2(-0.3f, -0.2f), glm::vec2(0.3f, 0.2f));
  outer_fill_.setColorMask(glm::vec4(darkness, darkness, darkness, 1.0f));
  outer_drawer_.init(circles);
  //outer_drawer_.changeRadii(border_width);
  outer_drawer_.useQuad(&outer_fill_);
  outer_drawer_.setParent(this);
  outer_drawer_.setIsVisible(false);
  //inner_fill_.init("content/textures/inner_thought_bubble.dds");
  //inner_fill_.setTextureScale(glm::vec2(texture_scale));
  //inner_fill_.setCorners(glm::vec2(-0.3f, -0.2f), glm::vec2(0.3f, 0.2f));
  //inner_drawer_.init(circles);
  //inner_drawer_.useQuad(&inner_fill_);
  //inner_drawer_.setParent(this);
  //inner_drawer_.setIsVisible(false);
}

ThoughtBubble::ThoughtBubble()
  : ready_to_animate_(false),
    circles_spring_constant_(100.0f) {}

ThoughtBubble::~ThoughtBubble() {}

void ThoughtBubble::init(Character *character) {
  character_ = character;
  // Model the thought bubble as a point mass which is pulled along with the character.
  bubble_mass_ = PointMass(anchorPoint(), glm::vec2(), 1.0f, 10.0f);
  position_ = anchorPoint();

  // Create collection of circles that make the thought bubble.
  glm::vec4 color(0.3f, 0.3f, 0.3f, 1.0f);
  Circle circle;
  circle.center = glm::vec2(-0.035f, -0.035f);
  circle.radius = 0.1f;
  circle.color = color;
  bubble_circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  circle.center = glm::vec2(0.05f, -0.032f);
  circle.radius = 0.1f;
  circle.color = color;
  bubble_circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  circle.center = glm::vec2(0.1f, 0.01f);
  circle.radius = 0.09f;
  circle.color = color;
  bubble_circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  circle.center = glm::vec2(-0.09f, 0.0f);
  circle.radius = 0.09f;
  circle.color = color;
  bubble_circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  circle.center = glm::vec2(0.04f, 0.06f);
  circle.radius = 0.1f;
  circle.color = color;
  bubble_circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  circle.center = glm::vec2(-0.05f, 0.05f);
  circle.radius = 0.1f;
  circle.color = color;
  bubble_circles_.push_back(circle);
  rest_radii_.push_back(circle.radius);
  // Sub-bubble circles.
  glm::vec4 sub_color(0.3f, 0.3f, 0.3f, 1.0f);
  circle.center = glm::vec2(0.0f, -0.03f);
  circle.radius = 0.11f;
  circle.color = sub_color;
  sub_bubble_circles_.push_back(circle);
  circle.center = glm::vec2(0.09f, 0.0f);
  circle.radius = 0.1f;
  circle.color = sub_color;
  sub_bubble_circles_.push_back(circle);
  circle.center = glm::vec2(-0.08f, 0.0f);
  circle.radius = 0.11f;
  circle.color = sub_color;
  sub_bubble_circles_.push_back(circle);
  circle.center = glm::vec2(0.05f, 0.05f);
  circle.radius = 0.1f;
  circle.color = sub_color;
  sub_bubble_circles_.push_back(circle);
  circle.center = glm::vec2(-0.06f, 0.05f);
  circle.radius = 0.1f;
  circle.color = sub_color;
  sub_bubble_circles_.push_back(circle);

  // A point mass is used to model the stretch of each circle.
  // Each mass is displaced in the x direction and held to the origin with a spring force.
  // The strech in the radius is given by the x coord of the mass.
  for (vector<Circle>::iterator it = bubble_circles_.begin(); it != bubble_circles_.end(); ++it) {
    stretch_masses_.push_back(PointMass(glm::vec2(), glm::vec2(), 1.0f, 30.0f));
  }

  // Ready the circle drawer.
  fill_.init();
  fill_.useTexture("content/textures/thought_bubble.dds");
  fill_.setTextureScale(glm::vec2(2.0f));
  fill_.setCorners(glm::vec2(-0.3f, -0.2f), glm::vec2(0.3f, 0.2f));
  circle_drawer_.init(&bubble_circles_);
  circle_drawer_.useQuad(&fill_);
  circle_drawer_.changeRadii(0.01f);
  circle_drawer_.setDisplayPriority(100);
  circle_drawer_.setIsOccluder(false);
  // Ready the circle drawer.
  circle_inside_drawer_.init(&bubble_circles_);
  circle_inside_drawer_.useScreenSpaceTexture("content/textures/motion_blur1.dds");
  circle_inside_drawer_.setDisplayPriority(101);
  circle_inside_drawer_.setIsOccluder(false);
  circle_inside_drawer_.setIs3DStencil(true);
  circle_inside_drawer_.setParent(&circle_drawer_);
  // same for sub bubble
  sub_bubble_.init(&sub_bubble_circles_, 1.0f, 0.85f);
  sub_bubble_.setDisplayPriority(99);

  sub_bubble2_.init(&sub_bubble_circles_, 0.5f, 0.7f);
  sub_bubble2_.setDisplayPriority(98);

  sub_bubble3_.init(&sub_bubble_circles_, 0.3f, 0.55f);
  sub_bubble3_.setDisplayPriority(97);
}

void ThoughtBubble::update(float delta_time, GameState *state) {
  if (*state == ENDING) return;
  // A simple spring keeps our point mass following the character.
  // We need to calculate forces on out point mass and update the physics sim.
  glm::vec2 spring_anchor, spring_force;
  if (*state == PRE_EXPLODING) {
    if (!ready_to_animate_) {
      sub_bubble_.setIsVisible(false);
      sub_bubble2_.setIsVisible(false);
      sub_bubble3_.setIsVisible(false);
      Renderer &renderer = Renderer::instance();
      start_ = bubble_mass_.position();
      end_ = glm::vec2(renderer.getLeftOfWindow() + renderer.windowWidth()/2, 0.75f);
      midway_ = glm::mix(start_, end_, 0.8f);
      time_to_destination_ = 4.0f;
      time_in_flight_ = 0.0f;
      ready_to_animate_ = true;
      in_position_ = false;
      for (vector<Circle>::iterator it = bubble_circles_.begin(); it != bubble_circles_.end(); ++it) {
        circle_starts_.push_back(it->center);
      }
    }
    if (in_position_) {
      if ( time_in_position_ > time_to_explode_ ) {
        *state = EXPLODING;
      } else {
        time_in_position_+=delta_time;
        circles_spring_constant_ -= delta_time * 20.0f;
      }
    } else {
      if (time_in_flight_ > time_to_destination_) {
        in_position_ = true;
        time_to_explode_ = 4.0f;
        time_in_position_ = 0.0f;
      } else {
          float t = time_in_flight_ / time_to_destination_;
          glm::vec2 start_mid = glm::mix(start_, midway_, t);
          glm::vec2 mid_end = glm::mix(midway_, end_, t);
          position_ = glm::mix(start_mid, mid_end, t);
          for (size_t i = 0; i < bubble_circles_.size(); ++i) {
            bubble_circles_[i].center = glm::mix(circle_starts_[i], glm::vec2(), t);
          }
          time_in_flight_+=delta_time;
      }
    }
  } else if (*state != EXPLODING) {
    spring_anchor = anchorPoint();
    // Let the actual spring anchor drift horizantally a bit.
    spring_anchor.x += glm::clamp(bubble_mass_.position().x - spring_anchor.x, -kHorizantalLeeway, kHorizantalLeeway);
    spring_force = -kBubbleSpringConstant * (bubble_mass_.position() - spring_anchor);
    bubble_mass_.applyForce(spring_force);
    bubble_mass_.update(delta_time);
    position_ = bubble_mass_.position();
    if (position_.y > 0.84f) position_.y = 0.84f;
  }
  // Update drawable.
  circle_drawer_.setRelativeTransform(translate2D(glm::mat3(1.0f), position_));
  glm::mat3 sub_transform(1.0f);
  glm::vec2 character_position = character_->position();//groundPosition();
  glm::vec2 bubble_edge = position_ + glm::normalize(character_position - position_) * .05f;
  sub_transform = translate2D(sub_transform, glm::mix(bubble_edge, character_position, 0.1f));
  sub_transform = translate2D(sub_transform, glm::vec2(0.0f, -0.1f));
  sub_transform = scale2D(sub_transform, glm::vec2(0.32f));
  sub_bubble_.setRelativeTransform(sub_transform);

  glm::mat3 sub_transform2(1.0f);
  sub_transform2 = translate2D(sub_transform2, glm::mix(bubble_edge, character_position, 0.45f));
  sub_transform2 = translate2D(sub_transform2, glm::vec2(0.0f, -0.07f));
  sub_transform2 = scale2D(sub_transform2, glm::vec2(0.2f));
  sub_bubble2_.setRelativeTransform(sub_transform2);

  glm::mat3 sub_transform3(1.0f);
  sub_transform3 = translate2D(sub_transform3, glm::mix(bubble_edge, character_position, 0.8f));
  sub_transform3 = scale2D(sub_transform3, glm::vec2(0.14f));
  sub_bubble3_.setRelativeTransform(sub_transform3);

  // Update our stretch masses. And assign new radius to circle based on stretch
  for (size_t i = 0; i < bubble_circles_.size(); ++i) {
    PointMass &mass = stretch_masses_[i];
    bubble_circles_[i].radius = rest_radii_[i] + mass.position().x;
    glm::vec2 spring_force(-circles_spring_constant_ * mass.position().x, 0.0f);
    mass.applyForce(spring_force);
    mass.update(delta_time);
  }
}

bool ThoughtBubble::collideIdea(glm::vec2 *position, glm::vec2 *velocity) {
  // Check if the idea is inside any of the bubble circles. If it is no collision.
  for (size_t i = 0; i < bubble_circles_.size(); ++i) {
    float intersect_radius = bubble_circles_[i].radius - kIdeaRadius;
    glm::vec2 center_to_particle = *position - bubble_circles_[i].center;
    if (glm::dot(center_to_particle, center_to_particle) < intersect_radius * intersect_radius) {
      return false;
    }
  }
  // We've collided with a circle.
  // Find the distance to the nearest circle.
  Circle *nearest_circle = NULL;
  PointMass *nearest_stretch_mass;
  float min_distance_to_circle = std::numeric_limits<float>::max();
  for (size_t i = 0; i < bubble_circles_.size(); ++i) {
    float intersect_radius = bubble_circles_[i].radius - kIdeaRadius;
    glm::vec2 center_to_particle = *position - bubble_circles_[i].center;
    float signed_distance = glm::length(center_to_particle) - intersect_radius;
    if (signed_distance < min_distance_to_circle) {
      min_distance_to_circle = signed_distance;
      nearest_circle = &bubble_circles_[i];
      nearest_stretch_mass = &stretch_masses_[i];
    }
  }
  // Calculate result of intersection with nearest circle.
  glm::vec2 old_velocity = *velocity;
  glm::vec2 intersection_normal = glm::normalize(*position - nearest_circle->center);
  if (glm::dot(intersection_normal, *velocity) >= 0) {
    *velocity = glm::reflect(*velocity, intersection_normal);
  }
  *position = intersection_normal * (nearest_circle->radius - kIdeaRadius) + nearest_circle->center;
  glm::vec2 impulse = (*velocity - old_velocity) * kIdeaMass;
  nearest_stretch_mass->applyImpulse(glm::vec2(glm::length(impulse), 0.0f));
  return true;
}

glm::vec2 ThoughtBubble::anchorPoint() {
  return character_->groundPosition() + glm::vec2(0.0f, kCharacterDistance);
}

void ThoughtBubble::shrink(float scale) {
  for (size_t i = 0; i < bubble_circles_.size(); i++) {
    float new_rest = rest_radii_[i] * 0.7f;
    float delta_rest = rest_radii_[i] - new_rest;
    stretch_masses_[i].setPosition(stretch_masses_[i].position() + glm::vec2(delta_rest, 0.0f));
    rest_radii_[i] = new_rest;
  }
}

void ThoughtBubble::stopDrawing() {
  circle_drawer_.setIsVisible(false);
  circle_inside_drawer_.setIsVisible(false);
  sub_bubble_.setIsVisible(false);
  sub_bubble2_.setIsVisible(false);
  sub_bubble3_.setIsVisible(false);
}