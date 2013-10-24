#version 330

uniform mat3 modelview;

in vec2 position;
in vec2 extra_point;

out vec4 geom_extra_point;

void main()
{
  geom_extra_point = vec4((modelview * vec3(extra_point, 1.0)).xy, 0.0, 1.0);
  vec2 screen_pos = (modelview * vec3(position, 1.0)).xy;
  gl_Position = vec4(screen_pos, 0.0, 1.0);
}
