#ifndef SRC_SHADER_PROGRAM_H_
#define SRC_SHADER_PROGRAM_H_

#include <GL/glew.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Shader {
  public:
    Shader();
    ~Shader();
    void load(string filename, GLenum type);
    GLuint handle() { return handle_; }

  private:
    GLuint handle_;
};

class Program {
  public:
    Program();
    ~Program();
    void create();
    void addShader(Shader *shader);
    void link();
    void use();
    GLuint handle() { return handle_; }
    void setAttributeHandle(string attribute, GLuint handle);
    GLuint attributeHandle(string attribute);
    GLuint uniformHandle(string uniform);

  private:
    bool linked_;
    GLuint handle_;
    vector<Shader *> shaders_;
};

#endif  // SRC_SHADER_PROGRAM_H_
