
#include "world/world.h"

#include "engine/engine.h"
#include "engine/entity.h"
#include "util/transform2D.h"

static World the_world;

World &theWorld() {
  return the_world;
}

void World::init() {
  // Someday do all this positioning from a file, but for now we'll just keep all the level set up here.
  vector<glm::vec2> ground_points;
  ground_points.push_back(glm::vec2(0.0f, 0.2f));
  ground_points.push_back(glm::vec2(0.3f, 0.19f));
  ground_points.push_back(glm::vec2(0.38f, 0.15f));
  ground_points.push_back(glm::vec2(0.6f, 0.145f));
  ground_points.push_back(glm::vec2(0.8f, 0.22f));
  ground_points.push_back(glm::vec2(1.2f, 0.17f));
  // Parents
  ground_points.push_back(glm::vec2(1.8f, 0.22f));
  ground_points.push_back(glm::vec2(2.3f, 0.22f));

  // Hill
  ground_points.push_back(glm::vec2(2.7f, 0.16f));
  ground_points.push_back(glm::vec2(3.0f, 0.34f));
  ground_points.push_back(glm::vec2(3.1f, 0.37f));
  ground_points.push_back(glm::vec2(3.25f, 0.345f));
  ground_points.push_back(glm::vec2(3.55f, 0.22f));
  ground_points.push_back(glm::vec2(3.75f, 0.16f));
  
  // First trees
  ground_points.push_back(glm::vec2(4.1f, 0.14f));
  ground_points.push_back(glm::vec2(4.3f, 0.16f));
  ground_points.push_back(glm::vec2(4.5f, 0.145f));
  ground_points.push_back(glm::vec2(4.7f, 0.165f));
  ground_points.push_back(glm::vec2(4.9f, 0.14f));
  ground_points.push_back(glm::vec2(5.1f, 0.15f));

  // Kids
  ground_points.push_back(glm::vec2(5.45f, 0.3f));
  ground_points.push_back(glm::vec2(5.85f, 0.35f));
  ground_points.push_back(glm::vec2(6.2f, 0.3f));

  // Valley
  ground_points.push_back(glm::vec2(6.5f, 0.2f));
  ground_points.push_back(glm::vec2(6.7f, 0.1f));
  ground_points.push_back(glm::vec2(6.8f, 0.09f));
  ground_points.push_back(glm::vec2(7.0f, 0.04f));
  ground_points.push_back(glm::vec2(7.4f, 0.12f));
  ground_points.push_back(glm::vec2(7.8f, 0.14f));
  
  // Mountain
  ground_points.push_back(glm::vec2(8.1f, 0.35f));
  ground_points.push_back(glm::vec2(8.13f, 0.31f));
  ground_points.push_back(glm::vec2(8.4f, 0.6f));
  ground_points.push_back(glm::vec2(8.6f, 0.35f));
  ground_points.push_back(glm::vec2(8.64f, 0.39f));

  // lil valley
  ground_points.push_back(glm::vec2(8.85f, 0.24f));
  ground_points.push_back(glm::vec2(9.15f, 0.16f));
  ground_points.push_back(glm::vec2(9.45f, 0.15f));

  // End
  ground_points.push_back(glm::vec2(9.65f, 0.20f));
  ground_points.push_back(glm::vec2(9.71f, 0.25f));
  ground_points.push_back(glm::vec2(9.77f, 0.25f));
  ground_points.push_back(glm::vec2(9.83f, 0.30f));
  ground_points.push_back(glm::vec2(9.99f, 0.30f));
  ground_points.push_back(glm::vec2(10.05f, 0.25f));
  ground_points.push_back(glm::vec2(10.11f, 0.25f));
  ground_points.push_back(glm::vec2(10.17f, 0.20f));

  theEngine().setLightPosition(glm::vec2(5.0f, 10.0f));

  // Init
  event_manager.init();
  ground.init(ground_points);
  background.init();
  scroller.init();
  cloud_manager.init();
  bird_manager.init();
  character.init();
  test_text.init();
  glm::mat3 transform = translate2D(glm::mat3(1.0f), glm::vec2(0.2f, 0.6f));
  test_text.setRelativeTransform(transform);
  test_text.setText("stringy string bean?");
  test_fill.init("content/textures/bookcover1.dds");
  test_text.setFill(&test_fill);
  test_text.setOccluderColor(0.7f);
  test_shapes.init("test.group");

  // Add to scene graph
  this->setParent(theEngine().rootEntity());
  event_manager.setParent(this);
  ground.setParent(this);
  background.setParent(this);
  scroller.setParent(this);
  cloud_manager.setParent(this);
  bird_manager.setParent(this);
  character.setParent(this);
  test_text.setParent(this);
  test_shapes.setParent(this);

  // Draw order
  background.setDisplayPriority(-1);
  ground.setDisplayPriority(0);
  cloud_manager.setDisplayPriority(1);
  bird_manager.setDisplayPriority(1);
  character.setDisplayPriority(2);
  test_text.setDisplayPriority(0);
  test_shapes.setDisplayPriority(2);
  for(int i = 1; i < 10; ++i) {
    ShapeGroup *s = new ShapeGroup();
    s->init("test.group");
    s->setParent(this);
    s->setDisplayPriority(10);
    glm::mat3 t = translate2D(glm::mat3(1.0f), glm::vec2(0.2f * i, 0.0f));
    s->setRelativeTransform(t);
  }
}