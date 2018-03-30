#version 330 core
in vec4 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 ourPos;

#define MAX_LIGHTS 10
uniform int _numLights;
uniform struct Light {
   vec3 position;
   vec4 l_color; //a.k.a the color of the light
   float attenuation;
   float ambientCoefficient;
   float coneAngle;
   vec3 coneDirection;
} _lights[MAX_LIGHTS];


uniform float _time;
uniform float test;
uniform vec4 col;
uniform vec4 col2;
uniform bool test_bool;
out vec4 color;


uniform  mat4 viewproj;
uniform mat4 model;

uniform mat4 view;



void main()
{
vec4 final_col = col;
if(!test_bool){
final_col = col2;
}
color =col2;
}