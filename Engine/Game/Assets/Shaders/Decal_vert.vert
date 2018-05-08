#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 5) in vec3 tangent;
layout(location = 6) in vec3 bitangent;

out float ourTime;
out vec2 TexCoord;
out vec3 ourPos;
out vec3 world_pos;
out vec3 world_normal;
out mat3 TBN;
out vec3 FragPos;
out vec3 ourNormal;

//Outputs for shadow-mapping
out vec4 shadowCoord;

uniform float _time;
uniform vec4 _color;
uniform mat4 model;
uniform mat4 viewproj;
uniform mat4 view;
uniform mat4 modelview;

// Uniform for shadow-mapping
uniform mat4 depthBias;



void main()
{
	TexCoord = texCoord;
	ourPos = position;
    world_pos = (model  * vec4(position, 1.0)).xyz;
    world_normal = normalize(mat3(model) * normal);

   ourNormal = normalize(normal);


	gl_Position = viewproj *  model * vec4(position, 1.0f);
}
