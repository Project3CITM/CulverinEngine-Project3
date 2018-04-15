#version 330 core

uniform samplerCube cubeMap;

in vec3 TexCoord;
out vec4 color;
 void main(){

color = texture(cubeMap,normalize(TexCoord));

 }