#version 150

in vec2 vs_texels;

out vec4 outColor;

uniform sampler2D colorMap;

void main(){
    outColor = texture(colorMap, vs_texels);
}