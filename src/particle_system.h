#ifndef SRC_PARTICLES_H_
#define SRC_PARTICLES_H_

#include <string>
#include <list>
#include <map>
#include <glm/glm.hpp>

#include "thought_bubble.h"
#include "particle.h"
#include "game_entity.h"

using std::string;
using std::list;
using std::map;

class ParticleSystem : public GameEntity {
  public:
    ParticleSystem();
    ~ParticleSystem();
    void init(ThoughtBubble *thought_bubble);
    void addEmitters(int num_particles);
    void update(float delta_time, GameState *state);
    void setTargets(vector<Target> &targets);
    bool targetWasHit(Target target);

  private:
    void addParticles(Emitter &emitter, float delta_time);

    ThoughtBubble *thought_bubble_;
    ParticleDrawInfo *render_data_;
    list<Particle> particles_;
    list<Emitter> emitters_;
    ParticleDrawer drawer_;
    map<int, Emitter *> target_to_emitter_;
    bool targets_;
};

#endif  // SRC_PARTICLES_H_
