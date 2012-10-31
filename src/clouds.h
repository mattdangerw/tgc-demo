#ifndef SRC_CLOUDS_H_
#define SRC_CLOUDS_H_

#include <glm/glm.hpp>
#include <list>
#include <map>

#include "game_entity.h"
#include "ground.h"
#include "path_shape.h"
#include "quad.h"

using std::list;
using std::map;

enum CloudType {
  BIG_CLOUD = 0,
  MEDIUM_CLOUD,
  SMALL_CLOUD
};

class Cloud {
  public:
    Cloud(glm::vec2 position, float velocity, float scale, float shade);
    ~Cloud();
    void init(CloudType type);
    void initBigCloudShape();
    void initMediumCloudShape();
    void initSmallCloudShape();
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
    PathShape shape_;
    Quad quad_;
};

class CloudManager : public GameEntity {
  public:
    CloudManager();
    ~CloudManager();
    void init(Ground * ground);
    // Keeps clouds wrapping around viewable area.
    void update(float delta_time, GameState *state);
    void getTargets(vector<Target> *targets);
    void colorTarget(Target target);

  private:
    // Helper methods
    void addRandomCloud(float x_position);

    // Member data.
    Ground *ground_;
    list<Cloud *> clouds_;
    float dist_to_next_cloud_;
    map<int, Cloud *> target_to_cloud_;
};

#endif  // SRC_CLOUDS_H_
