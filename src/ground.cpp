#include "ground.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "transform2D.h"

Ground::Ground() {}

Ground::~Ground() {}

void Ground::init() {
  // TODO: please load from file or something.
  points_.push_back(glm::vec2(0.0f, 0.2f));
  points_.push_back(glm::vec2(0.3f, 0.19f));
  points_.push_back(glm::vec2(0.38f, 0.15f));
  points_.push_back(glm::vec2(0.6f, 0.145f));
  points_.push_back(glm::vec2(0.8f, 0.22f));
  points_.push_back(glm::vec2(1.2f, 0.17f));
  // Parents
  points_.push_back(glm::vec2(1.8f, 0.22f));
  points_.push_back(glm::vec2(2.3f, 0.22f));

  // Hill
  points_.push_back(glm::vec2(2.7f, 0.16f));
  points_.push_back(glm::vec2(3.0f, 0.34f));
  points_.push_back(glm::vec2(3.1f, 0.37f));
  points_.push_back(glm::vec2(3.25f, 0.345f));
  points_.push_back(glm::vec2(3.55f, 0.22f));
  points_.push_back(glm::vec2(3.75f, 0.16f));
  
  // First trees
  points_.push_back(glm::vec2(4.1f, 0.14f));
  points_.push_back(glm::vec2(4.3f, 0.16f));
  points_.push_back(glm::vec2(4.5f, 0.145f));
  points_.push_back(glm::vec2(4.7f, 0.165f));
  points_.push_back(glm::vec2(4.9f, 0.14f));
  points_.push_back(glm::vec2(5.1f, 0.15f));

  // Kids
  points_.push_back(glm::vec2(5.45f, 0.3f));
  points_.push_back(glm::vec2(5.85f, 0.35f));
  points_.push_back(glm::vec2(6.2f, 0.3f));

  // Valley
  points_.push_back(glm::vec2(6.5f, 0.2f));
  points_.push_back(glm::vec2(6.7f, 0.1f));
  points_.push_back(glm::vec2(6.8f, 0.09f));
  points_.push_back(glm::vec2(7.0f, 0.04f));
  points_.push_back(glm::vec2(7.4f, 0.12f));
  points_.push_back(glm::vec2(7.8f, 0.14f));
  
  // Mountain
  points_.push_back(glm::vec2(8.1f, 0.35f));
  points_.push_back(glm::vec2(8.13f, 0.31f));
  points_.push_back(glm::vec2(8.4f, 0.6f));
  points_.push_back(glm::vec2(8.6f, 0.35f));
  points_.push_back(glm::vec2(8.64f, 0.39f));

  // lil valley
  points_.push_back(glm::vec2(8.85f, 0.24f));
  points_.push_back(glm::vec2(9.15f, 0.16f));
  points_.push_back(glm::vec2(9.45f, 0.15f));

  // End
  points_.push_back(glm::vec2(9.65f, 0.20f));
  points_.push_back(glm::vec2(9.71f, 0.25f));
  points_.push_back(glm::vec2(9.77f, 0.25f));
  points_.push_back(glm::vec2(9.83f, 0.30f));
  points_.push_back(glm::vec2(9.99f, 0.30f));
  points_.push_back(glm::vec2(10.05f, 0.25f));
  points_.push_back(glm::vec2(10.11f, 0.25f));
  points_.push_back(glm::vec2(10.17f, 0.20f));

  initPathShape();
  background_.init("textures/background.dds");
  background_.setDisplayPriority(-99);
  background_.setCorners(glm::vec2(0.0f, 0.0f), glm::vec2(width(), 1.0f));
  background_.setOccluder(false);
  background_.setShadowed(true);
  Renderer::instance().addDrawable(&background_);
  initTrees();

  Renderer::instance().setLightPosition(glm::vec2(5.0f, 10.0f));
}

void Ground::initTrees() {
  trees_.resize(10);
  Tree *tree = &trees_[0];
  tree->init();
  glm::mat3 transform(1.0f);
  transform = translate2D(transform, glm::vec2(4.0f, 0.09f));
  transform = scale2D(transform, glm::vec2(0.22f));
  transform = rotate2D(transform, 2.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(6);

  tree = &trees_[1];
  tree->init();
  transform = glm::mat3(1.0f);
  transform = translate2D(transform, glm::vec2(4.2f, 0.10f));
  transform = scale2D(transform, glm::vec2(0.25f));
  transform = rotate2D(transform, 0.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(1);

  tree = &trees_[2];
  tree->init();
  transform = glm::mat3(1.0f);
  transform = translate2D(transform, glm::vec2(4.39f, 0.09f));
  transform = scale2D(transform, glm::vec2(0.22f));
  transform = rotate2D(transform, -4.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(6);

  tree = &trees_[3];
  tree->init();
  transform = glm::mat3(1.0f);
  transform = translate2D(transform, glm::vec2(4.62f, 0.11f));
  transform = scale2D(transform, glm::vec2(0.18f));
  transform = rotate2D(transform, 1.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(1);

  tree = &trees_[4];
  tree->init();
  transform = glm::mat3(1.0f);
  transform = translate2D(transform, glm::vec2(7.4f, 0.08f));
  transform = scale2D(transform, glm::vec2(0.16f));
  transform = rotate2D(transform, 0.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(6);

  tree = &trees_[5];
  tree->init();
  transform = glm::mat3(1.0f);
  transform = translate2D(transform, glm::vec2(7.65f, 0.09f));
  transform = scale2D(transform, glm::vec2(0.2f));
  transform = rotate2D(transform, 3.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(1);

  tree = &trees_[6];
  tree->init();
  transform = glm::mat3(1.0f);
  transform = translate2D(transform, glm::vec2(8.95f, 0.15f));
  transform = scale2D(transform, glm::vec2(0.2f));
  transform = rotate2D(transform, 0.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(6);

  tree = &trees_[7];
  tree->init();
  transform = glm::mat3(1.0f);
  transform = translate2D(transform, glm::vec2(9.15f, 0.1f));
  transform = scale2D(transform, glm::vec2(0.26f));
  transform = rotate2D(transform, 0.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(1);

  tree = &trees_[8];
  tree->init();
  transform = glm::mat3(1.0f);
  transform = translate2D(transform, glm::vec2(9.36f, 0.1f));
  transform = scale2D(transform, glm::vec2(0.24f));
  transform = rotate2D(transform, 0.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(6);

  tree = &trees_[9];
  tree->init();
  transform = glm::mat3(1.0f);
  transform = translate2D(transform, glm::vec2(9.5f, 0.12f));
  transform = scale2D(transform, glm::vec2(0.21f));
  transform = rotate2D(transform, 0.0f);
  tree->setTransform(transform);
  tree->setDisplayPriority(1);

  for (vector<Tree>::iterator it = trees_.begin(); it != trees_.end(); ++it) {
    Renderer::instance().addDrawable(&(*it));
  }
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
  shape_.init(path, &quad_);
  //shape_.setOccluder(false);
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

void Ground::getTargets(vector<Target> &targets) {
  Target ground_target;
  ground_target.entity = this;
  Renderer &renderer = Renderer::instance();
  float left = renderer.getLeftOfWindow();
  float win_width = renderer.windowWidth();
  float x_ground_target = left + win_width/2.0f;
  ground_target.position = glm::vec2(x_ground_target, heightAt(x_ground_target)/2.0f);
  ground_target_id_ = ground_target.id;
  targets.push_back(ground_target);
  Target background_target;
  background_target.entity = this;
  background_target.position = glm::vec2(left + win_width/2.0f, 0.5f);
  background_target_id_ = background_target.id;
  targets.push_back(background_target);
  Target tree6_target;
  tree6_target.entity = this;
  tree6_target.position = glm::vec2(9.0f, 0.30f);
  tree6_target_id_ = tree6_target.id;
  targets.push_back(tree6_target);
  Target tree7_target;
  tree7_target.entity = this;
  tree7_target.position = glm::vec2(9.2f, 0.32f);
  tree7_target_id_ = tree7_target.id;
  targets.push_back(tree7_target);
  Target tree8_target;
  tree8_target.entity = this;
  tree8_target.position = glm::vec2(9.39f, 0.29f);
  tree8_target_id_ = tree8_target.id;
  targets.push_back(tree8_target);
  Target tree9_target;
  tree9_target.entity = this;
  tree9_target.position = glm::vec2(9.33f, 0.28f);
  tree9_target_id_ = tree9_target.id;
  targets.push_back(tree9_target);
}

void Ground::colorTarget(Target target) {
  if (target.id == ground_target_id_) {
    quad_.setColorMask(glm::vec4(1.5f, 0.9f, 0.7f, 1.0f));
  } else if (target.id == background_target_id_) {
    background_.setColorMask(glm::vec4(0.55f, 0.87f, 0.98f, 1.0f));
  } else if (target.id == tree6_target_id_) {
    trees_[6].setColor(glm::vec4(0.0f, 0.8f, 1.0f, 1.0f));
  } else if (target.id == tree7_target_id_) {
    trees_[7].setColor(glm::vec4(0.0f, 0.8f, 0.8f, 1.0f));
  } else if (target.id == tree8_target_id_) {
    trees_[8].setColor(glm::vec4(0.0f, 0.9f, 0.6f, 1.0f));
  } else if (target.id == tree9_target_id_) {
    trees_[9].setColor(glm::vec4(0.0f, 0.6f, 0.9f, 1.0f));
  }
}
