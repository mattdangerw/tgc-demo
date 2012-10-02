#version 330

uniform vec4 color;

in vec2 frag_tex_coords;

out vec4 out_color;

void main()
{
  vec2 dx = dFdx(frag_tex_coords);
  vec2 dy = dFdy(frag_tex_coords);
  if(dot(frag_tex_coords, frag_tex_coords) > 1.0f) discard;
  out_color = color;
}
