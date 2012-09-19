#ifndef SRC_CLOUD_H_
#define SRC_CLOUD_H_

#include <string>

#include "glm.hpp"
#include "renderer.h"

using std::string;

class Cloud : Drawable {
  public:
    Cloud();
    ~Cloud();
    void init(Renderer *renderer);
    // Gets the position of the bottom left corner of the cloud.
    glm::vec2 position() { return position_; }
    // Gets the cloud velocity: a scalar representing how fast the cloud travels in -x direction.
    float velocity() { return velocity_; }
    // Gets the clouds horizantal size.
    float size() { return size_; }
    // Sets the position of the bottom left corner of the cloud.
    void setPosition(glm::vec2 position) { position_ = position; }
    // Sets the cloud velocity: a scalar representing how fast the cloud travels in -x direction.
    void setVelocity(float velocity) { velocity_ = velocity; }
    // Sets the clouds horizantal size.
    void setSize(float size) { size_ = size; }
    // Keep that cloud moving left.
    void update(float delta_time);
    // Get the left and right extent of the renderable cloud object.
    void xExtent(float *x_begin, float *x_end);
    void draw();
    string shadingGrounp() { return "default"; }

  private:
    Renderer *renderer_;
    glm::vec2 position_;
    float size_, velocity_;
};

#endif  // SRC_CLOUD_H_
