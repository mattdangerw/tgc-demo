#ifndef TEXT_H_
#define TEXT_H_

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "engine/entity.h"
#include "engine/fill.h"
#include <ft2build.h>
#include FT_FREETYPE_H

using std::string;

class Text : public Entity {
  public:
    Text();
    ~Text();
    void init();
    void setFont(string font_file);
    void setText(string text);
    void setLineHeight(float height);
    void extent(glm::vec2 *min, glm::vec2 *max);
    void draw();
    void drawOccluder();

  private:
    void renderLine();
    void drawHelper(bool occluder);
    // Member data.
    FT_Face font_face_;
    string text_;
    float line_height_;
    glm::vec2 render_offset_, render_size_;
    // GL stuff
    GLuint line_frame_buffer_;
    GLuint glyph_texture_, line_texture_;
};

#endif  // TEXT_H_
