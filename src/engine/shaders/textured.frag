#version 330

uniform sampler2D color_texture;
uniform vec4 color_mul = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 color_add = vec4(0.0, 0.0, 0.0, 0.0);

in vec2 frag_tex_coord;

out vec4 out_color;

void main()
{
  out_color = color_mul * texture(color_texture, frag_tex_coord) + color_add;
}
