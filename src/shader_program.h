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
    void addShader(Shader *shader);
    void link();
    void use();
    GLuint handle() { return handle_; }
    GLint attributeHandle(string attribute);
    GLint uniformHandle(string uniform);

  private:
    GLuint handle_;
    vector<Shader *>shaders_;
};

#endif  // SRC_SHADER_PROGRAM_H_
