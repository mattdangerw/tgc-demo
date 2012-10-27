#ifndef SRC_TREE_H_
#define SRC_TREE_H_

#include "path_shape.h"
#include "quad.h"

class Tree : public SceneNode {
  public:
    Tree();
    ~Tree();
    void init();
    void setColor(glm::vec4 color);
    void draw();
    void drawOccluder();

  private:
    TexturedQuad trunk_quad_, leaves_quad_;
    PathShape trunk_shape_, leaves_shape_;
};

#endif  // SRC_TREE_H_
