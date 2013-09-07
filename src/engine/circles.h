#ifndef SRC_CIRCLES_H_
#define SRC_CIRCLES_H_

#include <GL/glew.h>
#include <vector>

#include "engine/shader_program.h"
#include "engine/entity.h"
#include "engine/engine.h"
#include "engine/fill.h"

class Circle : public Entity {
  public:
    Circle();
    ~Circle();
    // Set up the VAOs and VBOs and what not.
    void init() {}
    void extent(glm::vec2 *min, glm::vec2 *max);
    glm::vec2 center() { return center_; }
    void setCenter(glm::vec2 center) { center_ = center; }
    float radius() { return radius_; }
    void setRadius(float radius) { radius_ = radius; }
    void draw();
    void drawOccluder();

  private:
    void drawHelper(bool occluder);
    // Member data.
    glm::vec2 center_;
    float radius_;
    // GL stuff
    GLuint array_object_, buffer_object_;
    GLuint texture_handle_;
};

#endif  // SRC_CIRCLES_H_
