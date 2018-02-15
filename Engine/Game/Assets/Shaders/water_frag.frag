#version 330 core
in vec4 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 ourPos;
in float wave_height;
uniform float _time;
out vec4 color;
uniform sampler2D texture_;
uniform sampler2D normal_map;
uniform sampler2D texture3;

uniform  mat4 viewproj;
uniform mat4 model;
uniform mat4 view;



vec2 blinnPhongDir(vec3 lightDir, float lightInt, float Ka, float Kd, float Ks, float shininess)
{

 vec2 Tile = vec2(1,1);
vec2 xy = TexCoord.xy;
vec2 phase = fract(xy*Tile);


vec3 s = normalize(lightDir);
vec3 v = normalize(view * vec4(-ourPos,1)).xyz;
vec3 n = texture( normal_map,phase ).xyz*ourNormal*2 ;//ourNormal * texture(_normal_map, TexCoord).xyz;
vec3 h = normalize(v+s);
float diffuse = Ka + Kd * lightInt * max(0.0, dot(n, s)+0.8);
float spec = Ks * pow(max(0.0, dot(reflect(-lightDir,n),h)), shininess);
return vec2(diffuse, spec);

}


void main()
{
 // light data
 vec3 lightdir = normalize(vec3(1,1, 1));
 vec3 ambient = ourColor.xyz/15;
 vec3 diffuse = ourColor.xyz;
 vec3 specular = vec3(1, 0, 1);
vec3 lcolor =ourColor.xyz;
vec3 viewDirection = normalize(vec3( view *vec4(0,0,0, 1.0) - model * vec4(ourPos*2,1)));


vec3 color_texture = texture(texture_, TexCoord/1.2 + ((sin(_time/2)+1)/20)).xyz * diffuse;
vec3 normal_map = texture(normal_map, TexCoord).xyz;
vec3 foam_tex = texture(texture3,TexCoord/1.2 +  ((sin(_time*3))/100)).xyz * 0.7;

float y = normal_map.y;
normal_map.x = (2*normal_map.x)-1;
normal_map.y = (2*normal_map.z)-1;
normal_map.z = y;
normal_map = normalize(normal_map);
vec3 normal =normalize(ourNormal) + normal_map.x * vec3(1,0,0) + normal_map.y* vec3(0,1,0); 
vec3 specularReflection;

float spec;
float attenuation = 0.6;
float temp  = dot(normal, lightdir);
float height =wave_height - ourPos.z ;


vec3 reflection = normalize( temp * normal - lightdir); 

spec =pow(max(0, dot(reflection,viewDirection )),400);
specularReflection = vec3(1,1,1) * spec * attenuation;

vec3 final_color = max((color_texture)*temp ,ambient);
vec2 inten = blinnPhongDir(lightdir, 1, 0.5, 1, 1, 5.0);

float distortion_wave =  sin(ourPos.y)*0.3 * sin (ourPos.y/2) + sin(ourPos.y)*0.2;
float min_height = 1.3 +distortion_wave;
if(height > min_height)
{

color = vec4(mix(color_texture * inten.x + specularReflection,color_texture * inten.x + specularReflection + vec3(height-min_height)*foam_tex , 0.6 ) ,1);
}
else 
{
color = vec4(color_texture * inten.x + specularReflection , 1);
}

}