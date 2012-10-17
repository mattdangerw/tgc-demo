#version 330

uniform sampler2D color_texture;
uniform vec4 color_mask = vec4(1.0, 1.0, 1.0, 1.0);

in vec2 frag_tex_coord;
in vec2 screen_tex_coord;

out vec4 out_color;

void main()
{
  out_color = color_mask * texture(color_texture, frag_tex_coord);
}
