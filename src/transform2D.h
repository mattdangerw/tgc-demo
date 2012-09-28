#ifndef SRC_TRANSFORM2D_H_
#define SRC_TRANSFORM2D_H_

#include <glm/glm.hpp>

inline glm::mat3 translate2D(glm::mat3 transform, glm::vec2 shift) {
  glm::mat3 result(transform);
  result[2] = transform[0] * shift[0] + transform[1] * shift[1] + transform[2];
  return result;
}

inline glm::mat3 scale2D(glm::mat3 transform, glm::vec2 scale) {
  glm::mat3 result(1.0f);
  result[0] = transform[0] * scale[0];
  result[1] = transform[1] * scale[1];
  result[2] = transform[2];
  return result;
}

inline glm::mat3 rotate2D(glm::mat3 transform, float angle) {
  float a = glm::radians(angle);
  float c = glm::cos(a);
  float s = glm::sin(a);

  glm::mat3 rotate(glm::mat3::null);
  rotate[0][0] = c;
  rotate[0][1] = s;
  rotate[1][0] = -s;
  rotate[1][1] = c;

  glm::mat3 result(glm::mat3::null);
  result[0] = transform[0] * rotate[0][0] + transform[1] * rotate[0][1];
  result[1] = transform[0] * rotate[1][0] + transform[1] * rotate[1][1];
  result[2] = transform[2];
  return result;
}

#endif  // SRC_TRANSFORM2D_H_
