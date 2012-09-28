#ifndef SRC_RENDERER_H_
#define SRC_RENDERER_H_

#include <glm.hpp>
#include <string>
#include <vector>
#include <map>

#include "shader_program.h"

using std::string;
using std::vector;
using std::map;

class Drawable {
  public:
    Drawable() : priority_(0) {};
    virtual ~Drawable() {}
    // Make the GL calls to draw this object.
    virtual void draw(glm::mat3 transform) = 0;
    // We care about order cause we render in flatland.
    int displayPriority() const { return priority_; }
    void setDisplayPriority(int priority) { priority_ = priority; }
    //bool operator <(const Drawable &other) const { return displayPriority() < other.displayPriority(); }
  private:
    int priority_;
};

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
    // Gets the length in x axis of the area the camera will render.
    float windowWidth() { return aspect_; }
    // Sets where the left side of the camera should be.
    void setLeftOfWindow(float x) { left_of_window_ = x; }
    float getLeftOfWindow() { return left_of_window_; }
    // Update the size of the render window.
    void resize(int width, int height);
    // Get a shader program by string name.
    Program *getProgram(string name);
    // Get a texture handle by filename. Keeps two different objects from loading the same texture to memory.
    GLuint getTexture(string filename);

  private:
    // Singleton. Keep these private.
    Renderer();
    Renderer(Renderer const& copy);
    Renderer& operator=(Renderer const& copy);
    vector<Drawable *> to_draw_;
    float aspect_;
    float left_of_window_;
    map<string, Program> programs_;
    map<string, GLuint> textures_;
};

#endif  // SRC_RENDERER_H_
