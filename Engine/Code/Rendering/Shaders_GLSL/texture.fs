#version 150

in vec2 vs_texels;

out vec4 outColor;

uniform sampler2D   colorMap;
uniform vec2        in_uvOffset;

void main(){
    outColor = texture(colorMap, in_uvOffset+vs_texels);
}