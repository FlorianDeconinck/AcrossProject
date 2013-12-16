#version 150

in vec3 in_position;
in vec3 in_normal;
in vec4 in_color;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_proj;

out vec3 vs_normal;
out vec3 vs_position;
out vec4 vs_color;

void main() {
	vs_color    = in_color;
	vs_normal	= (vec4(in_normal, 1.0)).xyz;
	vs_position = (in_model * vec4(in_position, 1.0)).xyz;
	gl_Position = in_proj * in_view * in_model * vec4(in_position, 1.0);
}
