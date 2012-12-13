#ifndef STATE_H_
#define STATE_H_

#include "game_state/updater.h"
#include "game_state/clouds.h"

class State;

// A big fat global handle into everything game state! 
State &theState();

class State {
  public:
    void init();
    Updater updater;
    //Character character;
    //EventManager event_manager;
    CloudManager cloud_manager;
    //ParticleManager particle_manager;
    //TreeManager tree_manager;
    //FrogManager frog_manager;
    //StaticStuff static_stuff;
};

#endif  // STATE_H_