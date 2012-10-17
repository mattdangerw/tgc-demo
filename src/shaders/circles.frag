#version 330

uniform vec4 color;

in vec2 frag_tex_coord;

out vec4 out_color;

void main()
{
  vec2 dx = dFdx(frag_tex_coord);
  vec2 dy = dFdy(frag_tex_coord);
  if(dot(frag_tex_coord, frag_tex_coord) > 1.0f) discard;
  out_color = color;
}
