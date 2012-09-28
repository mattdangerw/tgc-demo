#version 150

uniform mat3 modelview;

in vec4 position;

void main()
{
  vec2 screen_pos = (modelview * vec3(position.xy, 1.0)).xy;
  gl_Position = vec4(screen_pos, 0.0, 1.0);
}