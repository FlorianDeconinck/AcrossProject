#version 150

out vec4 outColor;

uniform vec2        uScreenSize;

uniform sampler2D   uPositionMap;
uniform sampler2D   uColorMap;
uniform sampler2D   uNormalMap;
//uniform sampler2D   uTexCoordMap;

uniform vec3 light_specular;
uniform vec3 light_diffuse;
uniform vec3 light_position;

uniform mat4 in_view;

const vec3 kd = vec3 (0.7, 0.7, 0.7);
const vec3 ks = vec3 (1.0, 1.0, 1.0);
const float specular_exponent = 1.0;

vec3 phong (in vec3 p_eye, in vec3 n_eye) {
  //vec3 light_position_eye = vec3 (in_view * vec4 (light_position, 1.0));
  vec3 light_position_eye = light_position;
  vec3 dist_to_light_eye = light_position_eye - p_eye;
  vec3 direction_to_light_eye = normalize (dist_to_light_eye);
  
  // standard diffuse light
  float dot_prod = max (dot (direction_to_light_eye,  n_eye), 0.0);
  vec3 Id = light_diffuse * kd * dot_prod; // final diffuse intensity
  
  // standard specular light
  vec3 reflection_eye = reflect (-direction_to_light_eye, n_eye);
  vec3 surface_to_viewer_eye = normalize (-p_eye);
  float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
  dot_prod_specular = max (dot_prod_specular, 0.0);
  float specular_factor = pow (dot_prod_specular, specular_exponent);
  vec3 Is = light_specular * ks * specular_factor; // final specular intensity

  return (Id + Is);
}

void main(){
    vec2 TexCoord   = gl_FragCoord.xy / uScreenSize;
    vec3 Color      = texture(uColorMap, TexCoord).xyz;
    vec3 WorldPos   = texture(uPositionMap, TexCoord).xyz;
    vec3 Normal     = texture(uNormalMap, TexCoord).xyz;
    //vec3 TexCoordFM = texture(uTexCoordMap, TexCoord).xyz;
    Normal = normalize(Normal);

    outColor = vec4(Color * phong (WorldPos, Normal) , 1.0);
/*
    outColor = vec4(Normal, 1.0);
*/
}
