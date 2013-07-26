#ifndef SRC_SHAPE_GROUP_H_
#define SRC_SHAPE_GROUP_H_

#include <glm/glm.hpp>
#include <map>
#include <string>

#include "engine/entity.h"
#include "engine/quad.h"
#include "engine/shape.h"
#include "engine/animator.h"

using std::string;
using std::map;

struct ShapeAndFill{
  Shape shape;
  Quad fill;
};

class ShapeGroup : public Entity { 
  public:
    ShapeGroup();
    ~ShapeGroup();
    void init(string filename);
    Animator &animator() { return animator_; }
    void colorize();
    void setColorMultipliers(glm::vec4 color);
    void setColorAdditions(glm::vec4 color);
  private:
    map<string, ShapeAndFill *> shapes_;
    map<string, glm::vec4> colors_;
    Animator animator_;
};

#endif  // SRC_SHAPE_GROUP_H_
