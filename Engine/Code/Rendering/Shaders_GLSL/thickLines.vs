#version 410

in vec3 in_position;
in vec4 vertex_color;

out vData{
	vec4 vColor;
	float fDepth;
}vertex;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_proj;
uniform vec3 in_CameraEyePos;


void main() {
	vertex.vColor = vertex_color;

	vec4 worldPos = in_model * vec4(in_position, 1.0);
	vertex.fDepth = distance(worldPos, vec4(in_CameraEyePos, 1.0));

	gl_Position = in_proj * in_view * worldPos;
}