#version 410

uniform vec2 viewportSize;	//in pixels
uniform float lineWidth;	//in pixels

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

varying vec3 vertex_color;

in vData{
	vec4 vColor;
}vertices[];

out gData{
	vec4 gColor;
}geom;

void main(){
	if(gl_in[0].gl_Position.w<0 || gl_in[1].gl_Position.w<0)
		return;

	vec3 ndc0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
	vec3 ndc1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;

	vec2 lineScreenForward = normalize(ndc1.xy - ndc0.xy);
	vec2 lineScreenRight = vec2(-lineScreenForward.y, lineScreenForward.x);
	vec2 lineScreenOffset = (vec2(lineWidth) / viewportSize) * lineScreenRight;

	gl_Position = vec4(ndc0.xy + lineScreenOffset, ndc0.z, 1.0);
	geom.gColor = vertices[0].vColor;
	EmitVertex();

	gl_Position = vec4(ndc0.xy - lineScreenOffset, ndc0.z, 1.0);
	geom.gColor = vertices[0].vColor;
	EmitVertex();

	gl_Position = vec4(ndc1.xy + lineScreenOffset, ndc1.z, 1.0);
	geom.gColor = vertices[1].vColor;
	EmitVertex();

	gl_Position = vec4(ndc1.xy - lineScreenOffset, ndc1.z, 1.0);
	geom.gColor = vertices[1].vColor;
	EmitVertex();

	EndPrimitive();
}