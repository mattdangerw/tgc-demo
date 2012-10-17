#version 330

uniform sampler2D occluder_texture;
uniform vec2 light_position;

uniform float density;
uniform float decay_rate;
uniform float scale_factor;
uniform float constant_factor;

in vec2 frag_tex_coord;

out vec4 out_color;

bool occluder(float tex_color) {
  return tex_color < 1.0;
}

vec2 sample_dest() {
  float frac_to_top = (1.0 - frag_tex_coord.y) / (light_position.y - frag_tex_coord.y);
  return mix(frag_tex_coord, light_position, frac_to_top);
}

void main()
{
  float tex_color = texture(occluder_texture, frag_tex_coord).r;
  vec2 curr_tex_coord = frag_tex_coord;
  vec2 delta_tex_coord = frag_tex_coord - sample_dest();
  delta_tex_coord /= 32 * density;
  float decay = 1.0;
  out_color = vec4(tex_color);
  for(int i = 0; i < 32; i++) {
    curr_tex_coord -= delta_tex_coord;
    vec4 sample = vec4(texture(occluder_texture, curr_tex_coord).r);
    sample *= decay;
    out_color += sample;
    decay *= decay_rate;
  }
  out_color = out_color * scale_factor + constant_factor;
}
