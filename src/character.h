#ifndef SRC_CHARACTER_H_
#define SRC_CHARACTER_H_

#include <string>

#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "renderer.h"
#include "ground.h"

using std::string;

class Character : Drawable {
  public:
    Character();
    ~Character();
    void init(Renderer *renderer, Ground *ground);
    void draw();
    string shadingGrounp();
  private:
    Renderer *renderer;
    Ground *ground;
};

#endif  // SRC_CHARACTER_H_
