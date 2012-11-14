#version 330

in vec3 position;
in vec4 color;
in float visible;

out vec4 geom_color;
out float geom_visible;

void main()
{
  geom_visible = visible;
  geom_color = color;
  gl_Position = vec4(position, 1.0);
}