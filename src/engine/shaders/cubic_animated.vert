#version 330

uniform mat3 modelview;
uniform float lerp_t1 = 0.0;
uniform float lerp_t2 = 0.0;

in vec2 position;
in vec2 lerp_position1;
in vec2 lerp_position2;
in vec2 extra_point;
in vec2 lerp_extra_point1;
in vec2 lerp_extra_point2;

out vec4 geom_extra_point;

void main()
{
  vec2 animated_extra = mix(extra_point, lerp_extra_point1, lerp_t1);
  animated_extra = mix(animated_extra, lerp_extra_point2, lerp_t2);
  geom_extra_point = vec4((modelview * vec3(animated_extra, 1.0)).xy, 0.0, 1.0);
  vec2 animated_position = mix(position, lerp_position1, lerp_t1);
  animated_position = mix(animated_position, lerp_position2, lerp_t2);
  vec2 screen_pos = (modelview * vec3(animated_position, 1.0)).xy;
  gl_Position = vec4(screen_pos, 0.0, 1.0);
}
