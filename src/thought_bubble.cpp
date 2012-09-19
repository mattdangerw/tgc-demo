#include "thought_bubble.h"

#include <cmath>

#include "GL/glfw.h"

static const float kBubbleWidth = 0.15f;
static const float kBubbleHeight = 0.1f;
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
  float intersect_radius = kBubbleWidth - kParticleRadius;
  // This is hardly a correct reflection but assuming the change in loction is small should work fine.
  // As long as our final output position is always inside the thought bubble somewhere this should be stable.
  if (glm::dot(particle.position, particle.position) > intersect_radius * intersect_radius) {
    particle.velocity = glm::reflect(particle.velocity, glm::normalize(particle.position));
    particle.position = old_position;
  }
}

void ThoughtBubble::draw() {
  //glm::vec2 bottom, top, corner;
  //corner = glm::vec2(kBubbleWidth/2.0f, kBubbleHeight/2.0f);
  //bottom = mass_.position() - corner;
  //top = mass_.position() + corner;
  //glRectf(bottom.x, bottom.y, top.x , top.y);
  float PI = 3.141592f;
  glColor3f(0.0f, 0.0f, 0.0f);
  glm::vec2 center = mass_.position();
  glBegin(GL_TRIANGLE_FAN);
  glVertex2fv(glm::value_ptr(center));
  for (int i = 0; i <= 360; i+=10) {
    float rads = i * PI / 180;
    glVertex2f(center.x + kBubbleWidth*cos(rads), center.y + kBubbleWidth*sin(rads));
  }
  glEnd();
}

glm::vec2 ThoughtBubble::anchorPoint() {
  return character_->position() + glm::vec2(0.0f, kCharacterDistance);
}
