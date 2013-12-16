#version 150

in vec3 in_position;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_proj;

void main() {
	gl_Position = in_proj * in_view * in_model * vec4(in_position, 1.0);
}