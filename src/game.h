#ifndef SRC_GAME_H_
#define SRC_GAME_H_

#include "renderer.h"
#include "ground.h"

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
    Renderer renderer;
    bool leave_game;
    // Track left and right cursor keys.
    bool left_down, right_down;
    // Space bar was just pressed.
    bool space_pressed;
    // Game entities
    Ground ground;
};

#endif  // SRC_GAME_H_
