#ifndef STATE_H_
#define STATE_H_

#include "game_state/updater.h"
#include "game_state/event_manager.h"
#include "game_state/background.h"
#include "game_state/ground.h"
#include "game_state/clouds.h"
#include "game_state/birds.h"
#include "game_state/character.h"
#include "game_state/scroller.h"

class State;

// A big fat global handle into everything game state! 
State &theState();

class State {
  public:
    void init();
    Updater updater;
    EventManager event_manager;
    Ground ground;
    Background background;
    Scroller scroller;
    Character character;
    CloudManager cloud_manager;
    BirdManager bird_manager;
    //IdeaManager idea_manager;
    //TreeManager tree_manager;
    //FrogManager frog_manager;
    //StaticStuff static_stuff;
};

#endif  // STATE_H_