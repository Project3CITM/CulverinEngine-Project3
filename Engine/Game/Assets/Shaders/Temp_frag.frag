#version 330 core


#define MAX_LIGHTS 10
uniform int _numLights;
uniform struct Light {
   vec3 position;
   int type;
   vec4 l_color; //a.k.a the color of the light
   float attenuation;
   float ambientCoefficient;

} _lights[MAX_LIGHTS];

in vec4 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 ourPos;

uniform vec4 diff_color;
out vec4 color;
uniform sampler2D albedo;
uniform sampler2D normal_map;

uniform mat4 viewproj;
uniform mat4 model;
uniform mat4 view;
uniform vec3 _cameraPosition;

uniform float a_LightInt;
uniform float a_Ka;
uniform float a_Kd;
uniform float a_Ks;
uniform float a_shininess;


vec2 blinnPhongDir(Light light, float lightInt, float Ka, float Kd, float Ks, float shininess)
{
vec3 surfacePos =vec3(model * vec4(ourPos, 1));
vec3 surfaceToCamera =  normalize(_cameraPosition - surfacePos);
vec3 lightDir;
vec3 normal = normalize(transpose(inverse(mat3(model))) * ourNormal);
if(light.type != 0){

lightDir = normalize(light.position);
vec2 Tile = vec2(1,1);
vec2 xy = TexCoord.xy;
vec2 phase = fract(xy*Tile);


vec3 s = normalize(lightDir);

float diffuse = Ka+ Kd * lightInt * max(0, dot(normal, s));
float spec = 0;

spec = Ks* pow(max(0, dot(surfaceToCamera,reflect(-lightDir,normal))), shininess);
return vec2(diffuse, spec);

}

else {


vec3 s = normalize(light.position - surfacePos);

float distanceToLight = length(light.position - surfacePos);
float attenuation = lightInt/ (1.0 + 0.1 * pow(distanceToLight,2));

float diffuse = Ka + attenuation * Kd  * max(0, dot(normal, s));
float spec = attenuation * Ks * pow(max(0, dot(surfaceToCamera, reflect(-s, normal))), shininess);
 
return vec2(diffuse,spec);


}


}


void main()
{
 // light data
 vec3 lightdir = normalize(vec3(1,1, 0));

vec3 lcolor = diff_color.xyz;
vec3 viewDirection = normalize(vec3( view *vec4(1,1,1, 1.0) - model * vec4(ourPos,1)));


vec3 color_texture = texture(albedo, TexCoord).xyz;
vec3 normal_map = texture(normal_map, TexCoord).xyz;


float y = normal_map.y;
normal_map.x = (2*normal_map.x)-1;
normal_map.y = (2*normal_map.z)-1;
normal_map.z = y;
normal_map = normalize(normal_map);
vec3 normal =normalize(ourNormal);// + normal_map.x * vec3(1,0,0) + normal_map.y* vec3(0,1,0); 
vec3 specularReflection;

vec2 inten = vec2(0);
for(int i = 0; i <_numLights; ++i){
inten += blinnPhongDir(_lights[i], a_LightInt, a_Ka, a_Kd, a_Ks, a_shininess);
}


color = vec4(color_texture *( inten.x + inten.y) , 1);

}