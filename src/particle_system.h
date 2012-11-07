#ifndef SRC_PARTICLES_H_
#define SRC_PARTICLES_H_

#include <string>
#include <list>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "thought_bubble.h"
#include "particle.h"
#include "game_entity.h"

using std::string;
using std::list;
using std::map;
using std::vector;

class EmitterTrack {
  public:
    EmitterTrack() : segment_(0), current_time_(0.0f), start_(glm::vec3(0.0f)) {}
    ~EmitterTrack() {}
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

struct Emitter {
  glm::vec3 position, velocity;
  glm::vec4 color;
  float particles_per_second, leftover_from_last_frame;
  float time_till_escape;
  EmitterTrack track;
};

class ParticleSystem : public GameEntity {
  public:
    ParticleSystem();
    ~ParticleSystem();
    void init(ThoughtBubble *thought_bubble);
    void addEmitters(int num_particles);
    void update(float delta_time, GameState *state);
    void setTargets(const vector<Target> &targets);
    bool targetWasHit(const Target target);

  private:
    void sortDepthIndex();

    ThoughtBubble *thought_bubble_;
    ParticleDrawInfo *render_data_;
    vector<Emitter> emitters_;
    vector<int> emitters_by_depth_;
    // Need fast deletion so list.
    map<int, list<Particle>> emitter_particles_;
    ParticleDrawer drawer_;
    map<int, int> target_to_emitter_;
    bool targets_;
};

#endif  // SRC_PARTICLES_H_
