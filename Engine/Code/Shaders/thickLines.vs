#version 410

in vec3 in_position;
in vec4 vertex_color;

out vData{
	vec4 vColor;
}vertex;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_proj;

void main() {
	vertex.vColor = vertex_color;
	gl_Position = in_proj * in_view * in_model * vec4(in_position, 1.0);
}