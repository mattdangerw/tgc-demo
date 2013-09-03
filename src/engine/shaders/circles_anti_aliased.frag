#version 330

in vec2 frag_tex_coord;

out vec4 out_color;

void main()
{
  vec2 normalized = -1.0 + 2.0 * frag_tex_coord;

  float x = normalized.x;
  float y = normalized.y;

  vec2 dx = dFdx(normalized);
  vec2 dy = dFdy(normalized);
  // Chain rule
  float fx = 2*x*dx.x + 2*y*dx.y;
  float fy = 2*x*dy.x + 2*y*dy.y;
  // Signed distance
  float sd = (x*x + y*y - 1.0f)/sqrt(fx*fx + fy*fy);
  // Linear alpha
  float alpha = 0.5 - sd;
  if (alpha > 1) {  // Inside
    gl_FragDepth = 0.0;
    out_color = vec4(1.0, 0.0, 0.0, 1.0);
  }
  else if (alpha < 0) {  // Outside
    gl_FragDepth = 1.0;
    discard;
  }
  else {  // Near boundary
    gl_FragDepth = 0.0;
    out_color = vec4(1.0, 0.0, 0.0, alpha);
  }
}
