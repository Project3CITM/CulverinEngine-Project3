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
uniform vec4 _color;
uniform float _time;
uniform mat4 model;
uniform mat4 viewproj;
uniform mat4 view;

vec3 CalcNormal(in float x, in float y){

float A = cos(x + _time) +  cos(2*x + _time);// sin(x*4.46 + cos(2.9*x + _time)+_time);// + 4.46*cos( 4.46* x);// + 4.6*cos(4.6*x);
float B =  cos(y+x/2 + _time);// + cos(y*4+x/2 +_time);

return vec3(A,B/2,0);
}

void main()
{

vec3 asd = mat3(model) * normal;
float x = position.x* (1.13);
float y = position.y* (1.13) ;
float t = _time;
float distortion_x = sin(x + t ) + sin(2*x + t )*2;//+ sin(2.9*x + t);// + sin(4.46*x );
float distortion_y = (sin(y +x/2 + t ));// + sin(4*y + x/2 + t ));
gl_Position =  viewproj* model * vec4(position.x,position.y,position.z + distortion_x + distortion_y, 1.0f);
mat4 test = view;
ourColor = _color;
ourNormal = CalcNormal(x, y);
TexCoord = texCoord;
ourPos = position;
wave_height = position.z + distortion_x + distortion_y;
}
