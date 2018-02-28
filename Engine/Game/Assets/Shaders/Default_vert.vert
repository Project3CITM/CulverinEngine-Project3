#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 5) in vec3 tangent;
layout(location = 6) in vec3 bitangent;

out float ourTime;
out vec4 ourColor;
out vec3 ourNormal;
out vec2 TexCoord;
out vec3 ourPos;
out mat3 TBN;

uniform float _time;
uniform vec4 _color;
uniform mat4 model;
uniform mat4 viewproj;
uniform mat4 view;
uniform mat4 modelview;


uniform sampler2D height_map;

void main()
{
	TexCoord = texCoord;
	ourTime = _time;
	ourPos = position;
	ourNormal = normal;

   vec3 vertexTangent_cameraspace = mat3(modelview) * tangent;
	vec3 vertexBitangent_cameraspace = mat3(modelview) * bitangent;
	vec3 vertexNormal_cameraspace = mat3(modelview) * normal;
	
    vec3 T = normalize(vec3(model * vec4(tangent, 0)));
	vec3 B = normalize(vec3(model * vec4(bitangent, 0)));
	vec3 N = normalize(vec3(model * vec4(normal, 0)));
    TBN = transpose(mat3(T,B,N));
	
	

	gl_Position = viewproj *  model * vec4(position, 1.0f);
}