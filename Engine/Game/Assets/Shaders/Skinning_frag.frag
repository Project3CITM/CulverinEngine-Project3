#version 330 core

in vec4 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 ourPos;

out vec4 color;

 void main(){
    color = vec4(ourColor);
 }