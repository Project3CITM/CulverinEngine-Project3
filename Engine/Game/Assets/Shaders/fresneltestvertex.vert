#version 330 core



layout(location = 0) in vec3 position;

layout(location = 2) in vec3 normal;

 

uniform vec3 _cameraPosition;



uniform mat4 model;

uniform mat4 viewproj;

uniform mat4 view;



out vec3 camera_to_vertex;

 
out vec3 vertex_normal;

void main(){


gl_Position = viewproj * model * vec4(position, 1.0);


camera_to_vertex = normalize(_cameraPosition - (model * vec4(position, 1.0)).xyz);


vertex_normal = normalize(mat3(model) * normal);
}