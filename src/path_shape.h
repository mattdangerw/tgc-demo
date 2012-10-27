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

class PathShape : public SceneNode {
  public:
    PathShape();
    ~PathShape();
    void init(const vector<PathVertex> &vertices, Quad *fill);
    void init(string filename, Quad *fill);
    void init(vector<string> keyframe_files, vector<float> keyfram_times, Quad *fill);
    void setOccluderColor(glm::vec4 color);
    float width();
    float height();
    void draw() { drawHelper(false); }
    void drawOccluder() { drawHelper(true); }
    void animate(float delta_time);

  private:
    // Helper methods.
    void initHelper(Quad *fill, glm::vec2 min, glm::vec2 max);
    void readVertices(string filename, vector<PathVertex> &vertices);
    void prepVertices(const vector<PathVertex> &vertices, vector<glm::vec2> &solids, vector<glm::vec2> &quadrics);
    void corners(const vector<PathVertex> &vertices, glm::vec2 *min, glm::vec2 *max);
    void makeBezierTexCoords(vector<glm::vec2> &bezier_tex_coords);
    void createVAOs();
    void cubicToQuadrics(glm::vec2 start, glm::vec2 control1, glm::vec2 control2, glm::vec2 end, vector<glm::vec2> &solids, vector<glm::vec2> &quadrics);
    void drawHelper(bool asOccluder);
    // Member data.
    Quad *fill_;
    vector<glm::vec2> solid_vertices_, quadric_vertices_;

    bool dynamic_;
    float time_;
    vector<float> keyframe_times_;
    int last_keyframe_, next_keyframe_;
    float animation_duration_, keyframes_mix_;
    vector<vector<glm::vec2> > solid_keys_, quadric_keys_;

    // OpenGL stuff
    GLuint solid_array_object_, solid_vertex_buffer_;
    GLuint quadric_array_object_, quadric_buffers_[2];
};

#endif  // SRC_PATH_SHAPE_H_
