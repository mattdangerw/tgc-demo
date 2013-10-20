#version 330

uniform mat3 modelview;

in vec2 position;
in vec2 extra_point;

out vec2 geom_extra_point;

void main()
{
  geom_extra_point = (modelview * vec3(extra_point, 1.0)).xy;
  vec2 screen_pos = (modelview * vec3(position, 1.0)).xy;
  gl_Position = vec4(screen_pos, 0.0, 1.0);
}
