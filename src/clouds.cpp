#include "clouds.h"

#include <GL/glew.h>

#include "renderer.h"
#include "transform2D.h"
#include "path_shape.h"
#include "quad.h"

static const float kCloudMinScale = 0.18f;
static const float kCloudMaxScale = 0.3f;
static const float kCloudMinY = 0.6f;
static const float kCloudMaxY = 1.0f;
static const float kCloudMinXDistance = 0.4f;
static const float kCloudMaxXDistance = 0.5f;
static const float kCloudMinVelocity = 0.03f;
static const float kCloudMaxVelocity = 0.035f;
static const float kCloudMinShade = 1.1f;
static const float kCloudMaxShade = 1.3f;

static inline float randomFloat(float min, float max) {
  return min + rand()/(RAND_MAX/(max - min));
}

Cloud::Cloud(glm::vec2 position, float velocity, float scale, float shade)
  : position_(position),
    velocity_(velocity),
    scale_(scale),
    shade_(shade) {}

Cloud::~Cloud() {
  Renderer::instance().removeDrawable(&shape_);
}

void Cloud::init() {
  quad_.init("textures/paper3.dds");
  quad_.setColorMask(glm::vec4(shade_, shade_, shade_, 1.0f));
  shape_.init("paths/cloud.path", &quad_, true, false);
  width_ = shape_.width() * scale_;
  Renderer::instance().addDrawable(&shape_);
  updateShapeTransform();
}

void Cloud::update(float delta_time) {
  position_.x -= velocity_ * delta_time;
  updateShapeTransform();
}

void Cloud::updateShapeTransform() {
  glm::mat3 shape_transform = scale2D(translate2D(glm::mat3(1.0f), position_), glm::vec2(scale_));
  shape_.setTransform(shape_transform);
}


void Cloud::xExtent(float *x_begin, float *x_end) {
  *x_begin = position_.x;
  *x_end = position_.x + width_;
}

CloudManager::CloudManager() {}

CloudManager::~CloudManager() {
  for (list<Cloud *>::iterator it = clouds_.begin(); it != clouds_.end(); ++it) {
    delete (*it);
  }
}

void CloudManager::init(Ground *ground) {
  ground_ = ground;
  float x_position = randomFloat(0.0f, kCloudMaxXDistance);
  while (x_position < ground_->width()) {
    addRandomCloud(x_position);
    x_position += randomFloat(kCloudMinXDistance, kCloudMaxXDistance);
  }
  dist_to_next_cloud_ = randomFloat(kCloudMinXDistance, kCloudMaxXDistance);
}

// Keeps clouds wrapping around viewable area.
void CloudManager::update(float delta_time, GameState *state) {
  if (*state == EXPLODING) return;
  list<Cloud *>::iterator it;
  float last_cloud_x = 0.0f;
  for (it = clouds_.begin(); it != clouds_.end();) {
    Cloud *cloud = *it;
    float x_begin, x_end;
    cloud->xExtent(&x_begin, &x_end);
    if (x_begin > last_cloud_x) last_cloud_x = x_begin;
    float left_of_window = Renderer::instance().getLeftOfWindow();
    if (x_end < 0.0f) {
      delete *it;
      it = clouds_.erase(it);
    } else {
      cloud->update(delta_time);
      ++it;
    }
  }
  if (ground_->width() - last_cloud_x > dist_to_next_cloud_) {
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
    cloud->init();
    clouds_.push_back(cloud);
}
