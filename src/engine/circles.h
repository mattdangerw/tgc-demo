#ifndef SRC_CIRCLES_H_
#define SRC_CIRCLES_H_

#include <GL/glew.h>
#include <vector>

#include "engine/shader_program.h"
#include "engine/entity.h"
#include "engine/engine.h"
#include "engine/quad.h"

struct Circle {
  Circle() : color(0.0f, 0.0f, 0.0f, 1.0f), center(), radius(1.0f) {}
  glm::vec4 color;
  glm::vec2 center;
  float radius;
};

class CircleDrawer : public Entity {
  public:
    CircleDrawer();
    ~CircleDrawer();
    // Set up the VAOs and VBOs and what not.
    void init(vector<Circle> *circles);
    void extent(glm::vec2 *min, glm::vec2 *max);  
    void useScreenSpaceTexture(string texture_filename);
    void useQuad(Quad *quad);
    void changeRadii(float delta_radius) { delta_radius_ = delta_radius; }
    void draw();
    void drawOccluder();

  private:
    // Helpers.
    void drawColored();
    void drawWithScreenTexture();
    void drawWithQuad();
    void makeDrawCalls(bool sendColors);
    // Member data.
    vector<Circle> *circles_;
    float delta_radius_;
    bool use_texture_, use_quad_;
    Quad *fill_;
    // GL stuff
    GLuint array_object_, buffer_object_;
    GLuint texture_handle_;
};

#endif  // SRC_CIRCLES_H_
