#ifndef SRC_THOUGHT_BUBBLE_H_
#define SRC_THOUGHT_BUBBLE_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "world/character.h"
#include "world/point_mass.h"
#include "world/game_entity.h"
#include "engine/entity.h"
#include "engine/circles.h"
#include "engine/quad.h"

using std::vector;

class SubBubble: public Entity  {
  public:
    SubBubble() {}
    ~SubBubble() {}
    void init(vector<Circle> *circles, float texture_scale, float darkness);
  private:
    CircleDrawer outer_drawer_;
    Quad outer_fill_;
};

class ThoughtBubble : public GameEntity {
  public:
    ThoughtBubble();
    ~ThoughtBubble();
    void init(Character *character);
    glm::vec2 center() { return position_; }
    glm::vec2 velocity() { return bubble_mass_.velocity(); }
    void update(float delta_time, GameState *state);
    // Returns true and updates position and velocity if there is a collision.
    bool collideIdea(glm::vec2 *position, glm::vec2 *velocity);
    void shrink(float scale);
    void stopDrawing();

  private:
    // Helper methods
    glm::vec2 anchorPoint();

    // Memeber data
    glm::vec2 position_;
    Character *character_;
    PointMass bubble_mass_;
    vector<Circle> bubble_circles_, sub_bubble_circles_;
    float circles_spring_constant_;
    vector<float> rest_radii_;
    vector<PointMass> stretch_masses_;
    CircleDrawer circle_drawer_, circle_inside_drawer_;
    Quad fill_;
    SubBubble sub_bubble_, sub_bubble2_, sub_bubble3_;

    // For pre explosion animation
    bool ready_to_animate_, in_position_;
    float time_to_destination_, time_in_flight_;
    glm::vec2 start_, midway_, end_;
    vector<glm::vec2> circle_starts_;
    float time_to_explode_, time_in_position_;
};

#endif  // SRC_THOUGHT_BUBBLE_H_
