#include "clouds.h"

#include <GL/glew.h>

static const float kCloudMinSize = 0.1f;
static const float kCloudMaxSize = 0.35f;
static const float kCloudMinY = 0.6f;
static const float kCloudMaxY = 1.0f;
static const float kCloudMinXDistance = 0.2f;
static const float kCloudMaxXDistance = 0.4f;
static const float kCloudMinVelocity = 0.025f;
static const float kCloudMaxVelocity = 0.04f;

static inline float randomFloat(float min, float max) {
  return min + rand()/(RAND_MAX/(max - min));
}

Cloud::Cloud() {}

Cloud::~Cloud() {}

void Cloud::init(Renderer *renderer) {
  renderer_ = renderer;
  renderer_->addDrawable(this);
}

void Cloud::update(float delta_time) {
  position_.x -= velocity_ * delta_time;
}

void Cloud::xExtent(float *x_begin, float *x_end) {
  *x_begin = position_.x;
  *x_end = position_.x + size_;
}

void Cloud::draw() {
  glColor3f(0.5f, 0.5f, 0.5f);
  glRectf(position_.x, position_.y, position_.x + size_, position_.y + size_);
}

CloudManager::CloudManager() {}

CloudManager::~CloudManager() {
  for (list<Cloud *>::iterator it = clouds_.begin(); it != clouds_.end(); ++it) {
    delete (*it);
  }
}

void CloudManager::init(Renderer *renderer, Ground *ground) {
  renderer_ = renderer;
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
    float left_of_window = renderer_->getLeftOfWindow();
    if (x_end < left_of_window) {
      it = clouds_.erase(it);
      // TODO: Fix cloud destructor and renderer remove.
      // delete cloud;
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
    Cloud *cloud = new Cloud();
    cloud->init(renderer_);
    cloud->setVelocity(randomFloat(kCloudMinVelocity, kCloudMaxVelocity));
    float size = randomFloat(kCloudMinSize, kCloudMaxSize);
    cloud->setSize(size);
    float y_position = randomFloat(kCloudMinY, kCloudMaxY - size);
    cloud->setPosition(glm::vec2(x_position, y_position));
    clouds_.push_back(cloud);
}
