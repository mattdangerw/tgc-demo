#ifndef SRC_PARTICLE_H_
#define SRC_PARTICLE_H_

#include <glm/glm.hpp>
#include <list>

#include "renderer.h"

using std::list;

const float kParticleRadius = .012f;
const float kParticleMass = .25f;

struct Particle {
  glm::vec3 position, velocity;
  glm::vec4 color;
  float age, emitter;
};

class ParticleDrawer : public Drawable {
  public:
    ParticleDrawer();
    ~ParticleDrawer();
    // Set up the VAOs and VBOs and what not.
    void init();
    void setEmitterPosition(glm::vec3 postion);
    void setEmitterColor(glm::vec3 postion);
    void draw();
    glm::mat4 transform3D() { return transform3D_; }
    void setTransform3D(const glm::mat4 &transform) { transform3D_ = transform; }
    glm::mat3 transform2D() { return transform2D_; }
    void setTransform2D(const glm::mat3 &transform) { transform2D_ = transform; }
    glm::mat4 projection() { return projection_; }
    glm::mat4 inverseProjection() { return inverse_projection_; }

  private:
    int num_particles_;
    glm::mat4 projection_, inverse_projection_, transform3D_;
    glm::mat3 transform2D_;
    // GL stuff
    GLuint texture_handle_, array_object_, quad_buffer_objects_[2], particle_buffer_object_;
};

#endif  // SRC_PARTICLE_H_
