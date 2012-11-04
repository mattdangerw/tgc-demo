#include "renderer.h"

#include <GL/glew.h>
#include <gli/gli.hpp>
#include <gli/gtx/gl_texture2d.hpp>
#include <algorithm>

#include "error.h"
#include "transform2D.h"

SceneNode::SceneNode()
  : relative_transform_(1.0f),
    priority_(0),
    is_occluder_(true),
    occluder_color_(0.0f),
    is_3D_stencil_(false),
    is_visible_(true),
    parent_(Renderer::instance().rootNode()) {
  // Root node.
  if (parent_ == this) {
    parent_ = NULL;
  } else {
    parent_->addChild(this);
  }
}

SceneNode::~SceneNode() {
  vector<SceneNode *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    (*it)->parent_ = NULL;
  }
  if (parent_ != NULL) {
    parent_->removeChild(this);
  }
}

void SceneNode::setParent(SceneNode *parent) {
  if (parent_ != NULL) parent_->removeChild(this);
  parent_ = parent;
  if (parent_ != NULL) parent_->addChild(this);
}

void SceneNode::getVisibleDescendants(vector<SceneNode *> *drawables) {
  if (isVisible()) drawables->push_back(this);
  vector<SceneNode *>::iterator it;
  for (it = children_.begin(); it != children_.end(); ++it) {
    (*it)->getVisibleDescendants(drawables);
  }
}

glm::mat3 SceneNode::fullTransform() {
  if (parent_ == NULL) return relative_transform_;
  return parent_->fullTransform() * relative_transform_;
}

void SceneNode::addChild(SceneNode *child) {
  children_.push_back(child);
}

void SceneNode::removeChild(SceneNode *child) {
  vector<SceneNode *>::iterator it;
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
    light_position_(0.0f),
    current_program_(NULL) {}

void Renderer::init(int width, int height) {
  width_ = width;
  height_ = height;
  aspect_ = static_cast<float>(width)/height;

  // OpenGL settings.
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  loadShaders();
  setupScreenQuad();
  setupFBOs();

  useProgram("shadows");
  glUniform1f(uniformHandle("density"), 2.0f);
  glUniform1f(uniformHandle("decay_rate"), 0.98f);
  glUniform1f(uniformHandle("constant_factor"), 0.85f);
  glUniform1f(uniformHandle("scale_factor"), 1.0f/160.0f);
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
    error("Occlusion framebuffer object not complete. Something went wrong :(\n");
  }

  glGenFramebuffers(1, &shadow_frame_buffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_frame_buffer_);

  glGenTextures(1, &shadow_texture_);
  glBindTexture(GL_TEXTURE_2D, shadow_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_/2, height_/2, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadow_texture_, 0);
  
  status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    error("Exposure framebuffer object not complete. Something went wrong :(\n");
  }
}

void Renderer::loadShaders() {
  // Up to 16 this way. Then we'll have to think about what shaders need what attributes.
  attribute_handles_["position"] = 0;
  attribute_handles_["lerp_position1"] = 1;
  attribute_handles_["lerp_position2"] = 2;
  attribute_handles_["color"] = 3;
  attribute_handles_["tex_coord"] = 4;
  attribute_handles_["bezier_coord"] = 5;
  attribute_handles_["translate"] = 6;

  Shader general_vert, animated_vert, textured_frag, textured_with_shadows_frag, minimal_frag,
    quadric_frag, circles_frag, circles_screen_textured_frag, particles_vert, particles_frag, 
    shadows_vert, shadows_frag;
  general_vert.load("src/shaders/general.vert", GL_VERTEX_SHADER);
  animated_vert.load("src/shaders/animated.vert", GL_VERTEX_SHADER);
  textured_frag.load("src/shaders/textured.frag", GL_FRAGMENT_SHADER);
  textured_with_shadows_frag.load("src/shaders/textured_with_shadows.frag", GL_FRAGMENT_SHADER);
  minimal_frag.load("src/shaders/minimal.frag", GL_FRAGMENT_SHADER);
  quadric_frag.load("src/shaders/quadric_anti_aliased.frag", GL_FRAGMENT_SHADER);
  circles_frag.load("src/shaders/circles_anti_aliased.frag", GL_FRAGMENT_SHADER);
  circles_screen_textured_frag.load("src/shaders/circles_screen_textured.frag", GL_FRAGMENT_SHADER);
  particles_vert.load("src/shaders/particles.vert", GL_VERTEX_SHADER);
  particles_frag.load("src/shaders/particles.frag", GL_FRAGMENT_SHADER);
  shadows_vert.load("src/shaders/shadows.vert", GL_VERTEX_SHADER);
  shadows_frag.load("src/shaders/shadows.frag", GL_FRAGMENT_SHADER);
  
  programs_["textured"].addShader(&general_vert);
  programs_["textured"].addShader(&textured_frag);

  programs_["textured_with_shadows"].addShader(&general_vert);
  programs_["textured_with_shadows"].addShader(&textured_with_shadows_frag);

  programs_["minimal"].addShader(&general_vert);
  programs_["minimal"].addShader(&minimal_frag);
  
  programs_["minimal_animated"].addShader(&animated_vert);
  programs_["minimal_animated"].addShader(&minimal_frag);

  programs_["quadric"].addShader(&general_vert);
  programs_["quadric"].addShader(&quadric_frag);
  
  programs_["quadric_animated"].addShader(&animated_vert);
  programs_["quadric_animated"].addShader(&quadric_frag);

  programs_["circles"].addShader(&general_vert);
  programs_["circles"].addShader(&circles_frag);
  
  programs_["circles_screen_textured"].addShader(&general_vert);
  programs_["circles_screen_textured"].addShader(&circles_screen_textured_frag);

  programs_["particles"].addShader(&particles_vert);
  programs_["particles"].addShader(&particles_frag);

  programs_["shadows"].addShader(&shadows_vert);
  programs_["shadows"].addShader(&shadows_frag);

  setAttributesAndLink();
  setTextureUnits();
}

void Renderer::setAttributesAndLink() {
  for (map<string, Program>::iterator program_it = programs_.begin(); program_it != programs_.end(); ++program_it) {
    program_it->second.create();
    // Keep our vertex attributes in a consistent location accross programs.
    // This way we can VAOs with different programs without worrying.
    for (map<string, GLuint>::iterator attr_it = attribute_handles_.begin(); attr_it != attribute_handles_.end(); ++attr_it) {
      program_it->second.setAttributeHandle(attr_it->first, attr_it->second);
    }
    program_it->second.link();
  }
}

void Renderer::setTextureUnits() {
  useProgram("textured");
  glUniform1i(uniformHandle("color_texture"), 0);

  useProgram("circles_screen_textured");
  glUniform1i(uniformHandle("color_texture"), 0);

  useProgram("textured_with_shadows");
  glUniform1i(uniformHandle("color_texture"), 0);
  glUniform1i(uniformHandle("shadow_texture"), 1);

  useProgram("shadows");
  glUniform1i(uniformHandle("occluder_texture"), 0);

  useProgram("particles");
  glUniform1i(uniformHandle("color_texture"), 0);
}

struct PrioritySortFunctor {
  bool operator() (const SceneNode *left, const SceneNode *right) {
    return left->displayPriority() < right->displayPriority();
  }
};

void Renderer::draw() {
  // 2D rendering modelview
  glm::mat3 view(1.0f);
  view = translate2D(view, glm::vec2(-1.0f, -1.0f));
  view = scale2D(view, glm::vec2(2.0f/aspect_, 2.0f));
  view = translate2D(view, glm::vec2(-left_of_window_, 0.0f));
  root_node_.setRelativeTransform(view);

  // Sort drawables by priority.
  vector<SceneNode *> draw2D;
  root_node_.getVisibleDescendants(&draw2D);
  std::stable_sort(draw2D.begin(), draw2D.end(), PrioritySortFunctor());

  // Draw occluders to texture.
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glViewport(0,0,width_, height_);
  glBindFramebuffer(GL_FRAMEBUFFER, occluder_frame_buffer_);
  glViewport(0,0,width_/2, height_/2);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_FALSE);
  for (vector<SceneNode *>::iterator it = draw2D.begin(); it != draw2D.end(); ++it) {
    if ((*it)->isOccluder()) (*it)->drawOccluder();
  }
  //return;
  
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glViewport(0,0,width_, height_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_frame_buffer_);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_FALSE);
  useProgram("shadows");
  glm::vec3 transformed_light_position = view * glm::vec3(light_position_, 1.0f);
  glUniform2fv(uniformHandle("light_position"), 1, glm::value_ptr(transformed_light_position));
  glBindTexture(GL_TEXTURE_2D, shadow_texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, occluder_texture_);
  glBindVertexArray(quad_array_object_);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  //return;

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

  for (vector<SceneNode *>::iterator it = draw2D.begin(); it != draw2D.end(); ++it) {
    (*it)->draw();
  }

  if (do_stencil_) {
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    for (vector<SceneNode *>::iterator it = draw2D.begin(); it != draw2D.end(); ++it) {
      if ((*it)->is3DStencil()) (*it)->draw();
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  }
  glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
  glEnable(GL_BLEND);
  for (vector<Drawable *>::iterator it = draw3D_.begin(); it != draw3D_.end(); ++it) {
    (*it)->draw();
  }
  glDisable(GL_BLEND);
  if (do_stencil_) {
    glDisable(GL_STENCIL_TEST);
  }
}

void Renderer::addDrawable3D(Drawable *object) {
  draw3D_.push_back(object);
}

void Renderer::removeDrawable3D(Drawable *object) {
  vector<Drawable *>::iterator it;
  for (it = draw3D_.begin(); it != draw3D_.end(); ++it) {
    if (*it == object) {
      draw3D_.erase(it);
      return;
    }
  }
}

void Renderer::useProgram(string program) {
  if (programs_.count(program) == 0) error("No such program %s. Set it up in renderer.\n", program.c_str());
  current_program_ = &programs_[program];
  current_program_->use();
}

GLuint Renderer::uniformHandle(string uniform) {
  return current_program_->uniformHandle(uniform);
}

GLuint Renderer::attributeHandle(string attribute) {
  return attribute_handles_[attribute];
}

GLuint Renderer::getTexture(string filename) {
  if (textures_.count(filename) == 0) {
    textures_[filename] = gli::createTexture2D(filename);
  }
  return textures_[filename];
}
