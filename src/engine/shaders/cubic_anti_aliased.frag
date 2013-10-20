#version 330

in vec3 frag_bezier_coord;

out vec4 out_color;

void main()
{
  float x = frag_bezier_coord.x;
  float y = frag_bezier_coord.y;
  float z = frag_bezier_coord.z;
  vec2 dx = dFdx(frag_bezier_coord);
  vec2 dy = dFdy(frag_bezier_coord);

  // Chain rule
  float fx = 3*x*x*dx.x - y*dx.z - z*dx.y;
  float fy = 3*x*x*dy.x - y*dy.z - z*dy.y;
  // Signed distance
  float sd = (x*x*x - y*z)/sqrt(fx*fx + fy*fy);
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
