#version 150

in gData{
    vec4 gColor;
}geom;

out vec4 outColor;

void main(){
    outColor = geom.gColor;
}