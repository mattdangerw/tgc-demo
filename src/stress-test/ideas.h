#ifndef SRC_IDEAS_H_
#define SRC_IDEAS_H_

#include <string>
#include <list>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "world/thought_bubble.h"
#include "world/game_entity.h"
#include "engine/particle_system.h"

using std::string;
using std::map;
using std::vector;

class IdeaTrack {
  public:
    IdeaTrack() : segment_(0), current_time_(0.0f), start_(glm::vec3(0.0f)) {}
    ~IdeaTrack() {}
    void setStart(glm::vec3 start) { start_ = start; }
    void addDestination(glm::vec3 control, glm::vec3 destination, float time);
    glm::vec3 step(float delta_time);
    bool done() { return segment_ == controls_.size(); }
  private:
    int segment_;
    float current_time_;
    glm::vec3 start_;
    vector<glm::vec3> controls_;
    vector<glm::vec3> destinations_;
    vector<float> times_;
};

struct Idea {
  glm::vec3 position, velocity;
  float time_till_escape;
  IdeaTrack track;
};

class IdeaManager : public GameEntity {
  public:
    IdeaManager();
    ~IdeaManager();
    void init(ThoughtBubble *thought_bubble);
    void addIdeas(int num_particles);
    void update(float delta_time, GameState *state);
    void setTargets(const vector<Target> &targets);
    bool targetWasHit(const Target target);

  private:
    ThoughtBubble *thought_bubble_;
    vector<Idea> ideas_;
    ParticleSystem drawer_;
    bool targets_;
    map<int, int> target_to_idea_;
};

#endif  // SRC_IDEAS_H_
