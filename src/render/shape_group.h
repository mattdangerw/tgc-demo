#ifndef SRC_SHAPE_GROUP_H_
#define SRC_SHAPE_GROUP_H_

#include <vector>
#include <string>

#include "render/scene_node.h"
#include "render/quad.h"
#include "render/path_shape.h"
#include "render/animator.h"

using std::string;
using std::vector;

class ShapeGroup : public SceneNode { 
  public:
    ShapeGroup();
    ~ShapeGroup();
    void init(string filename);
  private:
    vector<Quad *> fills_;
    vector<PathShape *> shapes_;
    Animator animator_;
};

#endif  // SRC_SHAPE_GROUP_H_
