#ifndef SRC_THOUGHT_BUBBLE_H_
#define SRC_THOUGHT_BUBBLE_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "character.h"
#include "point_mass.h"
#include "particle.h"
#include "game_entity.h"
#include "circles.h"
#include "quad.h"

using std::vector;

class SubBubble: public Drawable2D  {
  public:
    SubBubble() {}
    ~SubBubble() {}
    void init(vector<Circle> *circles, float texture_scale, float darkness);
    void draw() {
      outer_drawer_.draw();
      //inner_drawer_.draw(view);
    }
    void drawOcclude(glm::mat3 view) {}
  private:
    CircleDrawer outer_drawer_;//, inner_drawer_;
    TexturedQuad outer_fill_;//, inner_fill_;
};

class ThoughtBubble : public GameEntity {
  public:
    ThoughtBubble();
    ~ThoughtBubble();
    void init(Character *character);
    glm::vec2 center() { return position_; }
    glm::vec2 velocity() { return bubble_mass_.velocity(); }
    void update(float delta_time, GameState *state);
    bool collideEmitter(glm::vec2 *position, glm::vec2 *velocity);
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
    TexturedQuad fill_;
    SubBubble sub_bubble_, sub_bubble2_, sub_bubble3_;

    // For pre explosion animation
    bool ready_to_animate_, in_position_;
    float time_to_destination_, time_in_flight_;
    glm::vec2 start_, midway_, end_;
    vector<glm::vec2> circle_starts_;
    float time_to_explode_, time_in_position_;
};

#endif  // SRC_THOUGHT_BUBBLE_H_
