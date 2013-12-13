#version 150

in vec3 in_position;
in vec2 in_texels;
in vec3 in_normal;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_proj;

out vec2 vs_texels;
out vec3 vs_normal;
out vec3 vs_position;

void main() {
	vs_texels	= in_texels;
	vs_normal	= (vec4(in_normal, 1.0)).xyz;
	vs_position = (in_model * vec4(in_position, 1.0)).xyz;
	gl_Position = in_proj * in_view * in_model * vec4(in_position, 1.0);
}
