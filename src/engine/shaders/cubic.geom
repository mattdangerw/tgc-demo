#version 330

layout(triangles) in;
in vec4 geom_extra_point[];

layout(triangle_strip, max_vertices=11) out;
out vec3 frag_bezier_coord;

float area(vec4 p1, vec4 p2, vec4 p3)
{
  return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool inside_curve(vec3 b) {
  return b.x * b.x * b.x - b.y * b.z <= 0;
}

bool inside_triangle(vec4 test, vec4 p1, vec4 p2, vec4 p3) {
  bool s1, s2, s3;

  s1 = area(test, p1, p2) < 0.0f;
  s2 = area(test, p2, p3) < 0.0f;
  s3 = area(test, p3, p1) < 0.0f;

  return (s1 == s2) && (s2 == s3);
}

// Emits the convex hull of a cubic curve. Handles flipping the curve so that
// the p1-p4 line is always inside. Also makes sure no portion of the curve is
// drawn twice.
void emit(vec4 p1, vec4 p2, vec4 p3, vec4 p4,
          vec3 b1, vec3 b2, vec3 b3, vec3 b4) {
  if (!inside_curve((b1 + b4) / 2.0)) {
    b1.xy *= -1.0;
    b2.xy *= -1.0;
    b3.xy *= -1.0;
    b4.xy *= -1.0;
  }

  gl_Position = p1;
  frag_bezier_coord = b1;
  EmitVertex();
  gl_Position = p4;
  frag_bezier_coord = b4;
  EmitVertex();
  // If our convex hull is a triangle, emit only the triangle. Doubling up on
  // portions of the curve would actually cut off parts of the shape, because
  // of our stencil trick
  if (!inside_triangle(p2, p1, p3, p4)) {
    gl_Position = p2;
    frag_bezier_coord = b2;
    EmitVertex();
  }
  if (!inside_triangle(p3, p1, p2, p4)) {
    gl_Position = p3;
    frag_bezier_coord = b3;
    EmitVertex();
  }
  EndPrimitive();
}

// Subdivides our control hull into two smaller ones at subdivide_t. Then
// emits these smaller curves along with a triangle for the space in the
// middle
void subdivide_and_emit(vec4 p1, vec4 p2, vec4 p3, vec4 p4,
               vec3 b1, vec3 b2, vec3 b3, vec3 b4,
               float subdivide_t) {
  vec4 p12, p23, p34, p123, p234, p1234;
  p12 = mix(p1, p2, subdivide_t);
  p23 = mix(p2, p3, subdivide_t);
  p34 = mix(p3, p4, subdivide_t);
  p123 = mix(p12, p23, subdivide_t);
  p234 = mix(p23, p34, subdivide_t);
  p1234 = mix(p123, p234, subdivide_t);
  vec3 b12, b23, b34, b123, b234, b1234;
  b12 = mix(b1, b2, subdivide_t);
  b23 = mix(b2, b3, subdivide_t);
  b34 = mix(b3, b4, subdivide_t);
  b123 = mix(b12, b23, subdivide_t);
  b234 = mix(b23, b34, subdivide_t);
  b1234 = mix(b123, b234, subdivide_t);
  
  // First subdivision
  emit(p1, p12, p123, p1234, b1, b12, b123, b1234);

  // Second subdivision
  emit(p1234, p234, p34, p4, b1234, b234, b34, b4);

  // Middle triangle. This bezier coord will make our fragment shader fill
  // in for every fragment
  frag_bezier_coord = vec3(0.0, 1.0, 1.0);
  gl_Position = p1;
  EmitVertex();
  gl_Position = p1234;
  EmitVertex();
  gl_Position = p4;
  EmitVertex();
  EndPrimitive();
}

void main() {
  vec4 p1 = gl_in[0].gl_Position;
  vec4 p2 = gl_in[1].gl_Position;
  vec4 p3 = gl_in[2].gl_Position;
  vec4 p4 = geom_extra_point[0];

  bool do_subdivide = false;
  float subdivide_t = 0;

  // Maths for cubic classification
  float a1 = area(p1, p4, p3);
  float a2 = area(p2, p1, p4);
  float a3 = area(p3, p2, p1);
  // Normalize the areas to sum to one. This keeps what follows numerically
  // stable. Otherwise tiny tiny klm values by the time you hit fragment shader
  float sum = a1 + a2 + a3;
  a1 /= sum;
  a2 /= sum;
  a3 /= sum;
  float d1 = a1 - 2 * a2 + 3 * a3;
  float d2 = -a2 + 3 * a3;
  float d3 = 3 * a3;
  float disc = d1 * d1 * (3 * d2 * d2 - 4 * d1 * d3);

  // Classify and determine implicit bezier coordinates
  vec3 b1, b2, b3, b4;
  if (d1 == 0 && d2 == 0) { // Quadratic
    b1 = vec3(0, 0, 0);
    b2 = vec3(1.0/3, 0, 1.0/3);
    b3 = vec3(2.0/3, 1.0/3, 2.0/3);
    b4 = vec3(1, 1, 1);
  } else if (disc > 0) { // Serpentine
    float rad = sqrt(9 * d2 * d2 - 12 * d1 * d3);
    float ls = 3 * d2 - rad;
    float ms = 3 * d2 + rad;
    float lt = 6 * d1;
    float mt = lt;
    b1 = vec3(ls * ms, pow(ls, 3), pow(ms, 3));
    b2 = vec3((3 * ls * ms - ls * mt  - mt * ms) / 3,
              ls * ls * (ls - lt),
              ms * ms * (ms - mt));
    b3 = vec3((lt * (mt - 2 * ms) + ls * (3 * ms - 2 * mt)) / 3,
              pow(lt - ls, 2) * ls,
              pow(mt - ms, 2) * ms);
    b4 = vec3((lt - ls) * (mt - ms),
              -1 * pow(lt - ls, 3),
              -1 * pow(mt - ms, 3));
  } else { // Loop
    float rad = sqrt(4 * d1 * d3 - 3 * d2 * d2);
    float ls = d2 - rad;
    float ms = d2 + rad;
    float lt = 2 * d1;
    float mt = lt;
    b1 = vec3(ls * ms, ls * ls * ms, ms * ms * ls);
    b2 = vec3((3 * ls * ms - ls * mt - lt * ms) / 3,
              -ls * (ls * (mt - 3 * ms) + 2 * lt * ms) / 3,
              -ms * (ls * (2 * mt - 3 * ms) + lt * ms) / 3);
    b3 = vec3((lt * (mt - 2 * ms) + ls * (3 * ms - 2 * mt)) / 3,
              (lt - ls) * (ls * (2 * mt - 3 * ms) + lt * ms) / 3,
              (mt - ms) * (ls * (mt - 3 * ms) + 2 * lt * ms) / 3);
    b4 = vec3((lt - ls) * (mt - ms),
              -pow(lt - ls, 2) * (mt - ms),
              -pow(mt - ms, 2) * (lt - ls));
    float double_point = ls / lt;
    if (double_point >= 0.0 && double_point <= 1.0) {
      do_subdivide = true;
      subdivide_t = double_point;
    }
    double_point = ms / mt;
    if (double_point >= 0.0 && double_point <= 1.0) {
      do_subdivide = true;
      subdivide_t = double_point;
    }
  }

  // if (a1 * a2 < 0) {
  //   do_subdivide at crossing
  // }

  if (do_subdivide) {
    subdivide_and_emit(p1, p2, p3, p4, b1, b2, b3, b4, subdivide_t);
  } else {
    emit(p1, p2, p3, p4, b1, b2, b3, b4);
  }
}
