#include "world/ground.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "util/transform2D.h"

Ground::Ground() {}

Ground::~Ground() {}

void Ground::init(vector<glm::vec2> points) {
  points_ = points;
  initShape();
}

void Ground::initShape() {
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
  fill_.init("content/textures/bookcover2.dds");
  fill_.setColorAddition(glm::vec4(glm::vec3(-0.25f), 1.0f));
  shape_.init(path);
  shape_.setParent(this);
  shape_.setFill(&fill_);
}

float Ground::width() {
  return points_.back().x;
}

float Ground::heightAt(float x) {
  // TODO: this could be way more efficient if we ever need it
  if (x < 0.0f || x > width()) return 0.0f;
  size_t index;
  for (index = 0; index < points_.size() - 1; index++) {
    if (points_[index+1].x > x) break;
  }
  glm::vec2 left = points_[index], right = points_[index+1];
  return glm::mix(left.y, right.y, (x - left.x) / (right.x - left.x));
}
