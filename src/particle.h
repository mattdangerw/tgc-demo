#ifndef SRC_PARTICLE_H_
#define SRC_PARTICLE_H_

#include <glm.hpp>

const float kParticleRadius = .01f;
const float kParticleMass = .1f;

struct Particle {
  glm::vec2 position, velocity;
  glm::vec3 color;
};

#endif  // SRC_PARTICLE_H_
