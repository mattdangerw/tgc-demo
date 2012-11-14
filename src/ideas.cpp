#include "ideas.h"

#include <cmath>
#include <stdlib.h>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

#include "transform2D.h"
#include "random.h"

static const float kNearZBoundary = -2.2f;
static const float kFarZBoundary = -4.0f;

static inline glm::vec3 transformAndHomogenize(glm::mat4 transform, glm::vec3 point) {
  glm::vec4 transformed = transform * glm::vec4(point, 1.0);
  return glm::vec3(transformed.x, transformed.y, transformed.z) / transformed.w;
}

void IdeaTrack::addDestination(glm::vec3 control, glm::vec3 destination, float time) {
  controls_.push_back(control);
  destinations_.push_back(destination);
  times_.push_back(time);
}

glm::vec3 IdeaTrack::step(float delta_time) {
  if (done()) return start_;
  current_time_+=delta_time;
  float segment_time = times_[segment_];
  glm::vec3 control = controls_[segment_];
  glm::vec3 destination = destinations_[segment_];
  if (current_time_ > segment_time) {
    current_time_ = 0.0f;
    ++segment_;
    start_ = destination;
    return destination;
  }
  float t = current_time_ / segment_time;
  glm::vec3 midpoint1, midpoint2;
  midpoint1 = glm::mix(start_, control, t);
  midpoint2 = glm::mix(control, destination, t);
  return glm::mix(midpoint1, midpoint2, t);
}

IdeaManager::IdeaManager() : targets_(false) {}

IdeaManager::~IdeaManager() {}

void IdeaManager::init(ThoughtBubble *thought_bubble) {
  thought_bubble_ = thought_bubble;
  // TODO: get the number of particles somewhere else.
  drawer_.init(20);
  Renderer::instance().addDrawable3D(&drawer_);
}

void IdeaManager::addIdeas(int num_ideas) {
  for(int i = 0; i < num_ideas; ++i) {
    Idea to_add;
    to_add.position = glm::vec3(0.0f, 0.0f, kFarZBoundary);
    to_add.velocity = glm::vec3(randomDirection() * 0.2f, 0.08f);
    drawer_.setEmitterColor(ideas_.size(), glm::vec4(randomColor(), 0.4f));
    drawer_.setEmitterVisible(ideas_.size(), true);
    ideas_.push_back(to_add);
  }
}

void IdeaManager::update(float delta_time, GameState *state) {
  if (targets_) {
    for (size_t i = 0; i < ideas_.size(); ++i) {
      Idea &idea = ideas_[i];
      idea.time_till_escape-=delta_time;
    }
  }
  for (size_t i = 0; i < ideas_.size(); ++i) {
    Idea &idea = ideas_[i];
    if (targets_ && idea.time_till_escape < 0.0f) {
      if (!idea.track.done()) {
        idea.position = idea.track.step(delta_time);
        if (idea.track.done()) drawer_.setEmitterVisible(i, false);
      }
    } else {
      idea.position += idea.velocity * delta_time;
      glm::vec3 projected_position = transformAndHomogenize(drawer_.projection(), idea.position);
      glm::vec2 position2D(projected_position.x, projected_position.y);
      glm::vec2 velocity2D(idea.velocity.x, idea.velocity.y);
      if (thought_bubble_->collideIdea(&position2D, &velocity2D)) {
        idea.position = transformAndHomogenize(drawer_.inverseProjection(), glm::vec3(position2D.x, position2D.y, projected_position.z));
        idea.velocity = glm::vec3(velocity2D.x, velocity2D.y, idea.velocity.z);
      }
      if (idea.position.z > kNearZBoundary) {
        idea.position.z = kNearZBoundary;
        idea.velocity.z *= -1.0f;
      } else if (idea.position.z < kFarZBoundary) {
        idea.position.z = kFarZBoundary;
        idea.velocity.z *= -1.0f;
      }
    }
  }

  drawer_.setTransform2D(translate2D(glm::mat3(1.0f), thought_bubble_->center()));
  for (size_t i = 0; i < ideas_.size(); ++i) {
    drawer_.setEmitterPosition(i, ideas_[i].position);
  }
  drawer_.update(delta_time);
}

void IdeaManager::setTargets(const vector<Target> &targets) {
  targets_ = true;
  float time_till_escape = 0.0f;
  float delta_escape = 1.0f;
  for (size_t i = 0; i < ideas_.size(); ++i) {
    Idea &idea = ideas_[i];
    idea.track.setStart(idea.position);
    glm::vec3 control = glm::vec3(0.0f, 0.0f, -1.7f);
    glm::vec3 destination = glm::vec3(0.0, 0.0, -1.1f);
    idea.track.addDestination(control, destination, 1.0f);
    control = glm::vec3(0.0f, 0.0f, -0.7f);
    if (i < targets.size()) {
      Target target = targets[i];
      // Particle system coordinate system is centered on the thought bubble.
      // Transform target to that space.
      destination = transformAndHomogenize(drawer_.inverseProjection(), glm::vec3(target.position - thought_bubble_->center(), 0.81f));
      target_to_idea_[target.id] = i;
      idea.time_till_escape = time_till_escape;
      time_till_escape+=delta_escape;
      delta_escape*=0.85f;
    } else {
      Renderer &renderer = Renderer::instance();
      glm::vec2 randomScreenSpace(renderer.getLeftOfWindow() + randomFloat(0.0f, 1.0f) * renderer.windowWidth(),
        randomFloat(0.0f, 1.0f));
      // Transform to particle sys coordinate system.
      destination = transformAndHomogenize(drawer_.inverseProjection(), glm::vec3(randomScreenSpace - thought_bubble_->center(), 0.81f));
      // Release at random time over the interval of real targets.
      idea.time_till_escape = randomFloat(0.0f, time_till_escape - delta_escape);
    }
    idea.track.addDestination(control, destination, 1.5f);
  }
}

bool IdeaManager::targetWasHit(Target target) {
  if (target_to_idea_.count(target.id) > 0) {
    Idea &idea = ideas_[target_to_idea_[target.id]];
    return idea.track.done();
  }
  return true;
}
