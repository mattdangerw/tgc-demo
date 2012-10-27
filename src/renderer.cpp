#include "renderer.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <gli/gli.hpp>
#include <gli/gtx/gl_texture2d.hpp>
#include <algorithm>

#include "transform2D.h"

// Forward declare out exit point.
void cleanupAndExit(int exit_code);

Drawable2D::Drawable2D()
  : relative_transform_(1.0f),
    priority_(0),
    is_occluder_(true),
    is_3D_stencil_(false),
    parent_(Renderer::instance().root2D()) {}

Drawable2D::~Drawable2D() {
  if (parent_ != NULL) {
    parent_->removeChild(this);
  }
}

void Drawable2D::drawWithChildren() {
  draw();
  for (vector<Drawable2D *>::iterator it = children_.begin(); it != children_.end(); ++it) {
    (*it)->drawWithChildren();
  }
}

void Drawable2D::drawOccluderWithChildren() {
  drawOccluder();
  for (vector<Drawable2D *>::iterator it = children_.begin(); it != children_.end(); ++it) {
    (*it)->drawOccluderWithChildren();
  }
}

void Drawable2D::drawStencilWithChildren() {
  if (is3DStencil()) draw();
  for (vector<Drawable2D *>::iterator it = children_.begin(); it != children_.end(); ++it) {
    (*it)->drawStencilWithChildren();
  }
}

void Drawable2D::setParent(Drawable2D *parent) {
  if (parent_ != NULL) parent_->removeChild(this);
  parent_ = parent;
  if (parent_ != NULL) parent_->addChild(this);
}

glm::mat3 Drawable2D::fullTransform() {
  if (parent_ == NULL) return relative_transform_;
  return parent_->fullTransform() * relative_transform_;
}

void Drawable2D::addChild(Drawable2D *child) {
  children_.push_back(child);
}

void Drawable2D::removeChild(Drawable2D *child) {
  vector<Drawable2D *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    if (*it == child) {
      children_.erase(it);
      return;
    }
  }
}

Renderer::Renderer()
  : left_of_window_(0.0f),
    do_stencil_(true),
    light_position_(0.0f) {}

void Renderer::init(int width, int height) {
  width_ = width;
  height_ = height;
  aspect_ = static_cast<float>(width)/height;
  projection_ = glm::perspective(35.0f, 1.0f, 0.1f, 100.0f);
  inverse_projection_ = glm::inverse(projection_);

  // OpenGL settings.
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  loadShaders();
  setupScreenQuad();
  setupFBOs();

  Program *shadows_program = getProgram("shadows");
  shadows_program->use();
  light_position_handle_ = shadows_program->uniformHandle("light_position");
  glUniform1f(shadows_program->uniformHandle("density"), 2.0f);
  glUniform1f(shadows_program->uniformHandle("decay_rate"), 0.98f);
  glUniform1f(shadows_program->uniformHandle("constant_factor"), 0.85f);
  glUniform1f(shadows_program->uniformHandle("scale_factor"), 1.0f/160.0f);
}

void Renderer::setupScreenQuad() {
  glm::vec2 vertices[4];
  vertices[0] = glm::vec2(-1.0f, -1.0f);
  vertices[1] = glm::vec2(1.0f, -1.0f);
  vertices[2] = glm::vec2(1.0f, 1.0f);
  vertices[3] = glm::vec2(-1.0f, 1.0f);
  glm::vec2 tex_coords[4];
  tex_coords[0] = glm::vec2(0.0f, 0.0f);
  tex_coords[1] = glm::vec2(1.0f, 0.0f);
  tex_coords[2] = glm::vec2(1.0f, 1.0f);
  tex_coords[3] = glm::vec2(0.0f, 1.0f);

  GLuint buffer_objects[2];  
  glGenVertexArrays(1, &quad_array_object_);
  glGenBuffers(2, buffer_objects);

  glBindVertexArray(quad_array_object_);
  
  glBindBuffer(GL_ARRAY_BUFFER, buffer_objects[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  GLint handle = programs_["shadows"].attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, buffer_objects[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
  handle = programs_["shadows"].attributeHandle("tex_coord");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void Renderer::setupFBOs() {
  glGenFramebuffers(1, &occluder_frame_buffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, occluder_frame_buffer_);

  glGenTextures(1, &occluder_texture_);
  glBindTexture(GL_TEXTURE_2D, occluder_texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_/2, height_/2, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, occluder_texture_, 0);
  
  glGenRenderbuffers(1, &occluder_stencil_);
  glBindRenderbuffer(GL_RENDERBUFFER, occluder_stencil_);
  // I think we need the depth packaged along with stencil. Stencil only formats aren't supported on any hardware.
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_/2, height_/2);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, occluder_stencil_);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Occlusion framebuffer object not complete. Something went wrong :(\n");
    cleanupAndExit(1);
  }

  glGenFramebuffers(1, &shadow_frame_buffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_frame_buffer_);

  glGenTextures(1, &shadow_texture_);
  glBindTexture(GL_TEXTURE_2D, shadow_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_/2, height_/2, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow_texture_, 0);
  
  status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Exposure framebuffer object not complete. Something went wrong :(\n");
    cleanupAndExit(1);
  }
}

void Renderer::loadShaders() {
  Shader general_vert, textured_frag, textured_with_shadows_frag, colored_frag, minimal_frag,
    quadric_frag, circles_frag, particles_vert, particles_frag, shadows_vert, shadows_frag,
    circles_textured_frag, circles_screen_textured_frag;

  general_vert.load("src/shaders/general.vert", GL_VERTEX_SHADER);
  textured_frag.load("src/shaders/textured.frag", GL_FRAGMENT_SHADER);
  Program &textured = programs_["textured"];
  textured.addShader(&general_vert);
  textured.addShader(&textured_frag);
  setAttributesAndLink(textured);

  textured_with_shadows_frag.load("src/shaders/textured_with_shadows.frag", GL_FRAGMENT_SHADER);
  Program &textured_with_shadows = programs_["textured_with_shadows"];
  textured_with_shadows.addShader(&general_vert);
  textured_with_shadows.addShader(&textured_with_shadows_frag);
  setAttributesAndLink(textured_with_shadows);

  colored_frag.load("src/shaders/colored.frag", GL_FRAGMENT_SHADER);
  Program &colored = programs_["colored"];
  colored.addShader(&general_vert);
  colored.addShader(&colored_frag);
  setAttributesAndLink(colored);
  
  minimal_frag.load("src/shaders/minimal.frag", GL_FRAGMENT_SHADER);
  Program &minimal = programs_["minimal"];
  minimal.addShader(&general_vert);
  minimal.addShader(&minimal_frag);
  setAttributesAndLink(minimal);
  
  quadric_frag.load("src/shaders/quadric_anti_aliased.frag", GL_FRAGMENT_SHADER);
  Program &quadric = programs_["quadric"];
  quadric.addShader(&general_vert);
  quadric.addShader(&quadric_frag);
  setAttributesAndLink(quadric);
  
  circles_frag.load("src/shaders/circles_anti_aliased.frag", GL_FRAGMENT_SHADER);
  Program &circles = programs_["circles"];
  circles.addShader(&general_vert);
  circles.addShader(&circles_frag);
  setAttributesAndLink(circles);
  
  circles_screen_textured_frag.load("src/shaders/circles_screen_textured.frag", GL_FRAGMENT_SHADER);
  Program &circles_screen_textured = programs_["circles_screen_textured"];
  circles_screen_textured.addShader(&general_vert);
  circles_screen_textured.addShader(&circles_screen_textured_frag);
  setAttributesAndLink(circles_screen_textured);

  particles_vert.load("src/shaders/particles.vert", GL_VERTEX_SHADER);
  particles_frag.load("src/shaders/particles.frag", GL_FRAGMENT_SHADER);
  Program &particles = programs_["particles"];
  particles.addShader(&particles_vert);
  particles.addShader(&particles_frag);
  setAttributesAndLink(particles);

  shadows_vert.load("src/shaders/shadows.vert", GL_VERTEX_SHADER);
  shadows_frag.load("src/shaders/shadows.frag", GL_FRAGMENT_SHADER);
  Program &shadows = programs_["shadows"];
  shadows.addShader(&shadows_vert);
  shadows.addShader(&shadows_frag);
  setAttributesAndLink(shadows);

  setTextureUnits();
}

void Renderer::setAttributesAndLink(Program &program) {
  program.create();
  // Keep our vertex attributes in a consistent location accross programs.
  // This way we can VAOs with different programs without worrying.
  program.setAttributeHandle("position", 0);
  program.setAttributeHandle("color", 1);
  program.setAttributeHandle("tex_coord", 2);
  program.setAttributeHandle("bezier_coord", 3);
  program.setAttributeHandle("translate", 4);
  program.link();
}

void Renderer::setTextureUnits() {
  Program &textured = programs_["textured"];
  textured.use();
  glUniform1i(textured.uniformHandle("color_texture"), 0);

  Program &circles_screen_textured = programs_["circles_screen_textured"];
  circles_screen_textured.use();
  glUniform1i(circles_screen_textured.uniformHandle("color_texture"), 0);

  Program &textured_with_shadows = programs_["textured_with_shadows"];
  textured_with_shadows.use();
  glUniform1i(textured_with_shadows.uniformHandle("color_texture"), 0);
  glUniform1i(textured_with_shadows.uniformHandle("shadow_texture"), 1);

  Program &shadows = programs_["shadows"];
  shadows.use();
  glUniform1i(shadows.uniformHandle("occluder_texture"), 0);

  Program &particles = programs_["particles"];
  particles.use();
  glUniform1i(particles.uniformHandle("color_texture"), 0);
}

struct PrioritySortFunctor {
  bool operator() (const Drawable2D *left, const Drawable2D *right) {
    return left->displayPriority() < right->displayPriority();
  }
};

void Renderer::draw() {
  // 2D rendering modelview
  glm::mat3 view(1.0f);
  view = translate2D(view, glm::vec2(-1.0f, -1.0f));
  view = scale2D(view, glm::vec2(2.0f/aspect_, 2.0f));
  view = translate2D(view, glm::vec2(-left_of_window_, 0.0f));

  // Sort drawables by priority.
  std::stable_sort(draw2D_.begin(), draw2D_.end(), PrioritySortFunctor());

  // Draw occluders to texture.
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, occluder_frame_buffer_);
  glViewport(0,0,width_/2, height_/2);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_FALSE);
  for (vector<Drawable2D *>::iterator it = draw2D_.begin(); it != draw2D_.end(); ++it) {
    if ((*it)->occluder()) (*it)->drawOcclude(view);
  }
  
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_frame_buffer_);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_FALSE);
  programs_["shadows"].use();
  glm::vec3 transformed_light_position = view * glm::vec3(light_position_, 1.0f);
  glUniform2fv(light_position_handle_, 1, glm::value_ptr(transformed_light_position));
  glBindTexture(GL_TEXTURE_2D, shadow_texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, occluder_texture_);
  glBindVertexArray(quad_array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0,0,width_, height_);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_FALSE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadow_texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  for (vector<Drawable2D *>::iterator it = draw2D_.begin(); it != draw2D_.end(); ++it) {
    (*it)->draw(view);
  }

  if (do_stencil_) {
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    for (vector<Drawable2D *>::iterator it = draw_stencil_.begin(); it != draw_stencil_.end(); ++it) {
      (*it)->draw(view);
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  }
  glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
  glEnable(GL_BLEND);
  for (vector<Drawable3D *>::iterator it = draw3D_.begin(); it != draw3D_.end(); ++it) {
    (*it)->draw(projection_, view);
  }
  glDisable(GL_BLEND);
  if (do_stencil_) {
    glDisable(GL_STENCIL_TEST);
  }
}

void Renderer::addDrawable3D(Drawable3D *object) {
  draw3D_.push_back(object);
}

void Renderer::removeDrawable3D(Drawable3D *object) {
  vector<Drawable3D *>::iterator it;
  for (it = draw3D_.begin(); it != draw3D_.end(); ++it) {
    if (*it == object) {
      draw3D_.erase(it);
      return;
    }
  }
}

Program *Renderer::getProgram(string name) {
  if (programs_.count(name) == 0) {
    return NULL;
  }
  return &programs_[name];
}

GLuint Renderer::getTexture(string filename) {
  if (textures_.count(filename) == 0) {
    textures_[filename] = gli::createTexture2D(filename);
  }
  return textures_[filename];
}
