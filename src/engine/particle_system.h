#ifndef SRC_PARTICLE_SYSTEM_H_
#define SRC_PARTICLE_SYSTEM_H_

#include <glm/glm.hpp>
#include <list>

#include "engine/entity.h"
#include "engine/engine.h"

using std::list;

struct Particle {
  glm::vec3 position, velocity;
  glm::vec4 color;
  float age, visible;
};

class Emitter {
  public:
    Emitter();
    ~Emitter();
    void init(int num_particles);
    bool visible() { return visible_; }
    void setVisible(bool visible) { visible_ = visible; }
    glm::vec3 position() { return position_; }
    void setPosition(glm::vec3 position) { position_ = position; }
    glm::vec4 color() { return color_; }
    void setColor(glm::vec4 color) { color_ = color; }
    void update(float delta_time);
    // Binds current VAO and draws it. Uniforms set by particle system before hand.
    void drawArray();
  private:
    bool visible_;
    glm::vec3 position_;
    glm::vec4 color_;
    int num_particles_, current_source_, current_dest_;
    GLuint array_objects_[2], buffer_objects_[2];
    //GLuint transform_feedbacks_[2];
};

class ParticleSystem : public Drawable {
  public:
    ParticleSystem();
    ~ParticleSystem();
    // Set up the VAOs and VBOs and what not.
    void init(int num_emitters);
    void setEmitterPosition(int index, glm::vec3 position) { emitters_[index].setPosition(position); }
    void setEmitterColor(int index, glm::vec4 color) { emitters_[index].setColor(color); }
    void setEmitterVisible(int index, bool visible) { emitters_[index].setVisible(visible); }
    void update(float delta_time);
    void draw();
    glm::mat4 transform3D() { return transform3D_; }
    void setTransform3D(const glm::mat4 &transform) { transform3D_ = transform; }
    glm::mat3 transform2D() { return transform2D_; }
    void setTransform2D(const glm::mat3 &transform) { transform2D_ = transform; }
    glm::mat4 projection() { return projection_; }
    glm::mat4 inverseProjection() { return inverse_projection_; }

  private:
    void sortDepthIndex();
    vector<Emitter> emitters_;
    vector<int> emitters_by_depth_;
    glm::mat4 projection_, inverse_projection_, transform3D_;
    glm::mat3 transform2D_;
    // GL stuff
    GLuint texture_handle_;
};

#endif  // SRC_PARTICLE_SYSTEM_H_
