#ifndef SRC_RENDERER_H_
#define SRC_RENDERER_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class Drawable {
  public:
    virtual ~Drawable() {}
    // Make the GL calls to draw this object.
    virtual void draw() = 0;
    // Name of the shading group that should be bound before calling draw.
    virtual string shadingGrounp() = 0;
};

class Renderer {
  public:
    Renderer();
    ~Renderer();
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
    void setCameraScroll(float x) { camera_scroll_ = x; }
    float getCameraScroll() { return camera_scroll_; }
    // Update the size of the render window.
    void resize(int width, int height);

  private:
    vector<Drawable *> to_draw_;
    float aspect_;
    float camera_scroll_;
};

#endif  // SRC_RENDERER_H_
