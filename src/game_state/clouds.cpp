#include "game_state/clouds.h"

#include <GL/glew.h>
#include <stdlib.h>

#include "render/renderer.h"
#include "render/path_shape.h"
#include "render/quad.h"
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
  quad_.init();
  quad_.useTexture("content/textures/paper3.dds");
  quad_.setColorMask(glm::vec4(shade_, shade_, shade_, 1.0f));
  quad_.setTextureScale(glm::vec2(scale_));
  switch (type) {
    case BIG_CLOUD:
      initBigCloudShape();
      break;
    case MEDIUM_CLOUD:
      initMediumCloudShape();
      break;
    case SMALL_CLOUD:
      initSmallCloudShape();
      break;
  }
  width_ = shape_.width() * scale_;
  updateShapeTransform();
}

void Cloud::initAnimator(float delay, bool small) {
  Animation slow_change;
  Keyframe frame1, frame2, frame3;
  frame1.index = 1;
  frame2.index = small ? 0 : 2;
  frame3.index = 0;
  frame1.time = delay;
  frame2.time = 2 * delay;
  frame3.time = 3 * delay;
  slow_change.addKeyframe(frame1);
  slow_change.addKeyframe(frame2);
  if (!small) slow_change.addKeyframe(frame3);
  slow_change.setRepeats(true);
  animator_.setStartKeyframe(0);
  animator_.addAnimation("slow_change", slow_change);
  animator_.queueAnimation("slow_change");
  animator_.update(randomFloat(0.0f, 3 * delay));
}

void Cloud::initBigCloudShape() {
  vector<string> filenames;
  filenames.push_back("content/paths/big_cloud1.path");
  filenames.push_back("content/paths/big_cloud2.path");
  filenames.push_back("content/paths/big_cloud3.path");
  initAnimator(25.0f, false);
  shape_.init(filenames, &quad_, &animator_);
}

void Cloud::initMediumCloudShape() {
  vector<string> filenames;
  filenames.push_back("content/paths/medium_cloud1.path");
  filenames.push_back("content/paths/medium_cloud2.path");
  filenames.push_back("content/paths/medium_cloud3.path");
  initAnimator(18.0f, false);
  shape_.init(filenames, &quad_, &animator_);
}

void Cloud::initSmallCloudShape() {
  vector<string> filenames;
  filenames.push_back("content/paths/small_cloud1.path");
  filenames.push_back("content/paths/small_cloud2.path");
  initAnimator(12.0f, true);
  shape_.init(filenames, &quad_, &animator_);
}

glm::vec2 Cloud::center() {
  return position_ + glm::vec2(width_/2.0f, scale_/2.0f);
}

void Cloud::update(float delta_time) {
  animator_.update(delta_time);
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
  quad_.setColorMask(color_mask);
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
  if (*state == PRE_EXPLODING || *state == EXPLODING) return;
  float last_cloud_x = 0.0f;
  list<Cloud *>::iterator it;
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
    if (*state != ENDING) {
      addRandomCloud(last_cloud_x + dist_to_next_cloud_);
      dist_to_next_cloud_ = randomFloat(kCloudMinXDistance, kCloudMaxXDistance);
    }
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

void CloudManager::getTargets(vector<Target> *targets) {
  for (list<Cloud *>::iterator it = clouds_.begin(); it != clouds_.end(); ++it) {
    Cloud *cloud = *it;
    float x_begin, x_end;
    cloud->xExtent(&x_begin, &x_end);
    float left_of_window = Renderer::instance().getLeftOfWindow();
    if (x_end > left_of_window) {
      Target target;
      target.position = cloud->center();
      target.entity = this;
      target.id = targets->size();
      targets->push_back(target);
      target_to_cloud_[target.id] = cloud;
    }
  }
}

void CloudManager::colorTarget(Target target) {
  target_to_cloud_[target.id]->setColorMask(kCloudColors[target.id%numColors]);
}