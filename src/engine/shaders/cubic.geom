#version 330

layout(triangles) in;
in vec4 geom_extra_point[];

layout(triangle_strip, max_vertices=11) out;
out vec3 frag_bezier_coord;

bool inside(vec3 t) {
  return t.x * t.x * t.x - t.y * t.z <= 0;
}

void emit(vec4 p1, vec4 p2, vec4 p3, vec4 p4,
          vec3 t1, vec3 t2, vec3 t3, vec3 t4) {
  if (!inside((t1 + t4) / 2.0)) {
    t1.xy *= -1.0;
    t2.xy *= -1.0;
    t3.xy *= -1.0;
    t4.xy *= -1.0;
  }

  if (!inside(t2)) {
    gl_Position = p2;
    frag_bezier_coord = t2;
    EmitVertex();
  }
  gl_Position = p1;
  frag_bezier_coord = t1;
  EmitVertex();
  if (!inside(t3)) {
    gl_Position = p3;
    frag_bezier_coord = t3;
    EmitVertex();
  }
  gl_Position = p4;
  frag_bezier_coord = t4;
  EmitVertex();
  EndPrimitive();
}


void main() {
  vec4 p1 = gl_in[0].gl_Position;
  vec4 p2 = gl_in[1].gl_Position;
  vec4 p3 = gl_in[2].gl_Position;
  vec4 p4 = geom_extra_point[0];

  bool flip = false;
  bool subdivide = false;
  float subdivide_t = 0;

  // Maths for cubic classification
  float a1 = dot(p1.xyw, cross(p4.xyw, p3.xyw));
  float a2 = dot(p2.xyw, cross(p1.xyw, p4.xyw));
  float a3 = dot(p3.xyw, cross(p2.xyw, p1.xyw));
  float a4 = dot(p4.xyw, cross(p3.xyw, p2.xyw));
  float d1 = a1 - 2 * a2 + 3 * a3;
  float d2 = -a2 + 3 * a3;
  float d3 = 3 * a3;
  float disc = d1 * d1 * (3 * d2 * d2 - 4 * d1 * d3);

  // Classify and determine implicit bezier coordinates
  vec3 t1, t2, t3, t4;
  if (d1 == 0 && d2 == 0) { // Quadratic
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
    t1 = vec3(ls * ms, pow(ls, 3), pow(ms, 3));
    t2 = vec3((3 * ls * ms - ls * mt  - mt * ms) / 3,
              ls * ls * (ls - lt),
              ms * ms * (ms - mt));
    t3 = vec3((lt * (mt - 2 * ms) + ls * (3 * ms - 2 * mt)) / 3,
              pow(lt - ls, 2) * ls,
              pow(mt - ms, 2) * ms);
    t4 = vec3((lt - ls) * (mt - ms),
              -1 * pow(lt - ls, 3),
              -1 * pow(mt - ms, 3));
  } else { // Loop
    float rad = sqrt(4 * d1 * d3 - 3 * d2 * d2);
    float ls = d2 - rad;
    float ms = d2 + rad;
    float lt = 2 * d1;
    float mt = lt;
    t1 = vec3(ls * ms, ls * ls * ms, ms * ms * ls);
    t2 = vec3((3 * ls * ms - ls * mt - lt * ms) / 3,
              -ls * (ls * (mt - 3 * ms) + 2 * lt * ms) / 3,
              -ms * (ls * (2 * mt - 3 * ms) + lt * ms) / 3);
    t3 = vec3((lt * (mt - 2 * ms) + ls * (3 * ms - 2 * mt)) / 3,
              (lt - ls) * (ls * (2 * mt - 3 * ms) + lt * ms) / 3,
              (mt - ms) * (ls * (mt - 3 * ms) + 2 * lt * ms) / 3);
    t4 = vec3((lt - ls) * (mt - ms),
              -pow(lt - ls, 2) * (mt - ms),
              -pow(mt - ms, 2) * (lt - ls));
    float double_point = ls / lt;
    if (double_point >= 0.0 && double_point <= 1.0) {
      subdivide = true;
      subdivide_t = double_point;
    }
    double_point = ms / mt;
    if (double_point >= 0.0 && double_point <= 1.0) {
      subdivide = true;
      subdivide_t = double_point;
    }
  }

  // if (crosses p1 p2 line) {
  //   subdivide at crossing
  // }

  if (!subdivide) {
    emit(p1, p2, p3, p4, t1, t2, t3, t4);
  } else {
    // subdivide the curve
    vec4 p12, p23, p34, p13, p24, p14;
    p12 = mix(p1, p2, subdivide_t);
    p23 = mix(p2, p3, subdivide_t);
    p34 = mix(p3, p4, subdivide_t);
    p13 = mix(p12, p23, subdivide_t);
    p24 = mix(p23, p34, subdivide_t);
    p14 = mix(p13, p24, subdivide_t);
    vec3 t12, t23, t34, t13, t24, t14;
    t12 = mix(t1, t2, subdivide_t);
    t23 = mix(t2, t3, subdivide_t);
    t34 = mix(t3, t4, subdivide_t);
    t13 = mix(t12, t23, subdivide_t);
    t24 = mix(t23, t34, subdivide_t);
    t14 = mix(t13, t24, subdivide_t);
    
    // First subdivision
    emit(p1, p12, p13, p14, t1, t12, t13, t14);

    // Second subdivision
    emit(p14, p24, p34, p4, t14, t24, t34, t4);

    // Middle triangle. This bezier coord will make our fragment shader fill
    // in for every fragment
    frag_bezier_coord = vec3(0.0, 1.0, 1.0);
    gl_Position = p1;
    EmitVertex();
    gl_Position = p14;
    EmitVertex();
    gl_Position = p4;
    EmitVertex();
    EndPrimitive();
  }
}
