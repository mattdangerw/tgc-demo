#ifndef SRC_CIRCLES_H_
#define SRC_CIRCLES_H_

#include <GL/glew.h>
#include <vector>

#include "shader_program.h"
#include "renderer.h"
#include "quad.h"

struct Circle {
  Circle() : color(0.0f, 0.0f, 0.0f, 1.0f), center(), radius(1.0f) {}
  glm::vec4 color;
  glm::vec2 center;
  float radius;
};

class CircleDrawer : public Drawable2D {
  public:
    CircleDrawer();
    ~CircleDrawer();
    // Set up the VAOs and VBOs and what not.
    void init(vector<Circle> *circles);
    void useScreenSpaceTexture(string texture_filename);
    void useQuad(Quad *quad);
    void changeRadii(float delta_radius) { delta_radius_ = delta_radius; }
    void draw(glm::mat3 view);
    void drawOcclude(glm::mat3 view);
    void setOccluderColor(glm::vec4 color) { occluder_color_ = color; }

  private:
    // Helpers.
    void drawColored(glm::mat3 view);
    void drawWithScreenTexture(glm::mat3 view);
    void drawWithQuad(glm::mat3 view);
    void makeDrawCalls(glm::mat3 view, Program *program, bool sendColors);
    // Member data.
    glm::vec4 occluder_color_;
    vector<Circle> *circles_;
    float delta_radius_;
    bool use_texture_, use_quad_;
    Quad *fill_;
    // GL stuff
    Program *colored_program_, *textured_program_;
    GLuint array_object_, buffer_object_;
    GLuint texture_handle_;
};

#endif  // SRC_CIRCLES_H_
