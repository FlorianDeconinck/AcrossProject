#version 150

in vec3 in_position;
in vec4 vertex_color;
in vec3 in_normal;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_proj;

out vec4 fragment_color;
out vec3 fragment_normal;

void main() {
	fragment_color = vertex_color;
	fragment_normal = in_normal;
	gl_Position = in_proj * in_view * in_model * vec4(in_position, 1.0);
}