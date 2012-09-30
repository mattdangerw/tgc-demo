#ifndef SRC_PARTICLES_H_
#define SRC_PARTICLES_H_

#include <string>
#include <list>
#include <glm.hpp>

#include "thought_bubble.h"
#include "particle.h"
#include "game_entity.h"

using std::string;
using std::list;

class ParticleSystem : public GameEntity {
  public:
    ParticleSystem();
    ~ParticleSystem();
    void init(ThoughtBubble *thought_bubble);
    void addEmitters(int num_particles);
    void update(float delta_time, GameState *state);

  private:
    void addParticles(Emitter &emitter, float delta_time);

    ThoughtBubble *thought_bubble_;
    list<Particle> particles_;
    list<Emitter> emitters_;
    ParticleDrawer drawer_;
};

#endif  // SRC_PARTICLES_H_
