#ifndef SRC_CIRCLES_H_
#define SRC_CIRCLES_H_

#include <GL/glew.h>
#include <vector>

#include "shader_program.h"
#include "renderer.h"

struct Circle {
  glm::vec2 center;
  float radius;
};

class CircleDrawer : public Drawable {
  public:
    CircleDrawer();
    ~CircleDrawer();
    // Set up the VAOs and VBOs and what not.
    void init(vector<Circle> *circles);
    void setColor(const glm::vec4 &color) { color_ = color; }
    void draw(glm::mat3 transform);

  private:
    vector<Circle> *circles_;
    glm::vec4 color_;
    // GL stuff
    Program *program_;
    GLuint array_object_, buffer_objects_[2];
    GLint modelview_handle_, color_handle_;
};

#endif  // SRC_CIRCLES_H_
