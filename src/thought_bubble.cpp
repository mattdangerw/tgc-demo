#include "thought_bubble.h"

#include <cmath>

#include "GL/glfw.h"

static const float kCharacterDistance = 0.25f;
static const float kHorizantalLeeway = 0.08f;
static const float kSpringConstant = 120.0f;

ThoughtBubble::ThoughtBubble() {}

ThoughtBubble::~ThoughtBubble() {}

void ThoughtBubble::init(Renderer *renderer, Character *character) {
  renderer_ = renderer;
  character_ = character;
  // We model the thought bubble as a point mass which is pulled along with the character.
  mass_ = PointMass(anchorPoint(), glm::vec2(), 1.0f, 10.0f);

  Circle circle;
  circle.position = glm::vec2(0.0f, -0.03f);  
  circle.radius = 0.11f;
  circles_.push_back(circle);
  circle.position = glm::vec2(0.09f, 0.0f);
  circle.radius = 0.1f;
  circles_.push_back(circle);
  circle.position = glm::vec2(-0.09f, 0.0f);
  circle.radius = 0.1f;
  circles_.push_back(circle);
  circle.position = glm::vec2(0.05f, 0.05f);
  circle.radius = 0.1f;
  circles_.push_back(circle);
  circle.position = glm::vec2(-0.05f, 0.05f);
  circle.radius = 0.1f;
  circles_.push_back(circle);


  renderer_->addDrawable(this);
}

void ThoughtBubble::update(float delta_time, GameState *state) {
  // A simple spring keeps our point mass following the character.
  // We need to calculate forces on out point mass and update the physics sim.
  glm::vec2 spring_anchor, spring_force;
  if (*state == EXPLODING) {
    spring_anchor = glm::vec2(renderer_->getLeftOfWindow() + renderer_->windowWidth()/2, 0.75f);
    spring_force = -5.0f * (mass_.position() - spring_anchor);
  } else {
    spring_anchor = anchorPoint();
    // Let the actual spring anchor drift horizantally a bit.
    spring_anchor.x += glm::clamp(mass_.position().x - spring_anchor.x, -kHorizantalLeeway, kHorizantalLeeway);
    spring_force = -kSpringConstant * (mass_.position() - spring_anchor);
  }
  mass_.applyForce(spring_force);
  mass_.update(delta_time);
}

void ThoughtBubble::collideParticle(Particle &particle, glm::vec2 old_position) {
  vector<Circle>::iterator it;
  // Check if the particle is inside any of the bubble circles. If it is we are fine.
  for (it = circles_.begin(); it != circles_.end(); ++it) {
    float intersect_radius = it->radius - kParticleRadius;
    glm::vec2 centerToParticle = particle.position - it->position;
    if (glm::dot(centerToParticle, centerToParticle) < intersect_radius * intersect_radius) {
      return;
    }
  }
  // Find one of the bubble circles the particle was in last time step, and collid with that.
  for (it = circles_.begin(); it != circles_.end(); ++it) {
    float intersect_radius = it->radius - kParticleRadius;
    glm::vec2 centerToOldParticle = old_position - it->position;
    // This is hardly a correct reflection but assuming the change in loction is small should work fine.
    // As long as our final output position is always inside the thought bubble somewhere this should be stable.
    if (glm::dot(centerToOldParticle, centerToOldParticle) < intersect_radius * intersect_radius) {
      // TODO: Calculate impulse, change velocity * particle mass. Add impulse to point mass to simulate stretch for each circle.
      particle.velocity = glm::reflect(particle.velocity, glm::normalize(centerToOldParticle));
      // Don't bother to calculate actual bounced position. Just reposition to the last inside postion.
      particle.position = old_position;
    }
  }
}

void ThoughtBubble::draw() {
  float PI = 3.141592f;
  glColor3f(0.0f, 0.0f, 0.0f);
  glPushMatrix();
    glm::vec2 to_bubble_origin = mass_.position();
    glTranslatef(to_bubble_origin.x, to_bubble_origin.y, 0.0f);
    vector<Circle>::iterator it;
    for (it = circles_.begin(); it != circles_.end(); ++it) {
      glBegin(GL_TRIANGLE_FAN);
      glVertex2fv(glm::value_ptr(it->position));
      for (int i = 0; i <= 360; i+=10) {
        float rads = i * PI / 180;
        glVertex2f(it->position.x + it->radius*cos(rads), it->position.y + it->radius*sin(rads));
      }
      glEnd();
    }
  glPopMatrix();
}

glm::vec2 ThoughtBubble::anchorPoint() {
  return character_->position() + glm::vec2(0.0f, kCharacterDistance);
}
