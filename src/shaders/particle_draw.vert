#version 330

in vec3 position;
in vec4 color;

out vec4 geom_color;

void main()
{
  geom_color = color;
  gl_Position = vec4(position, 1.0);
}