#ifndef SRC_CLOUDS_H_
#define SRC_CLOUDS_H_

#include <glm/glm.hpp>
#include <list>
#include <map>

#include "game_state/entity.h"
#include "render/shape_group.h"

using std::list;
using std::map;

enum CloudType {
  BIG_CLOUD = 0,
  MEDIUM_CLOUD,
  SMALL_CLOUD
};

class Cloud : Entity {
  public:
    Cloud(glm::vec2 position, float velocity, float scale, float shade);
    ~Cloud();
    void init(CloudType type);
    // Gets the position of the bottom left corner of the cloud.
    glm::vec2 position() { return position_; }
    glm::vec2 center();
    // Gets the cloud velocity: a scalar representing how fast the cloud travels in -x direction.
    float velocity() { return velocity_; }
    // Gets the clouds vertical size.
    float scale() { return scale_; }
    // Keep that cloud moving left.
    void update(float delta_time);
    // Get the left and right extent of the renderable cloud object.
    void xExtent(float *x_begin, float *x_end);
    void setColorMask(glm::vec4 color_mask);

  private:
    void updateShapeTransform();
    // State.
    glm::vec2 position_;
    float scale_, velocity_, width_, shade_;
    // Drawables.
    ShapeGroup shape_;
};

class CloudManager : public Entity {
  public:
    CloudManager();
    ~CloudManager();
    void init();
    // Keeps clouds wrapping around viewable area.
    void update(float delta_time);

  private:
    // Helper methods
    void addRandomCloud(float x_position);

    // Member data.
    list<Cloud *> clouds_;
    float dist_to_next_cloud_;
};

#endif  // SRC_CLOUDS_H_
