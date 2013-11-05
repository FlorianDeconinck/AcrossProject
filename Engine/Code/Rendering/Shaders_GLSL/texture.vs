#version 150

in vec3 in_position;
in vec2 in_texels;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_proj;

out vec2 vs_texels;

void main() {
	vs_texels = in_texels;
	gl_Position = in_proj * in_view * in_model * vec4(in_position, 1.0);
}