#ifndef WORLD_H_
#define WORLD_H_

#include "world/event_manager.h"
#include "world/background.h"
#include "world/ground.h"
#include "world/clouds.h"
#include "world/birds.h"
#include "world/character.h"
#include "world/scroller.h"
#include "engine/text.h"

class World;

// A big fat global handle into everything game state! 
World &theWorld();

class World : public Entity {
  public:
    void init();
    EventManager event_manager;
    Ground ground;
    Background background;
    Scroller scroller;
    Character character;
    CloudManager cloud_manager;
    BirdManager bird_manager;
    Text test_text;
    TexturedFill test_fill;
    ShapeGroup fern;
    //IdeaManager idea_manager;
    //TreeManager tree_manager;
    //FrogManager frog_manager;
    //StaticStuff static_stuff;
};

#endif  // WORLD_H_