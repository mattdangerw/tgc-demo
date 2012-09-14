#ifndef SRC_GROUND_H_
#define SRC_GROUND_H_

#include <string>
#include <vector>

#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "renderer.h"

using std::string;
using std::vector;

class Ground : Drawable {
  public:
    Ground();
    ~Ground();
    void init(Renderer *renderer);
    void draw();
    string shadingGrounp();
  private:
    vector<glm::vec2> points;
    Renderer *renderer;
};

#endif  // SRC_GROUND_H_
