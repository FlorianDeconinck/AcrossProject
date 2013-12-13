#version 150

in vec2 vs_texels;
in vec3 vs_normal;

out vec4 outColor;

uniform sampler2D   colorMap;
uniform vec2        in_uvOffset;

void main(){
    outColor = texture(colorMap, in_uvOffset+vs_texels);
}