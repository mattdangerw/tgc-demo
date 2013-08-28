#version 330

uniform sampler2D color_texture;
uniform sampler2D shadow_texture;
uniform vec2 tex_scale = vec2(1.0, 1.0);
uniform vec4 color_mul = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 color_add = vec4(0.0, 0.0, 0.0, 0.0);

in vec2 frag_tex_coord;
in vec2 screen_tex_coord;

out vec4 out_color;

void main()
{
  float exposure = texture(shadow_texture, screen_tex_coord).r;
  vec4 exposure_mask = vec4(exposure, exposure, exposure, 1.0);
  out_color = (color_mul * texture(color_texture, frag_tex_coord * tex_scale) + color_add)
    * exposure_mask;
}
