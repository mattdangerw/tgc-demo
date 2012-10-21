#version 330

uniform mat3 modelview;

in vec2 position;
in vec2 tex_coord;

out vec2 frag_tex_coord;
out vec2 screen_tex_coord;

void main()
{
  frag_tex_coord = tex_coord;
  vec2 screen_pos = (modelview * vec3(position, 1.0)).xy;
  screen_tex_coord = (screen_pos + vec2(1.0))/2.0;
  gl_Position = vec4(screen_pos, 0.0, 1.0);
}