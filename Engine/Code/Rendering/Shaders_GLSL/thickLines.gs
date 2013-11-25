#version 410

uniform vec2 viewportSize;	//in pixels
uniform float lineWidth;	//in pixels
uniform float in_MaxDepth;  //in meters

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

varying vec3 vertex_color;

in vData{
	vec4 vColor;
	float fDepth;
}vertices[];

out gData{
	vec4 gColor;
}geom;

void main(){
	if(gl_in[0].gl_Position.z<0 || gl_in[1].gl_Position.z<0)
		return;
	float lineWidthThr0;
	if(vertices[0].fDepth>in_MaxDepth)
		lineWidthThr0 = 1;
	else
		lineWidthThr0 = lineWidth+1 -  lineWidth * (vertices[0].fDepth/in_MaxDepth);
	float lineWidthThr1;
	if(vertices[1].fDepth>in_MaxDepth)
		lineWidthThr1 = 1;
	else
		lineWidthThr1 = lineWidth+1 -  lineWidth * (vertices[1].fDepth/in_MaxDepth);

	vec3 ndc0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
	vec3 ndc1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;

	vec2 lineScreenForward = normalize(ndc1.xy - ndc0.xy);
	vec2 lineScreenRight = vec2(-lineScreenForward.y, lineScreenForward.x);
	vec2 lineScreenOffset0 = (vec2(lineWidthThr0) / viewportSize) * lineScreenRight;
	vec2 lineScreenOffset1 = (vec2(lineWidthThr1) / viewportSize) * lineScreenRight;

	gl_Position = vec4(ndc0.xy + lineScreenOffset0, ndc0.z, 1.0);
	geom.gColor = vertices[0].vColor;
	EmitVertex();

	gl_Position = vec4(ndc0.xy - lineScreenOffset0, ndc0.z, 1.0);
	geom.gColor = vertices[0].vColor;
	EmitVertex();

	gl_Position = vec4(ndc1.xy + lineScreenOffset1, ndc1.z, 1.0);
	geom.gColor = vertices[1].vColor;
	EmitVertex();

	gl_Position = vec4(ndc1.xy - lineScreenOffset1, ndc1.z, 1.0);
	geom.gColor = vertices[1].vColor;
	EmitVertex();

	EndPrimitive();
}
