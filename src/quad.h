#ifndef SRC_QUAD_H_
#define SRC_QUAD_H_

#include <GL/glew.h>
#include <glm.hpp>
#include <string>

#include "shader_program.h"
#include "renderer.h"

using std::string;

class Quad : Drawable {
  public:
    Quad();
    ~Quad();
    void init(Program *program, string texture_file);
    void Quad::setCorners(glm::vec2 min, glm::vec2 max);
    void Quad::getCorners(glm::vec2 *min, glm::vec2 *max);
    void draw(glm::mat3 transform);
    int displayPriority() { return 0; }

  private:
    glm::vec2 vertices_[4], tex_coords_[4];
    // GL stuff
    Program *program_;
    GLuint texture_handle_;
    GLint modelview_handle_;
    GLuint array_object_, buffers_[2];
};

#endif  // SRC_QUAD_H_
