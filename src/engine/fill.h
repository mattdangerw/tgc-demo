#ifndef SRC_FILL_H_
#define SRC_FILL_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

#include "engine/entity.h"
#include "engine/engine.h"

using std::string;

class Fill {
  public:
    virtual void fillIn(Entity *entity) = 0;
    virtual void fillInOccluder(Entity *entity);
};

class ColoredFill : public Fill {
  public:
    ColoredFill() : color_(glm::vec4(0.0f)) {}
    ~ColoredFill() {}
    void init(glm::vec4 color) { setColor(color); }
    void setColor(glm::vec4 color) { color_ = color; }
    void fillIn(Entity *entity);
  private:
    glm::vec4 color_;
};

class TexturedFill : public Fill {
  public:
    TexturedFill();
    ~TexturedFill() {}
    void init(string texture_file) { setTexture(texture_file); }
    void setTexture(string texture_file) { texture_handle_ = theEngine().getTexture(texture_file); }
    glm::vec2 textureScale() { return texture_scale_; }
    void setTextureScale(glm::vec2 scale) { texture_scale_ = scale; }
    glm::vec4 colorMultiplier() { return color_multiplier_; }
    void setColorMultiplier(glm::vec4 color_multiplier) { color_multiplier_ = color_multiplier; }
    glm::vec4 colorAddition() { return color_addition_; }
    void setColorAddition(glm::vec4 color_addition) { color_addition_ = color_addition; }
    bool shadowed() { return shadowed_; }
    void setShadowed(bool shadowed) { shadowed_ = shadowed; }
    void fillIn(Entity *entity);
  private:
    bool shadowed_;
    GLuint texture_handle_;
    glm::vec2 texture_scale_;
    glm::vec4 color_multiplier_, color_addition_;
};

#endif  // SRC_FILL_H_
