#include "shader_program.h"

#include <cstdio>

#include "error.h"

Shader::Shader() : handle_(0) {}

Shader::~Shader() {
  if (handle_ != 0) glDeleteShader(handle_);
}

void Shader::load(string filename, GLenum type) {
  // Read the file into a buffer.
  FILE *file_pointer = fopen(filename.c_str(), "r");
  if (file_pointer == NULL) {
    error("Shader file %s not found.\n", filename.c_str());
  }
  fseek(file_pointer, 0, SEEK_END);
  long size = ftell(file_pointer);
  fseek(file_pointer, 0, SEEK_SET);
  char *source = new char[size + 1];
  long chars_read = fread(source, sizeof(char), size, file_pointer);
  source[chars_read] = '\0';
  fclose(file_pointer);
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
    error("%s", log);
  }
  delete source;
}

Program::Program()
  : handle_(0),
    linked_(false) {}

Program::~Program() {
  if (handle_ != 0) glDeleteProgram(handle_);
}

void Program::create() {
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
  // Comment out for gDebugger??
  glGetProgramiv(handle_, GL_COMPILE_STATUS, &linked);
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
  GLint handle = glGetAttribLocation(handle_, attribute.c_str());
  if (handle == -1) {
    error("Shader attribute %s not found.\n", attribute.c_str());
  }
  return static_cast<GLuint>(handle);
}

GLuint Program::uniformHandle(string uniform) {
  GLint handle = glGetUniformLocation(handle_, uniform.c_str());
  if (handle == -1) {
    error("Shader uniform %s not found.\n", uniform.c_str());
  }
  return static_cast<GLuint>(handle);
}
