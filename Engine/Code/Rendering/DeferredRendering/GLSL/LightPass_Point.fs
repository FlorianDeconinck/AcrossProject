#version 150

out vec4 outColor;

uniform vec2        uScreenSize;

uniform sampler2D   uPositionMap;
uniform sampler2D   uColorMap;
uniform sampler2D   uNormalMap;

uniform vec3 light_specular;
uniform float light_specular_intensity;
uniform vec3 light_diffuse;
uniform float light_diffuse_intensity;
uniform vec3 light_position;
uniform float light_radius;

uniform vec3 eye_world_position;

vec4 Phong(vec3 WorldPos, vec3 Normal){
    vec3 L = light_position - WorldPos;
    float dot = max(dot(L, Normal), 0);
    return vec4(light_diffuse * dot, 1.0);
}

//   Attenuation
float Attenuation(vec3 WorldPos){
    vec3 lightToVertex = light_position - WorldPos;
    float sqrDist = dot (lightToVertex, lightToVertex);
    float OneOverLightRangeSqr = 1  / (light_radius*light_radius);//to get ou of the shader
    float atten = max (0, 1 - sqrDist * OneOverLightRangeSqr);
    atten *= atten;     
    return atten;
}

void main(){
    vec2 TexCoord   = gl_FragCoord.xy / uScreenSize;
    vec3 Color      = texture(uColorMap, TexCoord).xyz;
    vec3 WorldPos   = texture(uPositionMap, TexCoord).xyz;
    vec3 Normal     = texture(uNormalMap, TexCoord).xyz;
    Normal = normalize(Normal);

    outColor = vec4(Color,1.0) * (Phong(WorldPos, Normal) * Attenuation(WorldPos));
}
