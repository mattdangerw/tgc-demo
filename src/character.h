#ifndef SRC_CHARACTER_H_
#define SRC_CHARACTER_H_

#include <string>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "renderer.h"
#include "ground.h"
#include "game_entity.h"

using std::string;

class Character : GameEntity, Drawable {
  public:
    Character();
    ~Character();
    void init(Renderer *renderer, Ground *ground);
    // Record keyboard input for the frame.
    void setInput(bool left_down, bool right_down, bool space_pressed);
    // Update without character movement
    void update(float delta_time, GameState *state);
    // Get the character position.
    glm::vec2 position() { return position_; }
    void draw();
    string shadingGrounp() { return "default"; }

  private:
    // Helper functions.
    void moveLeft(float delta_time);
    void moveRight(float delta_time);
    void jump();
    // Updates Y coord based up jumping state and ground height.
    void updateY(float delta_time);
    // Member data.
    Renderer *renderer_;
    Ground *ground_;
    bool left_down_, right_down_, space_pressed_;
    glm::vec2 position_;
    bool is_jumping_;
    float jump_velocity_;
};

#endif  // SRC_CHARACTER_H_
