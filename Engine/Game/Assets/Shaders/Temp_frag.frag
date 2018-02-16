#version 330 core
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

uniform float a_LightInt;
uniform float a_Ka;
uniform float a_Kd;
uniform float a_Ks;
uniform float a_shininess;

vec2 blinnPhongDir(vec3 lightDir, float lightInt, float Ka, float Kd, float Ks, float shininess)
{

 vec2 Tile = vec2(1,1);
vec2 xy = TexCoord.xy;
vec2 phase = fract(xy*Tile);


vec3 s = normalize(lightDir);
vec3 v = normalize(view * vec4(-ourPos,1)).xyz;
vec3 n =normalize(transpose(inverse(mat3(model))) * ourNormal); ;//ourNormal * texture(_normal_map, TexCoord).xyz;
vec3 h = normalize(v+s);
float diffuse = Ka+ Kd * lightInt * max(0, dot(n, s));
float spec = Ks* pow(max(0, dot(reflect(-lightDir,-n),h)), shininess);
return vec2(diffuse, spec);

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


vec2 inten = blinnPhongDir(lightdir, a_LightInt, a_Ka, a_Kd, a_Ks, a_shininess);



color = vec4(color_texture *( inten.x + inten.y) , 1);

}