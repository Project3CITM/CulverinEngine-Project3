#version 330 core

layout(location = 0) in vec3 position;
uniform mat4 model;
uniform mat4 viewproj;

out vec3 TexCoord;

 void main(){
    TexCoord = position;
	gl_Position = viewproj *  model * vec4(position, 1.0f);


 }