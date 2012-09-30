#version 150

uniform sampler2D color_texture;
uniform vec4 color_mask = vec4(1.0, 1.0, 1.0, 1.0);

in vec2 frag_tex_coords;

out vec4 frag_color;

void main()
{
  frag_color = color_mask * texture(color_texture, frag_tex_coords);
}
