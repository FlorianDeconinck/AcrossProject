#version 150
in vec2 Texcoord;

uniform int blurRadius;
uniform int blurWdw;
uniform sampler2D texFramebuffer;

out vec4 outColor;

void main(){
    float blurRatioH = 1.0/blurRadius;
    float blurRatioV = blurRatioH;
    vec4 E = vec4(0,0,0,0);
    int blurWdwSQ = blurWdw*blurWdw;
    for(int x = -blurWdw ; x < blurWdw ; x++)
        for(int y = -blurWdw ; y < blurWdw ; y++)
            E += texture(texFramebuffer, vec2(Texcoord.x+x*blurRatioH, Texcoord.y+y*blurRatioV))/blurWdwSQ;
    outColor = E;
}