#version 330 core

#extension GL_NV_shadow_samplers_cube : enable
// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 ShadowCoord;
in vec3 ourNormal;
in vec3 ourPos;
// Ouput data
out vec4 color;


#define MAX_LIGHTS 10
uniform int _numLights;
uniform struct Light {
   vec3 position;
   int type;
   vec4 l_color; //a.k.a the color of the light
   float attenuation;
   float ambientCoefficient;

} _lights[MAX_LIGHTS];

uniform vec4 col;
uniform float a_LightInt;
uniform float a_Ka;
uniform float a_Kd;
uniform float a_Ks;
uniform float a_shininess;
// Values that stay constant for the whole mesh.

uniform samplerCube cubeMap;
uniform sampler2D myTextureSampler;
uniform sampler2D normal_map;
uniform sampler2D occlusion_map;
uniform mat4 model;
uniform vec3 _light_dir;
uniform int iterations;
uniform int shadow_blur;
uniform float bias;
uniform vec3 _cameraPosition;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
};


float VectorToDepth (vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    // Replace f and n with the far and near plane values you used when
    //   you drew your cube map.
    const float f = 25;
    const float n = 1.0;

    float NormZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

vec2 blinnPhongDir(Light light, float lightInt, float Ka, float Kd, float Ks, float shininess)
{


 vec3 N =normalize(texture(normal_map,UV).xyz);

vec3 surfacePos =vec3(model * vec4(ourPos, 1));
vec3 surfaceToCamera =  normalize(_cameraPosition - surfacePos);
vec3 lightDir;

vec3 normal = normalize(transpose(inverse(mat3(model)))*ourNormal * N) ;

if(light.type != 0){


lightDir = normalize(light.position);


vec3 s = normalize(lightDir);

float diffuse = Ka+ Kd * lightInt *  max( dot(normal ,s),0);
float spec = 0;

spec = Ks* pow(max(0, dot(surfaceToCamera,reflect(-s,normal))), shininess);
return vec2(diffuse, spec);

}

else {



vec3 s = normalize(light.position - surfacePos);

float distanceToLight = length(light.position - surfacePos);
float attenuation = 1/ (1.0 + 0.1 * pow(distanceToLight,2));

float diffuse = Ka + attenuation * Kd *lightInt * max(0, dot(normal , s));
float spec = 0;
if (diffuse >0)
 spec = attenuation * Ks *lightInt* pow(max(0, dot(surfaceToCamera, reflect(-s, normal))), shininess);
 
return vec2(diffuse,spec);
 }

}

void main(){

vec3 color_texture = texture(myTextureSampler, UV).rgb;
vec3 occlusion_texture = texture(occlusion_map, UV).rgb;
vec3 ambient = 0.1 * color_texture;

vec3 n = normalize(mat3(model)*ourNormal);
vec3 l = normalize(_light_dir);

float cosTheta = dot(n,l);
float s_bias = bias * cosTheta;

s_bias = clamp(s_bias, 0.000,0.0045);
  float visibility = 1.0f;

vec4 ShadowPos = ShadowCoord/ShadowCoord.w;
vec3 LightDir = vec3(model * vec4(ourPos, 1)) -  _lights[0].position;
float LightDepth    = VectorToDepth (LightDir);


	float SampledDistance = texture(cubeMap,normalize(LightDir)).r;

	float Distance =  (length(LightDir) - 1)/(50-1);
 
    if (Distance> SampledDistance + s_bias)
        visibility =0;// Inside the light
    else
        visibility =1; // Inside the shadow



//}
int a = 1;
for(int i =0 ; i< _numLights ; i++){
a = 0;
}

color = vec4(vec3(SampledDistance-(Distance-0.5)), 1   - a * 0.001);
//color = vec4(vec3(Distance), 1   - a * 0.001);
color = vec4(vec3(SampledDistance), 1   - a * 0.001);
color = vec4(vec3(visibility), 1  - a * 0.001);
//color =vec4(ambient,1) + vec4(color_texture,1) * visibility * cosTheta;

}