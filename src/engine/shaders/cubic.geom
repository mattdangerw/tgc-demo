#version 330

layout(lines_adjacency) in;

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
// the curve so that the p1-p4 line is always inside.
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

// Subdivides the cubic curve if t is in the range (0, 1). Emits the first
// subdivision and the fill triangle directly. Sets the output p1-p4 and b1-b4
// to the second subdivision hull.
void subdivide(inout vec4 p1, inout vec4 p2, inout vec4 p3, vec4 p4,
               inout vec3 b1, inout vec3 b2, inout vec3 b3, vec3 b4,
               float t) {
  if (t <= 0 || t >= 1) return;
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
  vec4 p4 = gl_in[3].gl_Position;

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
  float rad, lu, mu, v;
  if (d1 == 0 && d2 == 0) { // Quadratic
    lu = 2;
    mu = 2;
    v = 1;
    b1 = vec3(0, 0, 0);
    b2 = vec3(1.0/3, 0, 1.0/3);
    b3 = vec3(2.0/3, 1.0/3, 2.0/3);
    b4 = vec3(1, 1, 1);
  } else if (disc > 0) { // Serpentine
    rad = sqrt(9 * d2 * d2 - 12 * d1 * d3);
    lu = 3 * d2 - rad;
    mu = 3 * d2 + rad;
    v = 6 * d1;
    b1 = vec3(lu * mu, pow(lu, 3), pow(mu, 3));
    b2 = vec3((3 * lu * mu - lu * v  - v * mu) / 3,
              lu * lu * (lu - v),
              mu * mu * (mu - v));
    b3 = vec3((v * (v - 2 * mu) + lu * (3 * mu - 2 * v)) / 3,
              pow(v - lu, 2) * lu,
              pow(v - mu, 2) * mu);
    b4 = vec3((v - lu) * (v - mu),
              -pow(v - lu, 3),
              -pow(v - mu, 3));
  } else { // Loop
    rad = sqrt(4 * d1 * d3 - 3 * d2 * d2);
    lu = d2 - rad;
    mu = d2 + rad;
    v = 2 * d1;
    b1 = vec3(lu * mu, lu * lu * mu, mu * mu * lu);
    b2 = vec3((3 * lu * mu - lu * v - v * mu) / 3,
              -lu * (lu * (v - 3 * mu) + 2 * v * mu) / 3,
              -mu * (lu * (2 * v - 3 * mu) + v * mu) / 3);
    b3 = vec3((v * (v - 2 * mu) + lu * (3 * mu - 2 * v)) / 3,
              (v - lu) * (lu * (2 * v - 3 * mu) + v * mu) / 3,
              (v - mu) * (lu * (v - 3 * mu) + 2 * v * mu) / 3);
    b4 = vec3((v - lu) * (v - mu),
              -pow(v - lu, 2) * (v - mu),
              -pow(v - mu, 2) * (v - lu));
  }

  // Subdivide at intersection or inflection points in the interval (0,1).
  float root_l = lu / v;
  float root_m = mu / v;
  float t1 = min(root_l, root_m);
  float t2 = (max(root_l, root_m) - max(t1, 0)) / (1 - max(t1, 0));
  subdivide(p1, p2, p3, p4, b1, b2, b3, b4, t1);
  subdivide(p1, p2, p3, p4, b1, b2, b3, b4, t2);
  emit(p1, p2, p3, p4, b1, b2, b3, b4);
}
