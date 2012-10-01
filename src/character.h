#ifndef SRC_CHARACTER_H_
#define SRC_CHARACTER_H_

#include <string>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "ground.h"
#include "game_entity.h"
#include "path_shape.h"
#include "quad.h"
#include "circles.h"

using std::string;

class Character : public GameEntity {
  public:
    Character();
    ~Character();
    void init(Ground *ground);
    // Record keyboard input for the frame.
    void setInput(bool left_down, bool right_down, bool space_pressed);
    // Update without character movement
    void update(float delta_time, GameState *state);
    // Get the character position.
    glm::vec2 position() { return position_; }
    glm::vec2 groundPosition();
    void getTargets(vector<Target> &targets);
    void colorTarget(Target target);

  private:
    // Helper functions.
    void updateCircle();
    void moveLeft(float delta_time);
    void moveRight(float delta_time);
    void jump();
    // Updates Y coord based up jumping state and ground height.
    void updateY(float delta_time);
    // Member data.
    Ground *ground_;
    bool left_down_, right_down_, space_pressed_;
    glm::vec2 position_;
    bool is_jumping_;
    float jump_velocity_, time_on_ground_, time_till_next_jump_;
    vector<Circle> circle_vector_;
    Circle *circle_;
    CircleDrawer drawer_;
};

#endif  // SRC_CHARACTER_H_
