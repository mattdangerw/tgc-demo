#version 330

uniform vec3 emitter_position;
uniform vec4 emitter_color;
uniform float emitter_visible;
uniform float delta_time;
uniform float alpha_decay;
uniform float lifetime;

in vec3 position;
in vec3 velocity;
in vec4 color;
in float age;
in float visible;

out vec3 feedback_position;
out vec3 feedback_velocity;
out vec4 feedback_color;
out float feedback_age;
out float feedback_visible;

void main() {
  feedback_position = position + velocity * delta_time;
  feedback_velocity = velocity;
  feedback_color = color;
  feedback_color.a -= delta_time * alpha_decay; 
  feedback_age = age + delta_time;
  feedback_visible = visible;
  if (feedback_age > lifetime) {
    feedback_position = emitter_position;
    // TODO: random velocity.
    feedback_velocity = vec3(0.0);
    feedback_color = emitter_color;
    feedback_age = feedback_age - lifetime;
    feedback_visible = emitter_visible;
  }
}