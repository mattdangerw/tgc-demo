#ifndef SRC_PATH_SHAPE_H_
#define SRC_PATH_SHAPE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>

#include "quad.h"
#include "shader_program.h"

using std::string;
using std::vector;
using std::map;

enum PathVertexType {
  ON_PATH = 1,
  QUADRIC = 2,
  CUBIC = 3
};

struct PathVertex {
  glm::vec2 position;
  PathVertexType type;
};

class ShapeData { 
  public:
    ShapeData();
    ~ShapeData();
    void init(string filename);
    void init(const vector<PathVertex> &vertices);
    void corners(glm::vec2 *min, glm::vec2 *max);
    // Solids.
    bool hasSolidVertices() { return has_solids_; }
    size_t solidVerticesSize() { return solids_size_; }
    GLuint solidBufferObject() { return solid_buffer_object_; }
    // Quadrics.
    bool hasQuadricVertices() { return has_quadrics_; }
    size_t quadricVerticesSize() { return quadrics_size_; }
    GLuint quadricBufferObject() { return quadric_buffer_object_; }
    GLuint bezierCoordsBufferObject() { return bezier_coords_buffer_object_; }
  private:
    // Helpers.
    void readVertices(string filename, vector<PathVertex> *vertices);
    void prepVertices(const vector<PathVertex> &vertices, vector<glm::vec2> *solids, vector<glm::vec2> *quadrics);
    void cubicToQuadrics(glm::vec2 start, glm::vec2 control1, glm::vec2 control2, glm::vec2 end, vector<glm::vec2> *solids, vector<glm::vec2> *quadrics);
    void makeBezierCoords(const vector<glm::vec2> &quadrics, vector<glm::vec2> *bezier_coords);
    void findCorners(const vector<PathVertex> &vertices);
    // Member data.
    bool has_solids_, has_quadrics_;
    size_t solids_size_, quadrics_size_;
    glm::vec2 min_corner_, max_corner_;
    GLuint solid_buffer_object_, quadric_buffer_object_, bezier_coords_buffer_object_;
};

class PathShape : public SceneNode {
  public:
    PathShape();
    ~PathShape();
    void init(const vector<PathVertex> &vertices, Quad *fill);
    void init(string filename, Quad *fill);
    void init(const vector<string> &keyframe_files, const vector<float> &keyfram_times, Quad *fill);
    void setOccluderColor(float color);
    float width();
    float height();
    void draw() { drawHelper(false); }
    void drawOccluder() { drawHelper(true); }
    void animate(float delta_time);

  private:
    // Helper methods.
    void initHelper(Quad *fill, glm::vec2 min, glm::vec2 max);
    void createVAOs();
    void drawHelper(bool asOccluder);
    void bindKeyframeBuffers();
    // Member data.
    bool dynamic_, from_file_;
    Quad *fill_;
    ShapeData *data_;
    // Animation stuff.
    float time_;
    vector<float> keyframe_times_;
    int last_keyframe_, next_keyframe_;
    float animation_duration_, keyframes_mix_;
    vector<ShapeData *> keyframes_;
    // OpenGL stuff
    GLuint solid_array_object_, quadric_array_object_;
};

#endif  // SRC_PATH_SHAPE_H_
