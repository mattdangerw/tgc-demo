#include "game/background.h"

#include "game/state.h"

Background::Background() {}

Background::~Background() {}

void Background::init() {
  quad_.init("content/textures/seamlesstexture22.dds", glm::vec2(1.0f));
  quad_.setParent(theEngine().rootNode());
  quad_.setColorMultiplier(glm::vec4(glm::vec3(0.5f), 1.0f));
  quad_.setColorAddition(glm::vec4(glm::vec3(0.5f), 1.0f));
  quad_.setDisplayPriority(-99.0f);
  quad_.setExtent(glm::vec2(0.0f, 0.0f), glm::vec2(theState().ground.width(), 1.0f));
  quad_.setIsOccluder(false);
  quad_.setShadowed(true);
}
