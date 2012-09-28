#version 150

uniform sampler2D color_texture;
uniform vec4 color_mask = vec4(1.0, 1.0, 1.0, 1.0);

in vec2 frag_tex_coords;

void main()
{
  gl_FragColor = color_mask * texture(color_texture, frag_tex_coords);
}
