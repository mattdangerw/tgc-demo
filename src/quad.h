#ifndef SRC_QUAD_H_
#define SRC_QUAD_H_

#include <GL/glew.h>
#include <glm.hpp>
#include <string>

#include "shader_program.h"
#include "renderer.h"

class Quad : public Drawable {
  public:
    Quad();
    virtual void setCorners(glm::vec2 min, glm::vec2 max);
    virtual void draw(glm::mat3 transform) = 0;
    void getCorners(glm::vec2 *min, glm::vec2 *max);
    void setTransform(const glm::mat3 &transform) { transform_ = transform; }
    void xExtent(float *x_begin, float *x_end);
  protected:
    void initPositionBuffer();
    glm::mat3 transform_;
    glm::vec2 vertices_[4];
    // GL
    Program *program_;
    GLuint array_object_, buffer_object_;
    GLint modelview_handle_;
};

class TexturedQuad : public Quad {
  public:
    TexturedQuad();
    ~TexturedQuad();
    void init(string texture_file);
    void draw(glm::mat3 transform);

  private:
    glm::vec2 tex_coords_[4];
    // GL stuff
    GLuint texture_handle_;
    GLuint texture_buffer_;
};

// For our paper textures.
class TiledTexturedQuad : public Quad {
  public:
    TiledTexturedQuad();
    ~TiledTexturedQuad();
    void setTextureScale(glm::vec2 scale) { tex_scale_ = scale; }
    void setCorners(glm::vec2 min, glm::vec2 max);
    void setColorMask(glm::vec4 color_mask) { color_mask_ = color_mask; }
    void init(string texture_file);
    void draw(glm::mat3 transform);

  private:
    glm::vec2 tex_coords_[4];
    glm::vec2 tex_scale_;
    glm::vec4 color_mask_;
    // GL stuff
    GLuint texture_handle_, texture_buffer_;
    GLint mask_handle_;
};

class ColoredQuad : public Quad {
  public:
    ColoredQuad();
    ~ColoredQuad();
    void init();
    void setColor(glm::vec4 color) { color_ = color; }
    void draw(glm::mat3 transform);

  private:
    glm::vec4 color_;
    // GL stuff
    GLint color_handle_;
};

#endif  // SRC_QUAD_H_
