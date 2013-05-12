#ifndef SRC_GROUND_H_
#define SRC_GROUND_H_

#include <glm/glm.hpp>
#include <vector>

#include "engine/path_shape.h"
#include "engine/shape_group.h"
#include "engine/quad.h"

using std::vector;

class Ground : public Entity {
  public:
    Ground();
    ~Ground();
    void init(vector<glm::vec2> points);
    float width();
    float heightAt(float x);
  private:
    void initPathShape();
    vector<glm::vec2> points_;
    // Drawables.
    PathShape shape_;
    Quad quad_;
};

#endif  // SRC_GROUND_H_
