#ifndef SRC_SHADER_PROGRAM_H_
#define SRC_SHADER_PROGRAM_H_

#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

class Shader {
  public:
    Shader();
    ~Shader();
    void load(string filename, GLenum type);
    GLuint handle() { return handle_; }

  private:
    GLuint handle_;
    string filename_;
};

class Program {
  public:
    Program();
    ~Program();
    void init();
    void addShader(Shader *shader);
    void link();
    void use();
    GLuint handle() { return handle_; }
    // Call before linking to enforce the location of an attribute handle.
    void setAttributeHandle(string attribute, GLuint handle);
    GLuint attributeHandle(string attribute);
    GLuint uniformHandle(string uniform);

  private:
    bool linked_;
    GLuint handle_;
    map<string, GLuint> attribute_handles_;
    map<string, GLuint> uniform_handles_;
    vector<Shader *> shaders_;
};

#endif  // SRC_SHADER_PROGRAM_H_
