#include "world/background.h"

#include "world/world.h"

Background::Background() {}

Background::~Background() {}

void Background::init() {
  fill_.init("content/textures/seamlesstexture22.dds");
  fill_.setColorMultiplier(glm::vec4(glm::vec3(0.5f), 1.0f));
  fill_.setColorAddition(glm::vec4(glm::vec3(0.5f), 1.0f));
  fill_.setShadowed(true);
  quad_.setParent(this);
  quad_.setDisplayPriority(-99.0f);
  quad_.setExtent(glm::vec2(0.0f, 0.0f), glm::vec2(theWorld().ground.width(), 1.0f));
  quad_.setIsOccluder(false);
  quad_.setFill(&fill_);
}
