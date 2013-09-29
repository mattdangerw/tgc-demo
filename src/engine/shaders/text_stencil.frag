#version 330

uniform sampler2D color_texture;

in vec2 frag_tex_coord;

out vec4 out_color;

void main()
{
  out_color.a = texture(color_texture, frag_tex_coord).r;
  if (out_color.a == 0.0) discard;
  out_color.r = 1.0;
}
