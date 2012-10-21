#version 330

uniform mat4 transform3D;
uniform mat3 transform2D;
uniform float size;

in vec4 color;
in vec2 position;
in vec3 translate;
in vec2 tex_coord;

out vec4 frag_color;
out vec2 frag_tex_coord;

void main()
{
  frag_color = color;
  frag_tex_coord = tex_coord;
  vec3 translated_position = vec3(position * size, 0.0f) + translate;
  vec4 projected_position = transform3D * vec4(translated_position, 1.0);
  // We need to homogenize here so we can apply the 2D transform.
  vec2 screen_pos = (transform2D * vec3(projected_position.xy / projected_position.w, 1.0)).xy;
  gl_Position = vec4(screen_pos, 0.0, 1.0);
}