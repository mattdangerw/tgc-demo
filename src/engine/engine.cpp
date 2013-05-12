#include "engine/engine.h"

#include <GL/glew.h>
#include <gli/gli.hpp>
#include <gli/gtx/gl_texture2d.hpp>

#include "util/error.h"
#include "util/transform2D.h"

Engine the_engine;

Engine &theEngine() {
  return the_engine;
}

Engine::Engine()
  : left_of_window_(0.0f),
    do_stencil_(true),
    light_position_(0.0f),
    current_program_(NULL) {}

Engine::~Engine() {}

void Engine::init(int width, int height) {
  width_ = width;
  height_ = height;
  aspect_ = static_cast<float>(width)/height;

  // OpenGL settings.
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClearDepth(1.0f);
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

void Engine::setupScreenQuad() {
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
  GLuint handle = programs_["shadows"].attributeHandle("position");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ARRAY_BUFFER, buffer_objects[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
  handle = programs_["shadows"].attributeHandle("tex_coord");
  glEnableVertexAttribArray(handle);
  glVertexAttribPointer(handle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void Engine::setupFBOs() {
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

void Engine::loadShaders() {
  // Up to 16 this way. Then we'll have to think about what shaders need what attributes.
  attribute_handles_["position"] = 0;
  attribute_handles_["lerp_position1"] = 1;
  attribute_handles_["lerp_position2"] = 2;
  attribute_handles_["color"] = 3;
  attribute_handles_["tex_coord"] = 4;
  attribute_handles_["bezier_coord"] = 5;
  // Particle attributes. These are only used with particle programs so we could give them there own set of numbers.
  attribute_handles_["velocity"] = 6;
  attribute_handles_["color"] = 7;
  attribute_handles_["age"] = 8;
  attribute_handles_["visible"] = 9;

  Shader general_vert, animated_vert, textured_frag, textured_with_shadows_frag, minimal_frag,
    quadric_frag, circles_frag, circles_screen_textured_frag, shadows_vert, shadows_frag,
    particle_feedback_vert, particle_draw_vert, particle_draw_geom, particle_draw_frag;
  general_vert.load("src/engine/shaders/general.vert", GL_VERTEX_SHADER);
  animated_vert.load("src/engine/shaders/animated.vert", GL_VERTEX_SHADER);
  textured_frag.load("src/engine/shaders/textured.frag", GL_FRAGMENT_SHADER);
  textured_with_shadows_frag.load("src/engine/shaders/textured_with_shadows.frag", GL_FRAGMENT_SHADER);
  minimal_frag.load("src/engine/shaders/minimal.frag", GL_FRAGMENT_SHADER);
  quadric_frag.load("src/engine/shaders/quadric_anti_aliased.frag", GL_FRAGMENT_SHADER);
  circles_frag.load("src/engine/shaders/circles_anti_aliased.frag", GL_FRAGMENT_SHADER);
  circles_screen_textured_frag.load("src/engine/shaders/circles_screen_textured.frag", GL_FRAGMENT_SHADER);
  shadows_vert.load("src/engine/shaders/shadows.vert", GL_VERTEX_SHADER);
  shadows_frag.load("src/engine/shaders/shadows.frag", GL_FRAGMENT_SHADER);
  particle_feedback_vert.load("src/engine/shaders/particle_feedback.vert", GL_VERTEX_SHADER);
  particle_draw_vert.load("src/engine/shaders/particle_draw.vert", GL_VERTEX_SHADER);
  particle_draw_geom.load("src/engine/shaders/particle_draw.geom", GL_GEOMETRY_SHADER);
  particle_draw_frag.load("src/engine/shaders/particle_draw.frag", GL_FRAGMENT_SHADER);
  
  programs_["textured"].init();
  programs_["textured"].addShader(&general_vert);
  programs_["textured"].addShader(&textured_frag);

  programs_["textured_with_shadows"].init();
  programs_["textured_with_shadows"].addShader(&general_vert);
  programs_["textured_with_shadows"].addShader(&textured_with_shadows_frag);

  programs_["minimal"].init();
  programs_["minimal"].addShader(&general_vert);
  programs_["minimal"].addShader(&minimal_frag);
  
  programs_["minimal_animated"].init();
  programs_["minimal_animated"].addShader(&animated_vert);
  programs_["minimal_animated"].addShader(&minimal_frag);

  programs_["quadric"].init();
  programs_["quadric"].addShader(&general_vert);
  programs_["quadric"].addShader(&quadric_frag);
  
  programs_["quadric_animated"].init();
  programs_["quadric_animated"].addShader(&animated_vert);
  programs_["quadric_animated"].addShader(&quadric_frag);

  programs_["circles"].init();
  programs_["circles"].addShader(&general_vert);
  programs_["circles"].addShader(&circles_frag);
  
  programs_["circles_screen_textured"].init();
  programs_["circles_screen_textured"].addShader(&general_vert);
  programs_["circles_screen_textured"].addShader(&circles_screen_textured_frag);

  programs_["shadows"].init();
  programs_["shadows"].addShader(&shadows_vert);
  programs_["shadows"].addShader(&shadows_frag);

  programs_["particle_feedback"].init();
  programs_["particle_feedback"].addShader(&particle_feedback_vert);
  const GLchar* varyings[5];
  varyings[0] = "feedback_position";
  varyings[1] = "feedback_velocity";
  varyings[2] = "feedback_color";
  varyings[3] = "feedback_age";
  varyings[4] = "feedback_visible";
  glTransformFeedbackVaryings(programs_["particle_feedback"].handle(), 5, varyings, GL_INTERLEAVED_ATTRIBS);

  programs_["particle_draw"].init();
  programs_["particle_draw"].addShader(&particle_draw_vert);
  programs_["particle_draw"].addShader(&particle_draw_geom);
  programs_["particle_draw"].addShader(&particle_draw_frag);

  setAttributesAndLink();
  setTextureUnits();
}

void Engine::setAttributesAndLink() {
  for (map<string, Program>::iterator program_it = programs_.begin(); program_it != programs_.end(); ++program_it) {
    // Keep our vertex attributes in a consistent location accross programs.
    // This way we can VAOs with different programs without worrying.
    for (map<string, GLuint>::iterator attr_it = attribute_handles_.begin(); attr_it != attribute_handles_.end(); ++attr_it) {
      program_it->second.setAttributeHandle(attr_it->first, attr_it->second);
    }
    program_it->second.link();
  }
}

void Engine::setTextureUnits() {
  useProgram("textured");
  glUniform1i(uniformHandle("color_texture"), 0);

  useProgram("circles_screen_textured");
  glUniform1i(uniformHandle("color_texture"), 0);

  useProgram("textured_with_shadows");
  glUniform1i(uniformHandle("color_texture"), 0);
  glUniform1i(uniformHandle("shadow_texture"), 1);

  useProgram("shadows");
  glUniform1i(uniformHandle("occluder_texture"), 0);

  useProgram("particle_draw");
  glUniform1i(uniformHandle("color_texture"), 0);
}

void Engine::draw() {
  // 2D rendering modelview
  glm::mat3 view(1.0f);
  view = translate2D(view, glm::vec2(-1.0f, -1.0f));
  view = scale2D(view, glm::vec2(2.0f/aspect_, 2.0f));
  view = translate2D(view, glm::vec2(-left_of_window_, 0.0f));
  root_entity_.setRelativeTransform(view);

  vector<Entity *> draw2D;
  root_entity_.getSortedVisibleDescendants(&draw2D);

  // Draw occluders to texture.
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glViewport(0,0,width_, height_);
  glBindFramebuffer(GL_FRAMEBUFFER, occluder_frame_buffer_);
  glViewport(0,0,width_/2, height_/2);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_FALSE);
  for (vector<Entity *>::iterator it = draw2D.begin(); it != draw2D.end(); ++it) {
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

  for (vector<Entity *>::iterator it = draw2D.begin(); it != draw2D.end(); ++it) {
    if ((*it)->isVisible()) (*it)->draw();
  }

  if (do_stencil_) {
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    for (vector<Entity *>::iterator it = draw2D.begin(); it != draw2D.end(); ++it) {
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

// TODO !!!!
// void Engine::addDrawable3D(Drawable *object) {
//   draw3D_.push_back(object);
// }

// void Engine::removeDrawable3D(Drawable *object) {
//   vector<Drawable *>::iterator it;
//   for (it = draw3D_.begin(); it != draw3D_.end(); ++it) {
//     if (*it == object) {
//       draw3D_.erase(it);
//       return;
//     }
//   }
// }

void Engine::useProgram(string program) {
  if (programs_.count(program) == 0) error("No such program %s. Set it up in engine.\n", program.c_str());
  current_program_ = &programs_[program];
  current_program_->use();
}

GLuint Engine::uniformHandle(string uniform) {
  return current_program_->uniformHandle(uniform);
}

GLuint Engine::attributeHandle(string attribute) {
  return attribute_handles_[attribute];
}

GLuint Engine::getTexture(string filename) {
  if (textures_.count(filename) == 0) {
    textures_[filename] = gli::createTexture2D(filename);
  }
  return textures_[filename];
}
