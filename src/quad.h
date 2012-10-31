#ifndef SRC_QUAD_H_
#define SRC_QUAD_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

#include "shader_program.h"
#include "renderer.h"

class Quad : public SceneNode {
  public:
    Quad();
    ~Quad();
    void init();
    void getCorners(glm::vec2 *min, glm::vec2 *max);
    void setCorners(glm::vec2 min, glm::vec2 max);
    void useTexture(string texture_file);
    void useColor(glm::vec4 color);
    void setTextureScale(glm::vec2 scale) { tex_scale_ = scale; }
    void setShadowed(bool shadowed) { shadowed_ = shadowed; }
    void setColorMask(glm::vec4 color_mask) { color_mask_ = color_mask; }
    void draw();
    void drawOccluder();
  private:
    bool shadowed_, textured_;
    glm::vec2 vertices_[4], tex_coords_[4];
    glm::vec2 tex_scale_;
    glm::vec4 color_mask_, color_;
    // GL.
    GLuint array_object_;
    GLuint texture_handle_;
    GLuint position_buffer_object_, tex_coord_buffer_object_;
};

#endif  // SRC_QUAD_H_
