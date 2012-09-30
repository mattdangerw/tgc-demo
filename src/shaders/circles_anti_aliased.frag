#version 150

uniform vec4 color;

in vec2 frag_tex_coords;

out vec4 frag_color;

void main()
{
  float x = frag_tex_coords.x;
  float y = frag_tex_coords.y;

  vec2 dx = dFdx(frag_tex_coords);
  vec2 dy = dFdy(frag_tex_coords);
  // Chain rule
  float fx = 2*x*dx.x + 2*y*dx.y;
  float fy = 2*x*dy.x + 2*y*dy.y;
  // Signed distance
  float sd = (x*x + y*y - 1.0f)/sqrt(fx*fx + fy*fy);
  // Linear alpha
  float alpha = 0.5 - sd;
  if (alpha > 1)  // Inside
    frag_color = color; 
  else if (alpha < 0)  // Outside
    discard;
  else  // Near boundary
    frag_color = vec4(color.xyz, alpha);
}
