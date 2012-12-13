#include "game_state/clouds.h"

#include <GL/glew.h>
#include <stdlib.h>

#include "render/renderer.h"
#include "util/transform2D.h"
#include "util/random.h"

static const float kCloudMinScale = 0.18f;
static const float kCloudMaxScale = 0.3f;
static const float kCloudMinY = 0.7f;
static const float kCloudMaxY = 1.0f;
static const float kCloudMinXDistance = 0.4f;
static const float kCloudMaxXDistance = 0.5f;
static const float kCloudMinVelocity = 0.03f;
static const float kCloudMaxVelocity = 0.035f;
static const float kCloudMinShade = 0.95f;
static const float kCloudMaxShade = 1.15f;

static const int numColors = 5;
static const glm::vec4 kCloudColors[5] = {glm::vec4(1.0f, 0.6f, 0.5f, 1.0f),  // yellow red
                                          glm::vec4(1.0f, 0.7f, 0.65f, 1.0f), // redish
                                          glm::vec4(1.0f, 0.65f, .85f, 1.0f), // pinkish
                                          glm::vec4(.97f, 0.75f, .97f, 1.0f), // i forget
                                          glm::vec4(0.7f, 0.65f, 1.0f, 1.0f)}; // purple

Cloud::Cloud(glm::vec2 position, float velocity, float scale, float shade)
  : position_(position),
    velocity_(velocity),
    scale_(scale),
    shade_(shade) {}

Cloud::~Cloud() {
  shape_.setParent(NULL);
}

void Cloud::init(CloudType type) {
  Entity::init();
  switch (type) {
    case BIG_CLOUD:
      shape_.init("big_cloud.group");
      break;
    case MEDIUM_CLOUD:
      shape_.init("medium_cloud.group");
      break;
    case SMALL_CLOUD:
      shape_.init("small_cloud.group");
      break;
  }
  shape_.setParent(theRenderer().rootNode());
  width_ = 0.5f;
  updateShapeTransform();
  shape_.animator().queueAnimation("slow_change");
  shape_.setColorMasks(glm::vec4(shade_, shade_, shade_, 1.0f));
}

glm::vec2 Cloud::center() {
  return position_ + glm::vec2(width_/2.0f, scale_/2.0f);
}

void Cloud::update(float delta_time) {
  shape_.animator().update(delta_time);
  position_.x -= velocity_ * delta_time;
  updateShapeTransform();
}

void Cloud::updateShapeTransform() {
  glm::mat3 shape_transform = scale2D(translate2D(glm::mat3(1.0f), position_), glm::vec2(scale_));
  shape_.setRelativeTransform(shape_transform);
}


void Cloud::xExtent(float *x_begin, float *x_end) {
  *x_begin = position_.x;
  *x_end = position_.x + width_;
}

void Cloud::setColorMask(glm::vec4 color_mask) {
  shape_.setColorMasks(color_mask);
}

CloudManager::CloudManager() {}

CloudManager::~CloudManager() {
  for (list<Cloud *>::iterator it = clouds_.begin(); it != clouds_.end(); ++it) {
    delete (*it);
  }
}

void CloudManager::init() {
  Entity::init();
  float x_position = randomFloat(0.0f, kCloudMaxXDistance);
  while (x_position < theRenderer().windowWidth()) {
    addRandomCloud(x_position);
    x_position += randomFloat(kCloudMinXDistance, kCloudMaxXDistance);
  }
  dist_to_next_cloud_ = randomFloat(kCloudMinXDistance, kCloudMaxXDistance);
}

// Keeps clouds wrapping around viewable area.
void CloudManager::update(float delta_time) {
  float last_cloud_x = 0.0f;
  list<Cloud *>::iterator it;
  for (it = clouds_.begin(); it != clouds_.end();) {
    Cloud *cloud = *it;
    float x_begin, x_end;
    cloud->xExtent(&x_begin, &x_end);
    if (x_begin > last_cloud_x) last_cloud_x = x_begin;
    float left_of_window = theRenderer().getLeftOfWindow();
    if (x_end < 0.0f) {
      delete *it;
      it = clouds_.erase(it);
    } else {
      cloud->update(delta_time);
      ++it;
    }
  }
  if (theRenderer().windowWidth() - last_cloud_x > dist_to_next_cloud_) {
    addRandomCloud(last_cloud_x + dist_to_next_cloud_);
    dist_to_next_cloud_ = randomFloat(kCloudMinXDistance, kCloudMaxXDistance);
  }
}

void CloudManager::addRandomCloud(float x_position) {
    float scale = randomFloat(kCloudMinScale, kCloudMaxScale);
    float velocity = randomFloat(kCloudMinVelocity, kCloudMaxVelocity);
    float shade = randomFloat(kCloudMinShade, kCloudMaxShade);
    float y_position = randomFloat(kCloudMinY, kCloudMaxY - scale);
    glm::vec2 position = glm::vec2(x_position, y_position);
    Cloud *cloud = new Cloud(position, velocity, scale, shade);
    CloudType type = BIG_CLOUD;
    if (scale < 0.21) {
      type = SMALL_CLOUD;
    } else if (scale < .25) {
      type = MEDIUM_CLOUD;
    }
    cloud->init(type);
    clouds_.push_back(cloud);
}
