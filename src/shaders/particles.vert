#version 150

uniform mat3 modelview;
uniform float size;

in vec4 color;
in vec4 position;
in vec2 translate;
in vec2 tex_coords;

out vec4 frag_color;
out vec2 frag_tex_coords;

void main()
{
  frag_color = color;
  frag_tex_coords = tex_coords;
  vec2 screen_pos = (modelview * vec3(position.xy * size + translate, 1.0)).xy;
  gl_Position = vec4(screen_pos, 0.0, 1.0);
}