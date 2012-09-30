#ifndef SRC_PARTICLE_H_
#define SRC_PARTICLE_H_

#include <glm.hpp>

#include "renderer.h"

const float kParticleRadius = .01f;
const float kParticleMass = .1f;
const float kMaxParticles = .1f;

struct Particle {
  glm::vec2 position, velocity;
  glm::vec4 color;
};

class ParticleDrawer : public Drawable {
  public:
    ParticleDrawer();
    ~ParticleDrawer();
    // Set up the VAOs and VBOs and what not.
    void init(vector<Particle> *paricles);
    void draw(glm::mat3 transform);

  private:
    vector<Particle> *particles_;
    // GL stuff
    Program *program_;
    GLuint texture_handle_, array_object_, quad_buffer_objects_[2], particle_buffer_object_;
    GLint modelview_handle_, size_handle_;
};

#endif  // SRC_PARTICLE_H_
