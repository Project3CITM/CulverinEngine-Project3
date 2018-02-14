#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 color;

in struct Light{
    vec3 position;
    

};

in Light[4] lights;


out float ourTime;
out vec4 ourColor;
out vec3 ourNormal;
out vec2 TexCoord;
uniform float _time;
uniform vec4 _color;
uniform mat4 model;
uniform mat4 viewproj;
uniform mat4 view;

void main()
{

gl_Position = viewproj *  model * vec4(position.x,position.y,position.z, 1.0f);
ourTime = _time;
		
}