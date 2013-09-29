#include "engine/text.h"

#include FT_GLYPH_H

#include "engine/engine.h"
#include "util/transform2D.h"
#include "util/error.h"

static const string DEFAULT_FONT = "content/fonts/im-fell.otf";
static FT_Library ft = NULL;
static map<string, FT_Face> loaded_font_faces;
static FT_Face loadIfNeeded(string fontfile) {
  if (loaded_font_faces.count(fontfile) == 0) {
    if (FT_New_Face(ft, fontfile.c_str(), 0, &loaded_font_faces[fontfile])) {
      error("Could not load font file %s\n", fontfile.c_str());
    }
  }
  return loaded_font_faces[fontfile];
}

Text::Text()
  : line_height_(0.2f) {}

Text::~Text() {}

void Text::init() {
  if (ft == NULL) {
    if (FT_Init_FreeType(&ft)) {
      error("Could not init freetype library\n");
    }
  }
  font_face_ = loadIfNeeded(DEFAULT_FONT);

  // Set up textures
  GLuint textures[2];
  glGenTextures(2, textures);
  for (int i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, textures[i]);
    // Clamping to edges is important to prevent artifacts when scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Linear filtering usually looks best for text
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  }
  glyph_texture_ = textures[0];
  line_texture_ = textures[1];

  // Set up frame buffer
  glGenFramebuffers(1, &line_frame_buffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, line_frame_buffer_);
  glBindTexture(GL_TEXTURE_2D, line_texture_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, line_texture_, 0);
}

void Text::setFont(string font_file) {
  font_face_ = loadIfNeeded(font_file);
}

void Text::setText(string text) {
  text_ = text;
  renderLine();
}

void Text::setLineHeight(float height) {
  line_height_ = height;
  renderLine();
}

void Text::extent(glm::vec2 *min, glm::vec2 *max) {
  *min = render_offset_;
  *max = render_offset_ + render_size_;
}

void Text::draw() {
  drawHelper(false);
}

void Text::drawOccluder() {
  drawHelper(true);
}

void Text::drawHelper(bool occluder) {
  glEnable(GL_STENCIL_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilFunc(GL_ALWAYS, 0, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
  theEngine().useProgram("text_stencil");
  glEnable(GL_DEPTH_TEST);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, line_texture_);
  // Calculate the modelview transform
  glm::mat3 modelview(1.0f);
  modelview = translate2D(modelview, render_offset_);
  modelview = scale2D(modelview, render_size_);
  glUniformMatrix3fv(theEngine().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(fullTransform() * modelview));
  theEngine().drawUnitQuad();

  // Fill in
  glDisable(GL_DEPTH_TEST);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
  glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
  if (occluder) {
    fill()->fillInOccluder(this);
  } else {
    fill()->fillIn(this);
  }
  glDisable(GL_STENCIL_TEST);
}

void Text::renderLine() {
  int pixel_line_height = (int)theEngine().getPixelHeight(line_height_);
  FT_Set_Pixel_Sizes(font_face_, pixel_line_height, pixel_line_height);

  // First pass load up glyphs calculate kerning
  // Adapted from http://www.freetype.org/freetype2/docs/tutorial/step2.html
  int x = 0, y = 0, num_glyphs  = 0;
  bool use_kerning = FT_HAS_KERNING(font_face_) != 0;
  FT_UInt previous = 0;
  FT_Glyph *glyphs = new FT_Glyph[text_.length()];
  for (const char *char_pointer = text_.c_str(); *char_pointer; char_pointer++) {
    // Convert character code to glyph index
    FT_UInt glyph_index = FT_Get_Char_Index(font_face_, *char_pointer);
    // Retrieve kerning distance and move pen position
    if (use_kerning && previous && glyph_index)
    {
      FT_Vector delta;
      FT_Get_Kerning(font_face_, previous, glyph_index,
                      FT_KERNING_DEFAULT, &delta);
      x += delta.x >> 6;
      y += delta.y >> 6;
    }
    // Load glyph image into the slot without rendering
    if (FT_Load_Glyph(font_face_, glyph_index, FT_LOAD_DEFAULT)) {
      continue;
    }
    // Extract glyph image and store it in our table
    if (FT_Get_Glyph(font_face_->glyph, &glyphs[num_glyphs])) {
      continue;
    }
    // Transform glyph offset in 64ths of pixel
    FT_Vector offset;
    offset.x = x * 64;
    offset.y = y * 64;
    FT_Glyph_Transform(glyphs[num_glyphs], 0, &offset);

    x += font_face_->glyph->advance.x >> 6;
    y += font_face_->glyph->advance.y >> 6;
    previous = glyph_index;
    num_glyphs++;
  }

  // Second pass, calculate bounding box of rendering area
  FT_BBox  bbox, glyph_bbox;
  bbox.xMin = bbox.yMin = std::numeric_limits<int>::max();
  bbox.xMax = bbox.yMax = -std::numeric_limits<int>::max();
  // For each glyph image, compute its bounding box
  for ( int i = 0; i < num_glyphs; i++ )
  {
    FT_Glyph_Get_CBox( glyphs[i], ft_glyph_bbox_pixels,
                       &glyph_bbox );
    if ( glyph_bbox.xMin < bbox.xMin )
      bbox.xMin = glyph_bbox.xMin;
    if ( glyph_bbox.yMin < bbox.yMin )
      bbox.yMin = glyph_bbox.yMin;
    if ( glyph_bbox.xMax > bbox.xMax )
      bbox.xMax = glyph_bbox.xMax;
    if ( glyph_bbox.yMax > bbox.yMax )
      bbox.yMax = glyph_bbox.yMax;
  }
  int line_texture_width = bbox.xMax - bbox.xMin;
  int line_texture_height = bbox.yMax - bbox.yMin;
  render_size_.x = line_height_ * line_texture_width / pixel_line_height;
  render_size_.y = line_height_ * line_texture_height / pixel_line_height;
  render_offset_.x = line_height_ * bbox.xMin / pixel_line_height;
  render_offset_.y = line_height_ * bbox.yMin / pixel_line_height;

  // Set up GL to render to line texture
  glBindTexture(GL_TEXTURE_2D, line_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, line_texture_width, line_texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);  
  glBindFramebuffer(GL_FRAMEBUFFER, line_frame_buffer_);
  glViewport(0, 0, line_texture_width, line_texture_height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  theEngine().useProgram("text_to_texture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, glyph_texture_);

  // Final pass render to texture
  for ( int i = 0; i < num_glyphs; i++ ) {
    // Render the bitmap
    if (FT_Glyph_To_Bitmap(&glyphs[i], FT_RENDER_MODE_NORMAL, NULL, 0)) {
      continue;
    }
    FT_BitmapGlyph bitglyph = (FT_BitmapGlyph)glyphs[i];

    // Upload the bitmap, which contains an 8-bit grayscale image, as an 1 channel texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitglyph->bitmap.width, bitglyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, bitglyph->bitmap.buffer);
    glm::mat3 modelview(1.0f);
    // Translate from the opengl screen coords to pixel coord system
    modelview = translate2D(modelview, glm::vec2(-1.0f, -1.0f));
    modelview = scale2D(modelview, glm::vec2(2.0f / line_texture_width, 2.0f / line_texture_height));
    modelview = translate2D(modelview, glm::vec2(bitglyph->left - bbox.xMin, bitglyph->top - bbox.yMin));
    // Freetype coordinates start at topleft instead of bottom left, so we need to flip y
    modelview = scale2D(modelview, glm::vec2(bitglyph->bitmap.width, -1.0f * bitglyph->bitmap.rows));
    // Draw the character on the screen
    glUniformMatrix3fv(theEngine().uniformHandle("modelview"), 1, GL_FALSE, glm::value_ptr(modelview));
    theEngine().drawUnitQuad();

    FT_Done_Glyph(glyphs[i]);
  }
  delete[] glyphs;
}
