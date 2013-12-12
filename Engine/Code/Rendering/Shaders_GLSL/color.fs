#version 150

in vec4 fragment_color;
in vec3 fragment_normal;

out vec4 outColor;

void main(){
    outColor = fragment_color;
}