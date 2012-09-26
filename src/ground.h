#ifndef SRC_GROUND_H_
#define SRC_GROUND_H_

#include <string>
#include <vector>
#include <glm.hpp>

#include "renderer.h"

using std::string;
using std::vector;

class Ground : Drawable {
  public:
    Ground();
    ~Ground();
    void init(Renderer *renderer);
    float width();
    float heightAt(float x);
    void draw();
    string shadingGrounp() { return "default"; }

  private:
    vector<glm::vec2> points_;
    Renderer *renderer_;
};

#endif  // SRC_GROUND_H_
