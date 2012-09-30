#version 150

uniform sampler2D color_texture;

in vec2 frag_tex_coords;
in vec4 frag_color;

out vec4 out_color;

void main()
{
  out_color = frag_color * texture(color_texture, frag_tex_coords);
}
