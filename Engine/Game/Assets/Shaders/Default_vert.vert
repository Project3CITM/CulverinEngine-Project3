#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 color; 

out float ourTime;
out vec4 ourColor;
out vec3 ourNormal;
out vec2 TexCoord;
out vec3 ourPos;
uniform float _time;
uniform vec4 _color;
uniform mat4 model;
uniform mat4 viewproj;
uniform mat4 view;

uniform sampler2D height_map;

void main()
{
	TexCoord = texCoord;
	ourTime = _time;
	ourPos = position;
	ourNormal = normal;

	gl_Position = viewproj *  model * vec4(position, 1.0f);
}