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
    Drawable() : transform_(1.0f), priority_(0), occluder_(true) {};
    virtual ~Drawable() {}
    // Make the GL calls to draw this object.
    virtual void draw(glm::mat3 view) = 0;
    virtual void drawOcclude(glm::mat3 view) = 0;
    // The local transform of the drawable.
    glm::mat3 transform() { return transform_; }
    void setTransform(const glm::mat3 &transform) { transform_ = transform; }
    // Multiplies this shapes transform in with the rest of the stack.
    glm::mat3 modelview(glm::mat3 view) { return view * transform(); }
    // Helpers for setting common uniforms in our shaders.
    void setModelviewUniform(Program *program, glm::mat3 view) {
      glUniformMatrix3fv(program->uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(modelview(view))); 
    }
    void setColorMaskUniform(Program *program, glm::vec4 color_mask) {
      glUniform4fv(program->uniformHandle("color_mask"), 1, glm::value_ptr(color_mask));
    }
    // We care about order cause we render in flatland.
    int displayPriority() const { return priority_; }
    void setDisplayPriority(int priority) { priority_ = priority; }
    // Controls whether or not to consider this shape an occluder while shading.
    bool occluder() { return occluder_; }
    void setOccluder(bool occluder) { occluder_ = occluder; }

  private:
    glm::mat3 transform_;
    int priority_;
    bool occluder_;
};

class Drawable3D {
  public:
    Drawable3D() : transform_(1.0f) {};
    virtual ~Drawable3D() {}
    // Make the GL calls to draw this object.
    virtual void draw(glm::mat4 projection) = 0;
    // The local transform of the drawable.
    glm::mat4 transform() { return transform_; }
    void setTransform(const glm::mat4 &transform) { transform_ = transform; }
    // Multiplies this shapes transform in with the rest of the stack.
    glm::mat4 mvp(glm::mat4 projection) { return projection * transform(); }
    // Helpers for setting common uniforms in our shaders.
    void setMVPUniform(Program *program, glm::mat4 projection) {
      glUniformMatrix4fv(program->uniformHandle("mvp"), 1, GL_FALSE, glm::value_ptr(mvp(projection))); 
    }
  private:
    glm::mat4 transform_;
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
    // Adds a drawable object to the renderer.
    // Not memory managed! Game entities should allocate and free drawable objects.
    void addDrawable(Drawable *object);
    // Removes a drawable object from the renderer.
    // Use this before deleting a drawable or to make the drawable invisible.
    void removeDrawable(Drawable *object);
    // Adds a shape to be drawn into the stencil buffer before drawing the particles.
    void addStencilShape(Drawable *object);
    void removeStencilShape(Drawable *object);
    // Adds the particles which are drawn with different opengl setting. and maybe 3d?
    void addParticles(Drawable3D *particles) { particles_ = particles; }
    // Sets location of our light source for the god rays.
    void setLightPosition(glm::vec2 position) { light_position_ = position; }
    // Gets the length in x axis of the area the camera will render.
    float windowWidth() { return aspect_; }
    // Sets where the left side of the camera should be.
    void setLeftOfWindow(float x) { left_of_window_ = x; }
    float getLeftOfWindow() { return left_of_window_; }
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
    float aspect_;
    float left_of_window_;
    bool do_stencil_;
    vector<Drawable *> draw_normal_, draw_stencil_;
    Drawable3D *particles_;
    glm::vec2 light_position_;
    map<string, Program> programs_;
    map<string, GLuint> textures_;
    // GL.
    GLuint occluder_frame_buffer_, occluder_texture_, occluder_stencil_;
    GLuint shadow_frame_buffer_, shadow_texture_;
    GLuint quad_array_object_;
    GLuint light_position_handle_;
};

#endif  // SRC_RENDERER_H_
