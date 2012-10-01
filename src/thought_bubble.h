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

class ThoughtBubble : public GameEntity {
  public:
    ThoughtBubble();
    ~ThoughtBubble();
    void init(Character *character);
    glm::vec2 center() { return position_; }
    glm::vec2 velocity() { return bubble_mass_.velocity(); }
    void update(float delta_time, GameState *state);
    void collideEmitter(Emitter &emitter, glm::vec2 old_position);
    void stopDrawing();

  private:
    // Helper methods
    glm::vec2 anchorPoint();

    // Memeber data
    glm::vec2 position_;
    Character *character_;
    PointMass bubble_mass_;
    vector<Circle> bubble_circles_, sub_bubble_circles_;
    vector<float> rest_radii_;
    vector<PointMass> stretch_masses_;
    CircleDrawer circle_drawer_, sub_circle_drawer_;
    float circles_spring_constant_;

    // For pre explosion animation
    bool ready_to_animate_, in_position_;
    float time_to_destination_, time_in_flight_;
    glm::vec2 start_, midway_, end_;
    vector<glm::vec2> circle_starts_;
    float time_to_explode_, time_in_position_;
};

#endif  // SRC_THOUGHT_BUBBLE_H_
