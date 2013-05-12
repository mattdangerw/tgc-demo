#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <list>
#include <map>

#include "engine/entity.h"
#include "engine/shader_program.h"

using std::string;
using std::vector;
using std::map;

class Engine;

Engine &theEngine();

// Does all the setting up of OpenGL and draws all the shapes in the scene.
// Also manages textures and shader loading.
// A singleton class, mainly because I only ever want one and I'm too lazy to pass it around everywhere.
class Engine {
  public:
    Engine();
    ~Engine();
    void init(int width, int height);
    // Get the root of the 2D scene graph.
    Entity *rootEntity() { return &root_entity_; }
    // draw everything
    void draw();
    // update everything
    void update(float delta_time) { root_entity_.updateAll(delta_time); }

    // Gets the length in x axis of the area the camera will render.
    float windowWidth() { return aspect_; }
    // Sets where the left side of the camera should be.
    void setLeftOfWindow(float x) { left_of_window_ = x; }
    float getLeftOfWindow() { return left_of_window_; }
    // Sets location of our light source for the god rays.
    void setLightPosition(glm::vec2 position) { light_position_ = position; }

    // GL stuff....
    void useProgram(string program);
    // Get handle for uniform shader variable for currently in use program.
    GLuint uniformHandle(string uniform);
    // Get handle for varying attribute these do not change across programs.
    GLuint attributeHandle(string attribute);
    // Get a texture handle by filename. Keeps two different objects from loading the same texture to memory.
    GLuint getTexture(string filename);

  private:
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
    Entity root_entity_;
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

#endif  // SRC_ENGINE_H_
