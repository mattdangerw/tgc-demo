#ifndef SRC_RANDOM_H_
#define SRC_RANDOM_H_

#include <glm/glm.hpp>

inline float randomFloat(const float &min, const float &max) {
  return min + rand()/(RAND_MAX/(max - min));
}

inline int randomInt(const int &min, const int &max) {
  return min + rand() % (max-min);
}

inline glm::vec2 randomDirection() {
  glm::vec2 direction = glm::vec2(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f));
  return glm::normalize(direction);
}

inline glm::vec3 randomDirection3D() {
  glm::vec3 direction = glm::vec3(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f));
  return glm::normalize(direction);
}

inline glm::vec2 fastRandomDirection() {
  return glm::vec2(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f));
}

inline glm::vec3 fastRandomDirection3D() {
  return glm::vec3(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f));
}

inline glm::vec3 randomColor() {
  //float hue = randomFloat(0.0f, 180.0f);
  //float saturation = randomFloat(0.5f, 1.0f);
  //float value = randomFloat(0.5f, 1.0f);
  //glm::vec3 hsv(hue, saturation, value);
  //return glm::rgbColor(hsv);
  return glm::vec3(randomFloat(0.2f, 1.0f), randomFloat(0.2f, 1.0f), randomFloat(0.2f, 1.0f));
}

#endif  // SRC_RANDOM_H_