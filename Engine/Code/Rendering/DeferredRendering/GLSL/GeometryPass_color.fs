#version 150

in vec2 vs_texels;
in vec3 vs_normal;
in vec3 vs_position;

in vec4 vs_color;

layout (location = 0) out vec3 Position; 
layout (location = 1) out vec3 Diffuse; 
layout (location = 2) out vec3 Normal; 
//layout (location = 3) out vec3 TexCoord;     

void main(){
    Position = vs_position;
    Diffuse = vs_color;
    Normal = vs_normal;
    //TexCoord = vec3(in_uvOffset+vs_texels, 0.0);
}
