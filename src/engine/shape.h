#ifndef SRC_PATH_SHAPE_H_
#define SRC_PATH_SHAPE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>

#include "engine/fill.h"
#include "engine/shader_program.h"
#include "engine/animator.h"

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
    void extent(glm::vec2 *min, glm::vec2 *max);
    // Solids.
    bool hasSolidVertices() { return has_solids_; }
    size_t solidVerticesSize() { return solids_size_; }
    GLuint solidBufferObject() { return solid_buffer_object_; }
    // Quadrics.
    bool hasQuadricVertices() { return has_quadrics_; }
    size_t quadricVerticesSize() { return quadrics_size_; }
    GLuint quadricBufferObject() { return quadric_buffer_object_; }
    GLuint bezierCoordsBufferObject() { return bezier_coords_buffer_object_; }
    // Cubics.
    bool hasCubicVertices() { return has_cubics_; }
    size_t cubicVerticesSize() { return cubics_size_; }
    GLuint cubicBufferObject() { return cubic_buffer_object_; }
    GLuint cubicExtraBufferObject() { return cubic_extra_buffer_object_; }
  private:
    // Helpers.
    void readVertices(string filename, vector<PathVertex> *vertices);
    void prepVertices(const vector<PathVertex> &vertices, vector<glm::vec2> *solids, vector<glm::vec2> *quadrics,
      vector<glm::vec2> *cubics, vector<glm::vec2> *cubic_extra);
    void makeBezierCoords(const vector<glm::vec2> &quadrics, vector<glm::vec2> *bezier_coords);
    void findCorners(const vector<PathVertex> &vertices);
    // Member data.
    bool has_solids_, has_quadrics_, has_cubics_;
    size_t solids_size_, quadrics_size_, cubics_size_;
    glm::vec2 min_corner_, max_corner_;
    GLuint solid_buffer_object_, quadric_buffer_object_, cubic_buffer_object_, cubic_extra_buffer_object_, bezier_coords_buffer_object_;
};

struct NamedFile {
  string name;
  string file;
};

class Shape : public Entity {
  public:
    Shape();
    ~Shape();
    void init(const vector<PathVertex> &vertices);
    void init(string filename);
    void init(const vector<NamedFile> &frames, Animator *animator);
    void extent(glm::vec2 *min, glm::vec2 *max) { *min = min_; *max = max_; }
    void draw() { drawHelper(false); }
    void drawOccluder() { drawHelper(true); }

  private:
    // Helper methods.
    void initHelper(Fill *fill, glm::vec2 min, glm::vec2 max);
    void createVAOs();
    void drawHelper(bool asOccluder);
    void bindKeyframeBuffers();
    // Member data.
    bool animated_, from_file_;
    ShapeData *data_;
    glm::vec2 min_, max_;
    // Animation stuff.
    map<string, ShapeData *> frames_;
    Animator *animator_;
    float lerp_ts_[2];
    // OpenGL stuff
    GLuint solid_array_object_, quadric_array_object_, cubic_array_object_;
};

#endif  // SRC_PATH_SHAPE_H_
