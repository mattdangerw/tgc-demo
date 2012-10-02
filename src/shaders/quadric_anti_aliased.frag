#version 330

in vec2 frag_tex_coords;

out vec4 out_color;

void main()
{
  float x = frag_tex_coords.x;
  float y = frag_tex_coords.y;

  vec2 dx = dFdx(frag_tex_coords);
  vec2 dy = dFdy(frag_tex_coords);
  // Chain rule
  float fx = 2*x*dx.x - dx.y;
  float fy = 2*x*dy.x - dy.y;
  // Signed distance
  float sd = (x*x - y)/sqrt(fx*fx + fy*fy);
  // Linear alpha
  float alpha = 0.5 - sd;
  if (alpha > 1.0) {  // Inside
    gl_FragDepth = 0.0;
    out_color = vec4(0.0, 0.0, 1.0, 1.0);
  }
  else if (alpha < 0.0) {  // Outside
    gl_FragDepth = 1.0;
    discard;
  }
  else {  // Near boundary
    gl_FragDepth = 0.0;
    out_color = vec4(0.0, 1.0, 0.0, alpha);
  }
}
