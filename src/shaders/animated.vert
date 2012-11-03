#version 330

uniform mat3 modelview;
uniform float key_mix1 = 0.0;
uniform float key_mix2 = 0.0;

in vec2 key1;
in vec2 key2;
in vec2 key3;
in vec2 tex_coord;
in vec2 bezier_coord;

out vec2 frag_tex_coord;
out vec2 frag_bezier_coord;
out vec2 screen_tex_coord;

void main()
{
  frag_tex_coord = tex_coord;
  frag_bezier_coord = bezier_coord;
  vec2 temp = mix(key1, key2, key_mix1);
  vec2 position = mix(temp, key3, key_mix2);
  vec2 screen_pos = (modelview * vec3(position, 1.0)).xy;
  screen_tex_coord = (screen_pos + vec2(1.0))/2.0;
  gl_Position = vec4(screen_pos, 0.0, 1.0);
}