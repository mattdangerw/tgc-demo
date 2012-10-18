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
    void init(string filename, Quad *fill);
    void init(string filename1, string filename2, Quad *fill);
    void init(const vector<PathVertex> &vertices, Quad *fill);
    void setOccluderColor(glm::vec4 color);
    float width();
    float height();
    void draw(glm::mat3 view) { drawHelper(view, false); }
    void drawOcclude(glm::mat3 view) { drawHelper(view, true); }
    void addTime(float delta_time) { time_+=delta_time; }

  private:
    // Helper methods.
    void readVertices(string filename, vector<PathVertex> &vertices);
    void prepVertices(const vector<PathVertex> &vertices, vector<glm::vec2> &solids, vector<glm::vec2> &quadrics);
    void finalizeInit(Quad *fill);
    void fitFillQuad(const vector<PathVertex> &vertices);
    void makeBezierTexCoords(vector<glm::vec2> &bezier_tex_coords);
    void createVAOs();
    void cubicToQuadrics(glm::vec2 start, glm::vec2 control1, glm::vec2 control2, glm::vec2 end, vector<glm::vec2> &solids, vector<glm::vec2> &quadrics);
    void drawHelper(glm::mat3 view, bool asOccluder);
    // Member data.
    Quad *fill_;
    bool dynamic_;
    float time_;
    vector<glm::vec2> solid_vertices_, quadric_vertices_;
    vector<glm::vec2> solid_vertices1_, quadric_vertices1_;
    vector<glm::vec2> solid_vertices2_, quadric_vertices2_;
    // OpenGL stuff
    Program *minimal_program_, *quadric_program_;
    GLuint solid_array_object_, solid_vertex_buffer_;
    GLuint quadric_array_object_, quadric_buffers_[2];
};

#endif  // SRC_PATH_SHAPE_H_
