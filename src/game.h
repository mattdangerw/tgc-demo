#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include <vector>

#include "renderer.h"
#include "ground.h"
#include "character.h"
#include "thought_bubble.h"
#include "particle_system.h"
#include "triggerable.h"
#include "clouds.h"

using std::vector;

class Game {
  public:
    Game();
    ~Game();
    // Does all the intialization work.
    void init(int width, int height);
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
    // Helper functions.
    void initTriggerables();
    void updateTriggerables(float delta_time);

    // Member data
    enum GameState {
      WALKING,
      TRIGGERING,
      EXPLODING
    };
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
    CloudManager cloud_manager_;
    Character character_;
    ThoughtBubble thought_bubble_;
    ParticleSystem particle_system_;
    vector<Triggerable *> triggerables_;
    unsigned int current_triggerable_;
};

#endif  // SRC_GAME_H_
