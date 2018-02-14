#version 330 core
in vec4 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 ourPos;

in struct Light{
    vec3 position;
    

};

in Light[4] lights;
uniform float _time;
out vec4 color;


uniform  mat4 viewproj;
uniform mat4 model;

uniform mat4 view;



void main()
{

color =  vec4(1 ,0,0,1);
}