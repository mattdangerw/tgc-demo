#ifndef SRC_GROUND_H_
#define SRC_GROUND_H_

#include <glm/glm.hpp>
#include <vector>

#include "engine/shape.h"
#include "engine/shape_group.h"
#include "engine/fill.h"

using std::vector;

class Ground : public Entity {
  public:
    Ground();
    ~Ground();
    void init(vector<glm::vec2> points);
    float width();
    float heightAt(float x);
  private:
    void initShape();
    vector<glm::vec2> points_;
    // Drawables.
    Shape shape_;
    TexturedFill fill_;
};

#endif  // SRC_GROUND_H_
