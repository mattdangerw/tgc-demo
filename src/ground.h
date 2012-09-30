#ifndef SRC_GROUND_H_
#define SRC_GROUND_H_

#include <glm.hpp>
#include <vector>

#include "path_shape.h"
#include "quad.h"
#include "tree.h"

using std::vector;

class Ground {
  public:
    Ground();
    ~Ground();
    void init();
    float width();
    float heightAt(float x);
  private:
    void initPathShape();
    vector<glm::vec2> points_;
    // Drawables.
    PathShape shape_;
    TiledTexturedQuad quad_;
    TiledTexturedQuad background_;
    Tree tree_;
};

#endif  // SRC_GROUND_H_
