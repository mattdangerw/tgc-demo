#include "ground.h"

#include <GL/glew.h>
#include <gtc/type_ptr.hpp>

Ground::Ground() {}

Ground::~Ground() {}

void Ground::init() {
  // TODO: load from file or something.
  points_.push_back(glm::vec2(0.0f, 0.2f));
  points_.push_back(glm::vec2(0.3f, 0.2f));
  points_.push_back(glm::vec2(0.38f, 0.15f));
  points_.push_back(glm::vec2(0.6f, 0.15f));
  points_.push_back(glm::vec2(1.0f, 0.3f));
  points_.push_back(glm::vec2(1.2f, 0.23f));
  points_.push_back(glm::vec2(1.3f, 0.23f));
  points_.push_back(glm::vec2(1.55f, 0.12f));
  points_.push_back(glm::vec2(1.9f, 0.12f));
  points_.push_back(glm::vec2(2.2f, 0.35f));
  points_.push_back(glm::vec2(2.23f, 0.31f));
  points_.push_back(glm::vec2(2.5f, 0.6f));
  points_.push_back(glm::vec2(2.7f, 0.35f));
  points_.push_back(glm::vec2(2.74f, 0.39f));
  points_.push_back(glm::vec2(2.95f, 0.24f));
  points_.push_back(glm::vec2(3.25f, 0.19f));
  points_.push_back(glm::vec2(3.65f, 0.19f));
  points_.push_back(glm::vec2(3.71f, 0.25f));
  points_.push_back(glm::vec2(3.91f, 0.25f));
  points_.push_back(glm::vec2(3.97f, 0.19f));
  initPathShape();
  background_.init("textures/background.dds");
  background_.setDisplayPriority(-1);
  background_.setCorners(glm::vec2(0.0f, 0.0f), glm::vec2(4.0f, 1.0f));
  Renderer::instance().addDrawable(&background_);
}

void Ground::initPathShape() {
  vector<PathVertex> path;
  PathVertex start, end;
  start.position = glm::vec2(0.0f, 0.0f);
  start.type = ON_PATH;
  path.push_back(start);
  for (vector<glm::vec2>::iterator it = points_.begin(); it != points_.end(); ++it) {
    PathVertex vertex;
    vertex.position = *it;
    vertex.type = ON_PATH;
    path.push_back(vertex);
  }
  end.position = glm::vec2(points_[points_.size() - 1].x, 0.0f);
  end.type = ON_PATH;
  path.push_back(end);
  quad_.init("textures/ground.dds");
  shape_.init(path, &quad_, true, false);
  Renderer::instance().addDrawable(&shape_);
}

float Ground::width() {
  return points_.back().x;
}

float Ground::heightAt(float x) {
  if (x < 0.0f || x > width()) return 0.0f;
  size_t index;
  for (index = 0; index < points_.size() - 1; index++) {
    if (points_[index+1].x > x) break;
  }
  glm::vec2 left = points_[index], right = points_[index+1];
  return glm::mix(left.y, right.y, (x - left.x) / (right.x - left.x));
}
