#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;


out vec4 ourColor;
out vec2 TexCoord;
out vec3 ourNormal;
out vec3 ourPos;
out float wave_height;

uniform float waveHeight;
uniform float num_waves_x;
uniform float num_waves_y;

uniform vec4 _color;
uniform float _time;
uniform mat4 model;
uniform mat4 viewproj;
uniform mat4 view;

vec3 CalcNormal(in float x, in float y, in float z){
float t = _time*10;
float A = cos(x + y+t) +  cos(x+y + t);// sin(x*4.46 + cos(2.9*x + _time)+_time);// + 4.46*cos( 4.46* x);// + 4.6*cos(4.6*x);
float B =  cos(y*1.1 +t/1.3) + cos(y*1.5+t/1.4);
float C =  cos(z*1.2 +t/1.1 ) + cos(z*1.6+t/1.6);
return vec3(A,B,C);
}

void main()
{

vec3 new_pos = mat3(model) * position;
float x = new_pos.x * num_waves_x;
float y = new_pos.y * num_waves_y;
float z = new_pos.z * num_waves_y;
float t = _time*10;
float distortion_x = sin(x + y+ t)+ sin(x+y + t );
float distortion_y =  sin(y*1.1 +t/1.3) + sin(y*1.5+t/1.4);
float distortion_z = sin(z*1.2 +t/1.1 ) + sin(z*1.6+t/1.6);
float distortion = waveHeight * (distortion_x + distortion_y + distortion_z);
gl_Position =  viewproj* model * vec4( (position.x + waveHeight* distortion_x ),(position.y +waveHeight* distortion_y), (position.z + waveHeight * distortion_z), 1.0f);
mat4 test = view;
ourColor = _color;
ourNormal = CalcNormal(x, y, z);
TexCoord = texCoord;
ourPos = position;
wave_height = position.y + distortion_x + distortion_y;

}
