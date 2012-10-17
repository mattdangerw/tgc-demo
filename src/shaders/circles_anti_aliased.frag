#version 330

uniform vec4 color;

in vec2 frag_tex_coord;

out vec4 out_color;

void main()
{
  float x = frag_tex_coord.x;
  float y = frag_tex_coord.y;

  vec2 dx = dFdx(frag_tex_coord);
  vec2 dy = dFdy(frag_tex_coord);
  // Chain rule
  float fx = 2*x*dx.x + 2*y*dx.y;
  float fy = 2*x*dy.x + 2*y*dy.y;
  // Signed distance
  float sd = (x*x + y*y - 1.0f)/sqrt(fx*fx + fy*fy);
  // Linear alpha
  float alpha = 0.5 - sd;
  if (alpha > 1) {  // Inside
    gl_FragDepth = 0.0;
    out_color = color;
  }
  else if (alpha < 0) {  // Outside
    gl_FragDepth = 1.0;
    discard;
  }
  else {  // Near boundary
    gl_FragDepth = 0.0;
    out_color = vec4(color.rgb, color.a * alpha);
  }
}
