#include "engine/shader_program.h"

#include <cstdio>

#include "util/error.h"
#include "util/read_file.h"

Shader::Shader() : handle_(0), filename_("") {}

Shader::~Shader() {
  if (handle_ != 0) glDeleteShader(handle_);
}

void Shader::load(string filename, GLenum type) {
  filename_ = filename;
  // Read the file into a buffer.
  char *source = readFileToCString(filename);
  // Set up and compile shader.
  handle_ = glCreateShader(type);
  glShaderSource(handle_, 1, const_cast<const GLchar **>(&source), NULL);
  glCompileShader(handle_);
  // Check compile.
  GLint compiled;
  glGetShaderiv(handle_, GL_COMPILE_STATUS, &compiled);
  if (compiled == GL_FALSE) {
    GLint log_length;
    glGetShaderiv(handle_, GL_INFO_LOG_LENGTH , &log_length);
    GLchar *log = new GLchar[log_length];
    glGetShaderInfoLog(handle_, log_length, NULL, log);
    error("Error compiling file %s:\n%s", filename_.c_str(), log);
  }
  delete source;
}

Program::Program()
  : handle_(0),
    linked_(false) {}

Program::~Program() {
  if (handle_ != 0) glDeleteProgram(handle_);
}

void Program::init() {
  handle_ = glCreateProgram();
}

void Program::addShader(Shader *shader) {
  shaders_.push_back(shader);
}

void Program::link() {
  for (vector<Shader *>::iterator it = shaders_.begin(); it != shaders_.end(); ++it) {
    glAttachShader(handle_, (*it)->handle());
  }
  glLinkProgram(handle_);
  GLint linked;
  glGetProgramiv(handle_, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE) {
    GLint log_length;
    glGetProgramiv(handle_, GL_INFO_LOG_LENGTH , &log_length);
    GLchar *log = new GLchar[log_length];
    glGetProgramInfoLog(handle_, log_length, NULL, log);
    error("%s", log);
  }
  linked_ = (linked == GL_TRUE);
}

void Program::use() {
  glUseProgram(handle_);
}

void Program::setAttributeHandle(string attribute, GLuint handle) {
  if (linked_) error("Shader aleardy linked.\n");
  glBindAttribLocation(handle_, handle, attribute.c_str());
}

GLuint Program::attributeHandle(string attribute) {
  if (attribute_handles_.count(attribute) == 0) {
    GLint handle_or_error = glGetAttribLocation(handle_, attribute.c_str());
    if (handle_or_error == -1) {
      error("Shader attribute %s not found.\n", attribute.c_str());
    }
    GLuint handle = static_cast<GLuint>(handle_or_error);
    attribute_handles_[attribute] = handle;
  }
  return attribute_handles_[attribute];
}

GLuint Program::uniformHandle(string uniform) {
  if (attribute_handles_.count(uniform) == 0) {
    GLint handle_or_error = glGetUniformLocation(handle_, uniform.c_str());
    if (handle_or_error == -1) {
      error("Shader uniform %s not found.\n", uniform.c_str());
    }
    GLuint handle = static_cast<GLuint>(handle_or_error);
    attribute_handles_[uniform] = handle;
  }
  return attribute_handles_[uniform];
}
