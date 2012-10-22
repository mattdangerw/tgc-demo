#version 330

in vec2 frag_bezier_coord;

out vec4 out_color;

void main()
{
  float x = frag_bezier_coord.x;
  float y = frag_bezier_coord.y;
  // Signed distance
  float sd = x*x - y;  
  if (sd > 0)
    discard;
  out_color = vec4(1.0, 0.0, 0.0, 1.0);
}
