#ifndef SRC_PARTICLES_H_
#define SRC_PARTICLES_H_

#include <string>
#include <vector>
#include <glm.hpp>

#include "thought_bubble.h"
#include "particle.h"
#include "game_entity.h"

using std::string;
using std::vector;

class ParticleSystem : GameEntity {
  public:
    ParticleSystem();
    ~ParticleSystem();
    void init(ThoughtBubble *thought_bubble);
    void addParticles(int num_particles);
    void update(float delta_time, GameState *state);

  private:
    ThoughtBubble *thought_bubble_;
    vector<Particle> particles_;
    ParticleDrawer drawer_;
};

#endif  // SRC_PARTICLES_H_
