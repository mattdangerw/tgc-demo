#include "game_state/background.h"

#include "game_state/state.h"

Background::Background() {}

Background::~Background() {}

void Background::init() {
  quad_.init("content/textures/seamlesstexture22.dds", glm::vec2(1.0f));
  quad_.setParent(theRenderer().rootNode());
  quad_.setDisplayPriority(-99.0f);
  quad_.setExtent(glm::vec2(0.0f, 0.0f), glm::vec2(theState().ground.width(), 1.0f));
  quad_.setIsOccluder(false);
  quad_.setShadowed(true);
}
