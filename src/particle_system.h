#ifndef SRC_PARTICLES_H_
#define SRC_PARTICLES_H_

#include <string>
#include <vector>
#include <glm.hpp>

#include "renderer.h"
#include "thought_bubble.h"
#include "particle.h"
#include "game_entity.h"

using std::string;
using std::vector;

class ParticleSystem : GameEntity, Drawable {
  public:
    ParticleSystem();
    ~ParticleSystem();
    void init(Renderer *renderer, ThoughtBubble *thought_bubble);
    void addParticles(int num_particles);
    void update(float delta_time, GameState *state);
    void draw();
    string shadingGrounp() { return "default"; }

  private:
    Renderer *renderer_;
    ThoughtBubble *thought_bubble_;
    vector<Particle> particles_;
};

#endif  // SRC_PARTICLES_H_
