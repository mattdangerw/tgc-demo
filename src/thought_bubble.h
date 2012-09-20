#ifndef SRC_THOUGHT_BUBBLE_H_
#define SRC_THOUGHT_BUBBLE_H_

#include <string>
#include <vector>

#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "renderer.h"
#include "character.h"
#include "point_mass.h"
#include "particle.h"
#include "game_entity.h"

using std::string;
using std::vector;

struct Circle {
  glm::vec2 position;
  float radius;
};

class ThoughtBubble : GameEntity, Drawable {
  public:
    ThoughtBubble();
    ~ThoughtBubble();
    void init(Renderer *renderer, Character *character);
    glm::vec2 center() { return mass_.position(); }
    glm::vec2 velocity() { return mass_.velocity(); }
    void update(float delta_time, GameState *state);
    void collideParticle(Particle &particle, glm::vec2 old_position);
    void draw();
    string shadingGrounp() { return "default"; }

  private:
    // Helper methods
    glm::vec2 anchorPoint();

    // Memeber data
    Renderer *renderer_;
    Character *character_;
    PointMass mass_;
    vector<Circle> circles_;
};

#endif  // SRC_THOUGHT_BUBBLE_H_
