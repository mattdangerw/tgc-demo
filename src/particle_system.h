#ifndef SRC_PARTICLES_H_
#define SRC_PARTICLES_H_

#include <string>
#include <vector>

#include "glm.hpp"
#include "renderer.h"
#include "thought_bubble.h"
#include "particle.h"

using std::string;
using std::vector;

class ParticleSystem : Drawable {
  public:
    ParticleSystem();
    ~ParticleSystem();
    void init(Renderer *renderer, ThoughtBubble *thought_bubble);
    void addParticles(int num_particles);
    void update(float delta_time);
    void draw();
    string shadingGrounp() { return "default"; }

  private:
    Renderer *renderer_;
    ThoughtBubble *thought_bubble_;
    vector<Particle> particles_;
};

#endif  // SRC_PARTICLES_H_
