#ifndef SRC_PATH_SHAPE_H_
#define SRC_PATH_SHAPE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
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

class PathShape : public Drawable {
  public:
    PathShape();
    ~PathShape();
    void init(string filename, Quad *fill, bool fit_fill, bool dynamic);
    void init(const vector<PathVertex> &vertices, Quad *fill, bool fit_fill, bool dynamic);
    void setOccluderColor(glm::vec4 color);
    float width();
    float height();
    void updateVertexPosition(int i, glm::vec2 position);
    void draw(glm::mat3 view) { drawHelper(view, false); }
    void drawOcclude(glm::mat3 view) { drawHelper(view, true); }

  private:
    // Helper methods.
    void fitFillQuad(const vector<PathVertex> &vertices);
    void createVAOs();
    void cubicToQuadrics(glm::vec2 start, glm::vec2 control1, glm::vec2 control2, glm::vec2 end);
    void pushBezierCoords();
    void drawHelper(glm::mat3 view, bool asOccluder);
    // Member data.
    Quad *fill_;
    bool dynamic_;
    vector<glm::vec2> solid_vertices_, quadric_vertices_, quadric_bezier_coords_;
    vector<vector<glm::vec2 *> > updates_by_index_;
    // OpenGL stuff
    Program *minimal_program_, *quadric_program_;
    GLuint solid_array_object_, solid_vertex_buffer_;
    GLuint quadric_array_object_, quadric_buffers_[2];
};

#endif  // SRC_PATH_SHAPE_H_
