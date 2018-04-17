#version 330 core



layout(location = 0) in vec3 position;

layout(location = 1) in vec3 normal;


uniform mat4 model;

uniform mat4 viewproj;

uniform mat4 view; 

uniform vec3 _cameraPosition;


out vec3 reflected_vector;

 

void main(){


 gl_Position = viewproj *  model * vec4(position, 1.0f);

 
vec3 tmp = normalize( vec3(viewproj  * vec4(position,1)) - _cameraPosition);
 
reflected_vector = reflect(tmp, normalize(normal));



 
}