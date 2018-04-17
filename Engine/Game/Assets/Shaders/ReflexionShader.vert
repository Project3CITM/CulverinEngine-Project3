#version 330 core



layout(location = 0) in vec3 position;
layout(location = 1)in vec2 texCoord;
layout(location = 2) in vec3 normals;


uniform mat4 model;

uniform mat4 viewproj;

uniform mat4 view; 

uniform vec3 _cameraPosition;


out vec3 reflected_vector;
out vec3 ourNormal;
out vec3 ourPos; 

void main(){

vec4 pos =  viewproj *  model * vec4(position, 1.0f);
 gl_Position = pos;
ourPos =  vec3(model * vec4(position, 1.0f));
 
vec3 tmp = normalize( ourPos -  _cameraPosition);
 
reflected_vector = reflect(tmp, normalize( mat3(model) * normals));
ourNormal =  mat3(model) * normals ;


 
}