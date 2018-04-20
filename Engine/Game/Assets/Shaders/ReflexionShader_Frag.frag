#version 330 core


uniform samplerCube cube_map;
uniform mat4 model;

uniform mat4 viewproj;
uniform vec3 _cameraPosition;

in vec3 reflected_vector;
in vec3 ourNormal;
in vec3 ourPos;
out vec4 color;

 

void main(){


 
vec3 tmp = normalize(ourPos -  _cameraPosition);
 
vec3 temp = reflect(tmp, normalize(ourNormal));

color = texture(cube_map, normalize(temp));



 }