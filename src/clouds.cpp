#include "clouds.h"

#include "GL/glfw.h"

static const int kNumClouds = 6;
static const float kCloudMinSize = 0.1f;
static const float kCloudMaxSize = 0.4f;
static const float kCloudMinY = 0.6f;
static const float kCloudMaxY = 1.2f;
static const float kCloudMinVelocity = 0.03f;
static const float kCloudMaxVelocity = 0.05f;
static const float kCloudOffscreen = 0.4f;

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

void CloudManager::init(Renderer *renderer) {
  renderer_ = renderer;
  for (int i = 0; i < kNumClouds; ++i) {
    Cloud *cloud = new Cloud();
    cloud->init(renderer);
    placeCloudRandomly(cloud);
    clouds_.push_back(cloud);
  }
}

// Keeps clouds wrapping around viewable area.
void CloudManager::update(float delta_time) {
  list<Cloud *>::iterator it;
  for (it = clouds_.begin(); it != clouds_.end(); ++it) {
    Cloud *cloud = *it;
    float x_begin, x_end;
    cloud->xExtent(&x_begin, &x_end);
    float left_of_window = renderer_->getLeftOfWindow();
    if (x_end < left_of_window - kCloudOffscreen) {
      placeCloudRandomlyOnRight(cloud);
    } else if (x_begin > left_of_window + renderer_->windowWidth() + kCloudOffscreen - cloud->size()) {
      placeCloudRandomlyOnLeft(cloud);
    }
    cloud->update(delta_time);
  }
}

void CloudManager::placeCloudRandomly(Cloud *cloud) {
  cloud->setVelocity(randomFloat(kCloudMinVelocity, kCloudMaxVelocity));
  float size = randomFloat(kCloudMinSize, kCloudMaxSize);
  cloud->setSize(size);
  float y_position = randomFloat(kCloudMinY, kCloudMaxY - size);
  float left_of_window = renderer_->getLeftOfWindow();
  float min_x = left_of_window - kCloudOffscreen;
  float max_x = left_of_window  + renderer_->windowWidth() + kCloudOffscreen - size;
  cloud->setPosition(glm::vec2(randomFloat(min_x, max_x), y_position));
}

void CloudManager::placeCloudRandomlyOnLeft(Cloud *cloud) {
  cloud->setVelocity(randomFloat(kCloudMinVelocity, kCloudMaxVelocity));
  float size = randomFloat(kCloudMinSize, kCloudMaxSize);
  cloud->setSize(size);
  float y_position = randomFloat(kCloudMinY, kCloudMaxY - size);
  cloud->setPosition(glm::vec2(renderer_->getLeftOfWindow() - kCloudOffscreen, y_position));
}

void CloudManager::placeCloudRandomlyOnRight(Cloud *cloud) {
  cloud->setVelocity(randomFloat(kCloudMinVelocity, kCloudMaxVelocity));
  float size = randomFloat(kCloudMinSize, kCloudMaxSize);
  cloud->setSize(size);
  float y_position = randomFloat(kCloudMinY, kCloudMaxY - size);
  cloud->setPosition(glm::vec2(renderer_->getLeftOfWindow()  + renderer_->windowWidth() + kCloudOffscreen - size, y_position));
}
