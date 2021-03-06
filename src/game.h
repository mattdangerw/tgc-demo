#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include <vector>

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
    // Last time update was called in seconds since start.
    float last_frame_time_;

    // Key state tracking
    bool leave_game_;
    // Track left and right cursor keys.
    bool left_down_, right_down_;
    // Space bar was just pressed.
    bool space_pressed_;
};

#endif  // SRC_GAME_H_
