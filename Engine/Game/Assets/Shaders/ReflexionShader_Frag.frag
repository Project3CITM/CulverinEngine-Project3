#version 330 core


uniform samplerCube cube_map;

in vec3 reflected_vector;


out vec4 color;

 

void main(){


color = texture(cube_map, normalize(reflected_vector));



 }