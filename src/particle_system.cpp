#include "particle_system.h"

#include <cmath>

#include "GL/glfw.h"
#include "gtc/type_ptr.hpp"

static inline float randomFloat(float min, float max) {
  return min + rand()/(RAND_MAX/(max - min));
}

ParticleSystem::ParticleSystem() {}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::init(Renderer *renderer, ThoughtBubble *thought_bubble) {
  renderer_ = renderer;
  thought_bubble_ = thought_bubble;
  renderer_->addDrawable(this);
}

void ParticleSystem::addParticles(int num_particles) {
  for(int i = 0; i < num_particles; i++) {
    Particle to_add;
    to_add.color = glm::vec3(randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f));
    to_add.position = glm::vec2(0.0f, 0.1f);
    to_add.velocity = glm::vec2(randomFloat(-0.4f, 0.4f), randomFloat(-0.4f, 0.4f));
    particles_.push_back(to_add);
  }
}

void ParticleSystem::update(float delta_time, GameState *state) {
  vector<Particle>::iterator it;
  for (it = particles_.begin(); it != particles_.end(); ++it) {
    glm::vec2 old_position = it->position;
    it->position += it->velocity * delta_time;
    thought_bubble_->collideParticle(*it, old_position);
    (*it).position += (*it).velocity * delta_time;
  }
}

void ParticleSystem::draw() {
  float PI = 3.141592f;
  glPushMatrix();
    glm::vec2 to_particle_origin = thought_bubble_->center();
    glTranslatef(to_particle_origin.x, to_particle_origin.y, 0.0f);
    vector<Particle>::iterator it;
    for (it = particles_.begin(); it != particles_.end(); ++it) {
      glColor3fv(glm::value_ptr((*it).color));
      glm::vec2 center = (*it).position;
      glBegin(GL_TRIANGLE_FAN);
      glVertex2fv(glm::value_ptr(center));
      for (int i = 0; i <= 360; i+=60) {
        float rads = i * PI/180;
        glVertex2f(center.x + kParticleRadius*cos(rads), center.y + kParticleRadius*sin(rads));
      }
      glEnd();
    }
  glPopMatrix();
}
