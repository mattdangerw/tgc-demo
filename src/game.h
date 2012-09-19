#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include <vector>

#include "renderer.h"
#include "ground.h"
#include "character.h"
#include "thought_bubble.h"
#include "particle_system.h"
#include "triggerables.h"
#include "clouds.h"
#include "game_entity.h"

using std::vector;

class Game {
  public:
    Game();
    ~Game();
    // Does all the intialization work.
    void init(int width, int height);
    // Gets game to quit next update.
    void prepareToQuit() { leave_game_ = true; }
    // Plays one frame worth of game.
    void update();
    // Draws the frame.
    void draw();
    // Check if our game is still going.
    bool stillRunning();
    // Handle keyboard input. Expects glfw key and action.
    void handleKeyboardEvent(int key, int action);
    // Change the screen size of our game.
    void resize(int width, int height);

  private:
    GameState state_;
    Renderer renderer_;

    // Last time update was called in seconds since start.
    float last_frame_time_;

    // Key state tracking
    bool leave_game_;
    // Track left and right cursor keys.
    bool left_down_, right_down_;
    // Space bar was just pressed.
    bool space_pressed_;

    // Game entities
    Ground ground_;
    Character character_;
    ThoughtBubble thought_bubble_;
    ParticleSystem particle_system_;
    CloudManager cloud_manager_;
    TriggerableManager triggerable_manager_;
};

#endif  // SRC_GAME_H_
