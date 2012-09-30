#include "shader_program.h"

#include <cstdio>

// Forward declare out exit point.
void cleanupAndExit(int exit_code);

Shader::Shader() : handle_(0) {}

Shader::~Shader() {
  if (handle_ != 0) glDeleteShader(handle_);
}

void Shader::load(string filename, GLenum type) {
  // Read the file into a buffer.
  FILE *file_pointer = fopen(filename.c_str(), "r");
  if (file_pointer == NULL) {
    fprintf(stderr, "Shader file %s not found.\n", filename.c_str());
    cleanupAndExit(1);
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
    fprintf(stderr, "%s", log);
    delete log;
    cleanupAndExit(1);
  }
  delete source;
}

Program::Program() : handle_(0) {}

Program::~Program() {
  if (handle_ != 0) glDeleteProgram(handle_);
}

void Program::addShader(Shader *shader) {
  shaders_.push_back(shader);
}

void Program::link() {
  handle_ = glCreateProgram();
  for (vector<Shader *>::iterator it = shaders_.begin(); it != shaders_.end(); ++it) {
    glAttachShader(handle_, (*it)->handle());
  }
  glLinkProgram(handle_);
  GLint linked;
  glGetProgramiv(handle_, GL_COMPILE_STATUS, &linked);
  if (linked == false) {
    GLint log_length;
    glGetProgramiv(handle_, GL_INFO_LOG_LENGTH , &log_length);
    GLchar *log = new GLchar[log_length];
    glGetProgramInfoLog(handle_, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    delete log;
    cleanupAndExit(1);
  }
}

void Program::use() {
  glUseProgram(handle_);
}

GLint Program::attributeHandle(string attribute) {
  GLint handle = glGetAttribLocation(handle_, attribute.c_str());
  if (handle == -1) {
    fprintf(stderr, "Shader attribute %s not found.\n", attribute.c_str());
    cleanupAndExit(1);
  }
  return handle;
}

GLint Program::uniformHandle(string uniform) {
  GLint handle = glGetUniformLocation(handle_, uniform.c_str());
  if (handle == -1) {
    fprintf(stderr, "Shader uniform %s not found.\n", uniform.c_str());
    cleanupAndExit(1);
  }
  return handle;
}
