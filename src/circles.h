#ifndef SRC_CIRCLES_H_
#define SRC_CIRCLES_H_

#include <GL/glew.h>
#include <vector>

#include "shader_program.h"
#include "renderer.h"

struct Circle {
  Circle() : color(0.0f, 0.0f, 0.0f, 1.0f), center(), radius(1.0f) {}
  glm::vec4 color;
  glm::vec2 center;
  float radius;
};

class CircleDrawer : public Drawable {
  public:
    CircleDrawer();
    ~CircleDrawer();
    // Set up the VAOs and VBOs and what not.
    void init(vector<Circle> *circles);
    void addScreenSpaceTexture(string texture_filename);
    void changeRadii(float delta_radius) { delta_radius_ = delta_radius; }
    void draw(glm::mat3 view) { drawHelper(view, false); }
    void drawOcclude(glm::mat3 view) { drawHelper(view, true); }

  private:
    void drawHelper(glm::mat3 view, bool asOccluder);
    vector<Circle> *circles_;
    float delta_radius_;
    bool with_texture_;
    // GL stuff
    Program *colored_program_, *textured_program_;
    GLuint array_object_, buffer_object_;
    GLuint texture_handle_;
};

#endif  // SRC_CIRCLES_H_
