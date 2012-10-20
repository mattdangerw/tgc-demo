#version 330

uniform mat4 mvp;
uniform float size;

in vec4 color;
in vec4 position;
in vec3 translate;
in vec2 tex_coord;

out vec4 frag_color;
out vec2 frag_tex_coord;

void main()
{
  frag_color = color;
  frag_tex_coord = tex_coord;
  vec3 sized_position = vec3(position.xy * size, 0.0f) + translate;
  gl_Position = mvp * vec4(sized_position.xyz, 1.0);
}