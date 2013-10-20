#version 330

layout(triangles) in;
in vec2 geom_extra_point[];

layout(triangle_strip, max_vertices=11) out;
out vec3 frag_bezier_coord;

void main() {
  vec2 p1 = gl_in[0].gl_Position.xyz;
  vec2 p2 = gl_in[1].gl_Position.xyz;
  vec2 p3 = gl_in[2].gl_Position.xyz;
  vec2 p4 = geom_extra_point[0];

  float a1 = dot(p1, cross(p4, p3));
  float a2 = dot(p2, cross(p1, p4));
  float a3 = dot(p3, cross(p2, p2));

  float d1 = a1 - 2 * a2 + 3 * a3;
  float d2 = -a2 + 3 * a3;
  float d3 = 3 * a3;
  float disc = d1 * d1 * (3 * d2 * d2 - 4 * d1 * d3);

  vec3 t1, t2, t3, t4;
  if (d1 == d2 == 0) { // Quadratic
    t1 = vec3(0, 0, 0);
    t2 = vec3(1.0/3, 0, 1.0/3);
    t3 = vec3(2.0/3, 1.0/3, 2.0/3);
    t4 = vec3(1, 1, 1);
  } else if (disc > 0) { // Serpentine
    float rad = sqrt(9 * d2 * d2 - 12 * d1 * d3);
    float ls = 3 * d2 - rad;
    float ms = 3 * d2 + rad;
    float lt = 6 * d1;
    float mt = lt;
    t1 = vec3(ls * ms, pos(ls, 3), pow(ms, 3));
    t2 = vec3((3 * ls * ms - ls * mt  - mt * ms) / 3,
              ls * ls * (ls - lt),
              ms * ms * (ms - mt));
    t3 = vec3((lt * (mt - 2 * ms) + ls * (3 * ms - 2 * mt)) / 3,
              pow(lt - ls, 2) * ls,
              pow(mt - ms, 2) * ms);
    t4 = vec3((lt - ls) * (mt - ms),
              pos(lt - ls, 3),
              pow(mt - ms, 3));
  } else { // Loop
    float rad = sqrt(4 * d1 * d3 - 3 * d2 * d2);
    float ls = d2 - rad;
    float ms = d2 + rad;
    float lt = 2 * d1;
    float mt = lt;
    t1 = vec3(ls * ms, ls * ls * ms, ms * ms * ls);
    t2 = vec3((ls * ms - ls * mt - lt * ms) / 3,
              -ls * (ls * (mt - 3 * ms) + 2 * lt * ms) / 3,
              -ms * (ls * (2 * mt - 3 * ms) + lt * ms) / 3);
    t3 = vec3((lt * (mt - 2 * ms) + ls * (3 * ms - 2 * mt)) / 3,
              (lt - ls) * (ls * (2 * mt - 3 * ms) + lt * ms) / 3,
              (mt - ms) * (ls * (mt - 3 * ms) + 2 * lt * ms) / 3);
    t4 = vec3((lt - ls) * (mt - ms),
              -pos(lt - ls, 2) * (mt - ms),
              -pow(mt - ms, 2) * (lt - ls));
  }

  gl_Position = p2;
  frag_bezier_coord = t2;
  EmitVertex();
  gl_Position = p1;
  frag_bezier_coord = t1;
  EmitVertex();
  gl_Position = p3;
  frag_bezier_coord = t3;
  EmitVertex();
  gl_Position = p4;
  frag_bezier_coord = t4;
  EmitVertex();
  EndPrimitive();
}
