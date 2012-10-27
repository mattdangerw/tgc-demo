#ifndef SRC_QUAD_H_
#define SRC_QUAD_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

#include "shader_program.h"
#include "renderer.h"

class Quad : public Drawable2D {
  public:
    Quad();
    virtual void init();
    virtual void setCorners(glm::vec2 min, glm::vec2 max);
    virtual void draw();
    void setOccluderColor(glm::vec4 color) { occluder_color_ = color; }
    void drawOccluder();
    void getCorners(glm::vec2 *min, glm::vec2 *max);
    void xExtent(float *x_begin, float *x_end);
  protected:
    GLuint array_object_;
  private:
    glm::vec2 vertices_[4];
    glm::vec4 occluder_color_;
    // GL.
    Program *program_;
    GLuint buffer_object_;
};

// For our paper textures.
class TexturedQuad : public Quad {
  public:
    TexturedQuad();
    ~TexturedQuad();
    void init(string texture_file);
    void setShadowed(bool shadowed) { shadowed_ = shadowed; }
    void setTextureScale(glm::vec2 scale) { tex_scale_ = scale; }
    void setColorMask(glm::vec4 color_mask) { color_mask_ = color_mask; }
    void setCorners(glm::vec2 min, glm::vec2 max);
    void draw();

  private:
    bool shadowed_;
    glm::vec2 tex_coords_[4];
    glm::vec2 tex_scale_;
    glm::vec4 color_mask_;
    // GL stuff.
    Program *textured_program_, *shadowed_program_;
    GLuint texture_handle_, texture_buffer_;
};

class ColoredQuad : public Quad {
  public:
    ColoredQuad();
    ~ColoredQuad();
    void init();
    void setColor(glm::vec4 color) { color_ = color; }
    void draw();

  private:
    glm::vec4 color_;
    // GL stuff
    Program *program_;
    GLint color_handle_;
};

#endif  // SRC_QUAD_H_
