#include "game_state/state.h"

State the_state;

State &theState() {
  return the_state;
}

void State::init() {
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

  theRenderer().setLightPosition(glm::vec2(5.0f, 10.0f));

  ground.init(ground_points);
  background.init();
  scroller.init();
  cloud_manager.init();
  character.init();
}