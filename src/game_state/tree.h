#ifndef SRC_TREE_H_
#define SRC_TREE_H_

#include "render/path_shape.h"
#include "render/quad.h"

class Tree : public SceneNode {
  public:
    Tree();
    ~Tree();
    void init();
    void setColor(glm::vec4 color);

  private:
    Quad trunk_quad_, leaves_quad_;
    PathShape trunk_shape_, leaves_shape_;
};

#endif  // SRC_TREE_H_
