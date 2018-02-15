#version 330 core
in vec4 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 ourPos;

#define MAX_LIGHTS 10
uniform int numLights;
uniform struct Light {
   vec3 position;
   vec3 intensities; //a.k.a the color of the light
   float attenuation;
   float ambientCoefficient;
   float coneAngle;
   vec3 coneDirection;
} _lights[MAX_LIGHTS];


uniform float _time;
uniform float test;
uniform vec4 col;
uniform vec4 col2;
out vec4 color;


uniform  mat4 viewproj;
uniform mat4 model;

uniform mat4 view;



void main()
{

color = col + col2;
}