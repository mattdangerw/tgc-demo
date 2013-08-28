#version 330

uniform vec4 color;

in vec2 frag_tex_coord;

out vec4 out_color;

void main()
{
  if(dot(frag_tex_coord, frag_tex_coord) > 1.0f) discard;
  out_color = vec4(1.0, 0.0, 0.0, 1.0);
}
