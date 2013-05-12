#ifndef STATE_H_
#define STATE_H_

#include "engine/updater.h"
#include "game/event_manager.h"
#include "game/background.h"
#include "game/ground.h"
#include "game/clouds.h"
#include "game/birds.h"
#include "game/character.h"
#include "game/scroller.h"

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