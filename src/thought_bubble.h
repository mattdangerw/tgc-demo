#ifndef SRC_THOUGHT_BUBBLE_H_
#define SRC_THOUGHT_BUBBLE_H_

#include <vector>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "character.h"
#include "point_mass.h"
#include "particle.h"
#include "game_entity.h"
#include "circles.h"

using std::vector;

class ThoughtBubble : GameEntity {
  public:
    ThoughtBubble();
    ~ThoughtBubble();
    void init(Character *character);
    glm::vec2 center() { return mass_.position(); }
    glm::vec2 velocity() { return mass_.velocity(); }
    void update(float delta_time, GameState *state);
    void collideParticle(Particle &particle, glm::vec2 old_position);

  private:
    // Helper methods
    glm::vec2 anchorPoint();

    // Memeber data
    Character *character_;
    PointMass mass_;
    vector<Circle> circles_;
    vector<float> rest_radii_;
    vector<PointMass> stretch_masses_;
    // For drawing. TODO name CircleCluster better.
    CircleDrawer circle_drawer_;
};

#endif  // SRC_THOUGHT_BUBBLE_H_
