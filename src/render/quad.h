#ifndef SRC_QUAD_H_
#define SRC_QUAD_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

#include "render/shader_program.h"
#include "render/scene_node.h"
#include "render/renderer.h"

class Quad : public SceneNode {
  public:
    Quad();
    ~Quad();
    void init(glm::vec4 color);
    void init(string texture_file, glm::vec2 texture_scale);
    void extent(glm::vec2 *min, glm::vec2 *max);
    void setExtent(glm::vec2 min, glm::vec2 max);
    void useTexture(string texture_file);
    void setTextureScale(glm::vec2 scale) { texture_scale_ = scale; }
    void useColor(glm::vec4 color);
    void setShadowed(bool shadowed) { shadowed_ = shadowed; }
    void setColorMask(glm::vec4 color_mask) { color_mask_ = color_mask; }
    void draw();
    void drawOccluder();
  private:
    // Helpers.
    void initHelper();
    // Member data.
    bool shadowed_, textured_;
    glm::vec2 vertices_[4], tex_coords_[4];
    glm::vec2 texture_scale_;
    glm::vec4 color_mask_, color_;
    // GL.
    GLuint array_object_;
    GLuint texture_handle_;
    GLuint position_buffer_object_, tex_coord_buffer_object_;
};

#endif  // SRC_QUAD_H_
