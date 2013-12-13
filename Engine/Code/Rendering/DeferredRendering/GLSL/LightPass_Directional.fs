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
uniform vec3 light_direction;

uniform vec3 eye_world_position;

vec4 Phong(vec3 WorldPos, vec3 Normal){
    float MatSpecular = 1.f;
    
    //vec4 AmbientColor = vec4(light_diffuse, 1.0f) * light_diffuse_intensity;
    float DiffuseFactor = dot(Normal, -light_direction);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(light_diffuse, 1.0f) * light_diffuse_intensity * DiffuseFactor;

        vec3 VertexToEye = normalize(eye_world_position - WorldPos);
        vec3 LightReflect = normalize(reflect(light_direction, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, light_specular_intensity);
        if (SpecularFactor > 0) {
            SpecularColor = vec4(light_specular, 1.0f) * MatSpecular * SpecularFactor;
        }
    }
    //return AmbientColor + DiffuseColor + SpecularColor;    
    return DiffuseColor + SpecularColor;    
}


void main(){
    vec2 TexCoord   = gl_FragCoord.xy / uScreenSize;
    vec3 Color      = texture(uColorMap, TexCoord).xyz;
    vec3 WorldPos   = texture(uPositionMap, TexCoord).xyz;
    vec3 Normal     = texture(uNormalMap, TexCoord).xyz;
    Normal = normalize(Normal);

    outColor = vec4(Color,1.0) * Phong(WorldPos, Normal);
}
