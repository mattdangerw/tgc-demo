#ifndef SRC_PATH_SHAPE_H_
#define SRC_PATH_SHAPE_H_

#include <GL/glew.h>
#include <glm.hpp>
#include <string>
#include <vector>

#include "quad.h"
#include "shader_program.h"

using std::string;
using std::vector;

enum PathVertexType {
  ON_PATH = 1,
  QUADRIC = 2,
  CUBIC = 3
};

struct PathVertex {
  glm::vec2 position;
  PathVertexType type;
};

class PathShape : Drawable {
  public:
    PathShape();
    ~PathShape();
    void init(string filename, Quad *fill, Program *minimal_program, Program *quadric_program, bool dynamic, bool fit_fill);
    void init(const vector<PathVertex> &vertices, Quad *fill, Program *minimal_program, Program *quadric_program, bool dynamic, bool fit_fill);
    void fitFillToShape();
    void updateVertexPosition(int i, glm::vec2 position);
    void draw(glm::mat3 transform);
    int displayPriority() { return 0; }

  private:
    // Helper methods.
    void fitFillQuad(const vector<PathVertex> &vertices);
    void createVAOs();
    void cubicToQuadrics(glm::vec2 start, glm::vec2 control1, glm::vec2 control2, glm::vec2 end);
    void pushBezierCoords();
    // Member data.
    Quad *fill_;
    bool dynamic_;
    vector<glm::vec2> solid_vertices_, quadric_vertices_, quadric_bezier_coords_;
    vector<vector<glm::vec2 *> > updates_by_index_;
    // OpenGL stuff
    Program *minimal_program_, *quadric_program_;
    GLuint solid_array_object_, solid_vertex_buffer_;
    GLuint quadric_array_object_, quadric_buffers_[2];
    GLint modelview_handle_minimal_, modelview_handle_quadric_;
};

#endif  // SRC_PATH_SHAPE_H_
