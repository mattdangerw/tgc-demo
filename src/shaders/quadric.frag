#version 150

in vec2 frag_tex_coords;

out vec4 frag_color;

void main()
{
  float x = frag_tex_coords.x;
  float y = frag_tex_coords.y;
  // Signed distance
  float sd = x*x - y;  
  if (sd > 0)
    discard;
  frag_color = vec4(1.0, 0.0, 0.0, 1.0);
}
