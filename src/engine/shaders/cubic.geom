#version 330

layout(triangles) in;
in vec4 geom_extra_point[];

layout(triangle_strip, max_vertices=18) out;
out vec3 frag_bezier_coord;

// If we have non zero ws ever, use the actual dot cross.
float area(vec4 p1, vec4 p2, vec4 p3)
{
  return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool inside_curve(vec3 b) {
  return b.x * b.x * b.x - b.y * b.z <= 0;
}

// Emits the hull of a cubic curve with bezier coordinates. Handles flipping
// the curve so that the p1-p4 line is always inside. Also makes sure no
// portion of the curve is drawn twice.
void emit(vec4 p1, vec4 p2, vec4 p3, vec4 p4,
          vec3 b1, vec3 b2, vec3 b3, vec3 b4) {
  if (!inside_curve((b1 + b2 + b3 + b4) / 4.0)) {
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
  gl_Position = p2;
  frag_bezier_coord = b2;
  EmitVertex();
  gl_Position = p3;
  frag_bezier_coord = b3;
  EmitVertex();
  EndPrimitive();
}

// Subdivides the cubic curve. Emits the first subdivision and the fill
// triangle directly. Sets the output p1-p4 and b1-b4 to the second
// subdivision hull.
void subdivide(inout vec4 p1, inout vec4 p2, inout vec4 p3, vec4 p4,
               inout vec3 b1, inout vec3 b2, inout vec3 b3, vec3 b4,
               float t) {
  vec4 p12, p23, p34, p123, p234, p1234;
  p12 = mix(p1, p2, t);
  p23 = mix(p2, p3, t);
  p34 = mix(p3, p4, t);
  p123 = mix(p12, p23, t);
  p234 = mix(p23, p34, t);
  p1234 = mix(p123, p234, t);
  vec3 b12, b23, b34, b123, b234, b1234;
  b12 = mix(b1, b2, t);
  b23 = mix(b2, b3, t);
  b34 = mix(b3, b4, t);
  b123 = mix(b12, b23, t);
  b234 = mix(b23, b34, t);
  b1234 = mix(b123, b234, t);

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

  // First subdivision
  emit(p1, p12, p123, p1234, b1, b12, b123, b1234);

  // Set our new hull to be the second subdivision. This way we can "recurse"
  // into subdivide
  p1 = p1234;
  p2 = p234;
  p3 = p34;
  b1 = b1234;
  b2 = b234;
  b3 = b34;
}

void main() {
  vec4 p1 = gl_in[0].gl_Position;
  vec4 p2 = gl_in[1].gl_Position;
  vec4 p3 = gl_in[2].gl_Position;
  vec4 p4 = geom_extra_point[0];

  // Maths for cubic classification
  float a1 = area(p1, p4, p3);
  float a2 = area(p2, p1, p4);
  float a3 = area(p3, p2, p1);
  // Normalize the areas to sum to one. This keeps what follows numerically
  // stable. Otherwise tiny tiny bezier coordinate values by the time you hit
  // fragment shader
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
  float rad, ls, lt, ms, mt;
  if (d1 == 0 && d2 == 0) { // Quadratic
    ls = 2;
    lt = 1;
    ms = 2;
    mt = 1;
    b1 = vec3(0, 0, 0);
    b2 = vec3(1.0/3, 0, 1.0/3);
    b3 = vec3(2.0/3, 1.0/3, 2.0/3);
    b4 = vec3(1, 1, 1);
  } else if (disc > 0) { // Serpentine
    rad = sqrt(9 * d2 * d2 - 12 * d1 * d3);
    ls = 3 * d2 - rad;
    ms = 3 * d2 + rad;
    lt = 6 * d1;
    mt = lt;
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
    rad = sqrt(4 * d1 * d3 - 3 * d2 * d2);
    ls = d2 - rad;
    ms = d2 + rad;
    lt = 2 * d1;
    mt = lt;
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
  }

  float subdivide_l = ls / lt;
  float subdivide_m = ms / mt;
  if (subdivide_l <= 0) subdivide_l = 2;
  if (subdivide_m <= 0) subdivide_m = 2;
  float t1 = min(subdivide_l, subdivide_m);
  float t2 = (max(subdivide_l, subdivide_m) - t1) / (1 - t1);
  if (t1 < 1) {
    subdivide(p1, p2, p3, p4, b1, b2, b3, b4, t1);
    if (t2 < 1) {
      subdivide(p1, p2, p3, p4, b1, b2, b3, b4, t2);
    }
  }
  emit(p1, p2, p3, p4, b1, b2, b3, b4);
}
