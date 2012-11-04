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

class Drawable {
  public:
    virtual void draw() = 0;
};

class SceneNode : public Drawable {
  public:
    SceneNode();
    virtual ~SceneNode();
    // Make the GL calls to draw this object. Drawables should redefine these.
    virtual void draw() {}
    virtual void drawOccluder() {}
    // Sets the drawable parent. Setting parent to NULL removes this drawable and all children from the scene graph.
    SceneNode *parent() { return parent_; }
    void setParent(SceneNode *parent);
    // Gets all visible descendents of tree for rendering.
    void getVisibleDescendants(vector<SceneNode *> *drawables);
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
    float occluderColor() { return occluder_color_; }
    void setOccluderColor(float color) { occluder_color_ = color; }
    // Controls whether or not to draw this shape to the stencil buffer for the stencil test before the 3D is drawn.
    bool is3DStencil() { return is_3D_stencil_; }
    void setIs3DStencil(bool stencil) { is_3D_stencil_ = stencil; }
  private:
    void addChild(SceneNode *child);
    void removeChild(SceneNode *child);
    // Member data.
    SceneNode *parent_;
    vector<SceneNode *> children_;
    glm::mat3 relative_transform_;
    int priority_;
    bool is_occluder_, is_3D_stencil_, is_visible_;
    float occluder_color_;
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
    void draw();
    // Get the root of the 2D scene graph.
    SceneNode *rootNode() { return &root_node_; }
    // Add/remove a 3D drawable to scene. No currently part of scene graph.
    void addDrawable3D(Drawable *object);
    void removeDrawable3D(Drawable *object);
    // Sets location of our light source for the god rays.
    void setLightPosition(glm::vec2 position) { light_position_ = position; }
    // Gets the length in x axis of the area the camera will render.
    float windowWidth() { return aspect_; }
    // Sets where the left side of the camera should be.
    void setLeftOfWindow(float x) { left_of_window_ = x; }
    float getLeftOfWindow() { return left_of_window_; }
    // Stop using stencil test for particle drawing
    void stopStenciling() { do_stencil_ = false; }
    void useProgram(string program);
    // Get handle for uniform shader variable for currently in use program.
    GLuint uniformHandle(string uniform);
    // Get handle for varying attribute these do not change across programs.
    GLuint attributeHandle(string attribute);
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
    void setAttributesAndLink();
    void setTextureUnits();
    // Memeber data.
    int width_, height_;
    float aspect_, left_of_window_;
    bool do_stencil_;
    glm::vec2 light_position_;
    SceneNode root_node_;
    vector<Drawable *> draw3D_;
    Program *current_program_;
    map<string, Program> programs_;
    map<string, GLuint> attribute_handles_;
    map<string, GLuint> textures_;
    // GL.
    GLuint occluder_frame_buffer_, occluder_texture_, occluder_stencil_;
    GLuint shadow_frame_buffer_, shadow_texture_;
    GLuint quad_array_object_;
};

#endif  // SRC_RENDERER_H_
