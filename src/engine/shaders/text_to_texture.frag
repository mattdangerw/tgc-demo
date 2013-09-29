#version 330

uniform sampler2D color_texture;

in vec2 frag_tex_coord;

out vec4 out_color;

void main()
{
  out_color = texture(color_texture, frag_tex_coord);
}
