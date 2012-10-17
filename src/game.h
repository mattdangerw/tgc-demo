#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include <vector>

#include "ground.h"
#include "character.h"
#include "thought_bubble.h"
#include "particle_system.h"
#include "triggerables.h"
#include "clouds.h"
#include "game_entity.h"
#include "jumping_crowd.h"

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

  private:
    GameState state_, last_state_;

    // Last time update was called in seconds since start.
    float last_frame_time_, time_in_ending_, time_in_exploding_;

    // Key state tracking
    bool leave_game_;
    // Track left and right cursor keys.
    bool left_down_, right_down_;
    // Space bar was just pressed.
    bool space_pressed_;

    vector<Target> targets_;
    
    // Game entities
    vector<GameEntity *> entities_;
    Ground ground_;
    Character character_;
    ThoughtBubble thought_bubble_;
    ParticleSystem particle_system_;
    CloudManager cloud_manager_;
    TriggerableManager triggerable_manager_;
    JumpingCrowd crowds_[2];
};

#endif  // SRC_GAME_H_
