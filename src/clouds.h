#ifndef SRC_CLOUDS_H_
#define SRC_CLOUDS_H_

#include <string>
#include <list>
#include <glm.hpp>

#include "renderer.h"
#include "game_entity.h"
#include "ground.h"

using std::string;
using std::list;

class Cloud {
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

  private:
    Renderer *renderer_;
    glm::vec2 position_;
    float size_, velocity_;
};

class CloudManager : GameEntity {
  public:
    CloudManager();
    ~CloudManager();
    void init(Renderer *renderer, Ground * ground);
    // Keeps clouds wrapping around viewable area.
    void update(float delta_time, GameState *state);

  private:
    // Helper methods
    void addRandomCloud(float x_position);

    // Member data.
    Renderer *renderer_;
    Ground *ground_;
    list<Cloud *> clouds_;
    float dist_to_next_cloud_;
};

#endif  // SRC_CLOUDS_H_
