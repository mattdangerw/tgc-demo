#ifndef SRC_PARTICLE_H_
#define SRC_PARTICLE_H_

#include <glm/glm.hpp>
#include <list>

#include "renderer.h"

using std::list;

const float kParticleRadius = .012f;
const float kParticleMass = .1f;

struct Particle {
  glm::vec2 position, velocity;
  glm::vec4 color;
  float age, lifetime, alpha_decay;
};

struct Emitter {
  glm::vec2 position, velocity;
  glm::vec4 color;
  float particles_per_second, leftover_from_last_frame;
  float heat;
  //Stuff for explosion effect.
  glm::vec2 target, start, midway;
  float time_to_target, time_in_flight;
};

struct ParticleDrawInfo {
  glm::vec2 position;
  glm::vec4 color;
};

class ParticleDrawer : public Drawable {
  public:
    ParticleDrawer();
    ~ParticleDrawer();
    // Set up the VAOs and VBOs and what not.
    void init();
    void sendParticles(ParticleDrawInfo *particles, int num_particles);
    void draw(glm::mat3 view);
    void drawOcclude(glm::mat3 view) {}

  private:
    int num_particles_;
    // GL stuff
    Program *program_;
    GLuint texture_handle_, array_object_, quad_buffer_objects_[2], particle_buffer_object_;
    GLint modelview_handle_, size_handle_;
};

#endif  // SRC_PARTICLE_H_
