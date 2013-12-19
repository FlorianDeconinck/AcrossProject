#version 150

in vec4 vs_position;

out vec4 outColor;

uniform vec2 ScreenSize;
uniform vec3 light_emitted;
uniform sampler2D depth_test;

void main(){
    vec4 FragCoorNorm = vec4(gl_FragCoord.x/ScreenSize.x, gl_FragCoord.y/ScreenSize.y, gl_FragCoord.z, gl_FragCoord.w);
    if(texture(depth_test, FragCoorNorm.xy).r > FragCoorNorm.z){
        outColor = vec4(light_emitted,1.0);
    }
}
