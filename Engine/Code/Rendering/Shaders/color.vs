#version 150

in vec3 in_position;
in vec4 vertex_color;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_proj;

out vec4 fragment_color;

void main() {
	fragment_color = vertex_color;
	gl_Position = in_proj * in_view * in_model * vec4(in_position, 1.0);
}