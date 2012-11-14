#version 330

uniform mat4 transform3D;
uniform mat3 transform2D;
uniform vec3 camera_position;
uniform float particle_radius;

layout(points) in;
in vec4 geom_color[];
in float visible[];

layout(triangle_strip, max_vertices = 4) out;
out vec4 frag_color;
out vec2 frag_tex_coord;

vec4 transform(vec3 position) {
  vec4 projected_position = transform3D * vec4(position, 1.0);
  // We need to homogenize here so we can apply the 2D transform.
  vec2 screen_position = (transform2D * vec3(projected_position.xy / projected_position.w, 1.0)).xy;
  return vec4(screen_position, 0.0, 1.0);
}

void main() {
  if (visible[0] > 0.0) {
    frag_color = geom_color[0];
    vec3 position = gl_in[0].gl_Position.xyz;
    vec3 to_camera = normalize(camera_position - position);
    vec3 right = particle_radius * cross(vec3(0.0, 1.0, 0.0), to_camera);
    vec3 up = particle_radius * cross(to_camera, vec3(1.0, 0.0, 0.0));

    gl_Position = transform(position - right - up);
    frag_tex_coord = vec2(0.0, 0.0);
    EmitVertex();
    gl_Position = transform(position + right - up);
    frag_tex_coord = vec2(1.0, 0.0);
    EmitVertex();
    gl_Position = transform(position - right + up);
    frag_tex_coord = vec2(0.0, 1.0);
    EmitVertex();
    gl_Position = transform(position + right + up);
    frag_tex_coord = vec2(1.0, 1.0);
    EmitVertex();
    EndPrimitive();
  }
}