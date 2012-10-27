#ifndef SRC_RENDERER_H_
#define SRC_RENDERER_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <map>

#include "shader_program.h"

using std::string;
using std::vector;
using std::map;

class Drawable2D {
  public:
    Drawable2D();
    virtual ~Drawable2D();
    // Make the GL calls to draw this object. Drawables should redefine these.
    virtual void draw() {}
    virtual void drawOccluder() {}
    // Sets the drawable parent. Setting parent to NULL removes this drawable and all children from the scene graph.
    Drawable2D *parent() { return parent_; }
    void setParent(Drawable2D *parent);
    // Gets all visible descendents of tree for rendering.
    void getVisibleDescendants(vector<Drawable2D *> &drawables);
    // Get the full transform of drawable element.
    glm::mat3 fullTransform();
    // Get the transform relative to the parent drawable
    glm::mat3 relativeTransform() { return relative_transform_; }
    void setRelativeTransform(const glm::mat3 &transform) { relative_transform_ = transform; }
    // We care about order cause we render in flatland.
    int displayPriority() const { return priority_; }
    void setDisplayPriority(int priority) { priority_ = priority; }
    // Controls visibility.
    bool isVisible() { return is_visible_; }
    void setIsVisible(bool visible) { is_visible_ = visible; }
    // Controls whether or not to consider this shape an occluder while shading.
    bool isOccluder() { return is_occluder_ && is_visible_; }
    void setIsOccluder(bool occluder) { is_occluder_ = occluder; }
    // Controls whether or not to draw this shape to the stencil buffer for the stencil test before the 3D is drawn.
    bool is3DStencil() { return is_3D_stencil_; }
    void setIs3DStencil(bool stencil) { is_3D_stencil_ = stencil; }
  private:
    void addChild(Drawable2D *child);
    void removeChild(Drawable2D *child);
    // Member data.
    Drawable2D *parent_;
    vector<Drawable2D *> children_;
    glm::mat3 relative_transform_;
    int priority_;
    bool is_occluder_, is_3D_stencil_, is_visible_;
};

class Drawable3D {
  public:
    Drawable3D() : transform2D_(1.0f), transform3D_(1.0f) {}
    virtual ~Drawable3D() {}
    // Make the GL calls to draw this object.
    virtual void draw(glm::mat4 parent_transform3D, glm::mat3 parent_transform2D) = 0;
    // The local transform of the drawable.
    glm::mat4 transform3D() { return transform3D_; }
    void setTransform3D(const glm::mat4 &transform) { transform3D_ = transform; }
    glm::mat3 transform2D() { return transform2D_; }
    void setTransform2D(const glm::mat3 &transform) { transform2D_ = transform; }
    // Helpers for setting common uniforms in our shaders.
    void setTransform3DUniform(Program *program, glm::mat4 parent_transform) {
      glUniformMatrix4fv(program->uniformHandle("transform3D"), 1, GL_FALSE, glm::value_ptr(parent_transform * transform3D_)); 
    }
    // Helpers for setting common uniforms in our shaders.
    void setTransform2DUniform(Program *program, glm::mat3 parent_transform) {
      glUniformMatrix3fv(program->uniformHandle("transform2D"), 1, GL_FALSE, glm::value_ptr(parent_transform * transform2D_)); 
    }

  private:
    glm::mat4 transform3D_;
    glm::mat3 transform2D_;
};

// Does all the setting up of OpenGL and draws all the shapes in the scene.
// Also manages textures and shader loading.
// A singleton class, because I only ever want one and I'm too lazy to pass it around everywhere.
class Renderer {
  public:
    static Renderer& instance() {
      static Renderer instance;
      return instance;
    }
    // Does GL calls to prep for rendering.
    void init(int width, int height);
    // Renders the scene.
    void draw();
    // Get the root of the 2D scene graph.
    Drawable2D *root2D() { return &root2D_; }
    // Adds the particles which are drawn with different opengl setting. and maybe 3d?
    void addDrawable3D(Drawable3D *object);
    void removeDrawable3D(Drawable3D *object);
    // Sets location of our light source for the god rays.
    void setLightPosition(glm::vec2 position) { light_position_ = position; }
    // Gets the length in x axis of the area the camera will render.
    float windowWidth() { return aspect_; }
    // Sets where the left side of the camera should be.
    void setLeftOfWindow(float x) { left_of_window_ = x; }
    float getLeftOfWindow() { return left_of_window_; }
    // Projection matrix.
    glm::mat4 projection() { return projection_; }
    glm::mat4 inverseProjection() { return inverse_projection_; }
    // Stop using stencil test for particle drawing
    void stopStenciling() { do_stencil_ = false; }
    // Get a shader program by string name.
    Program *getProgram(string name);
    // Get a texture handle by filename. Keeps two different objects from loading the same texture to memory.
    GLuint getTexture(string filename);

  private:
    // Singleton. Keep these private.
    Renderer();
    Renderer(Renderer const& copy);
    Renderer& operator=(Renderer const& copy);
    // Helper methods.
    void setupScreenQuad();
    void setupFBOs();
    void loadShaders();
    void setAttributesAndLink(Program &program);
    void setTextureUnits();
    // Memeber data.
    int width_, height_;
    float aspect_, left_of_window_;
    bool do_stencil_;
    glm::mat4 projection_, inverse_projection_;
    glm::vec2 light_position_;
    Drawable2D root2D_;
    vector<Drawable3D *> draw3D_;
    map<string, Program> programs_;
    map<string, GLuint> textures_;
    // GL.
    GLuint occluder_frame_buffer_, occluder_texture_, occluder_stencil_;
    GLuint shadow_frame_buffer_, shadow_texture_;
    GLuint quad_array_object_;
    GLuint light_position_handle_;
};

#endif  // SRC_RENDERER_H_
