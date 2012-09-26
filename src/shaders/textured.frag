#version 150

uniform sampler2D color_texture;

in vec2 frag_tex_coords;

void main()
{
  gl_FragColor = texture(color_texture, frag_tex_coords);
}
