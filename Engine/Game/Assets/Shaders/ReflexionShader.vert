#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 5) in vec3 tangent;
layout(location = 6) in vec3 bitangent;


uniform float _time;
uniform vec4 _color;
uniform mat4 model;
uniform mat4 viewproj;
uniform mat4 view; 

out float ourTime;
out vec2 TexCoord;
out mat3 TBN;
out vec3 FragPos;
out vec3 ourNormal;

out vec3 ourPos; 

//Outputs for shadow-mapping
out vec4 shadowCoord;

// Uniform for shadow-mapping
uniform mat4 depthBias;

void main(){

	TexCoord = texCoord;
	shadowCoord = depthBias * model * vec4(position, 1.0);

	vec3 T = normalize(vec3( model * vec4(tangent, 0)));
	vec3 B = normalize(vec3( model * vec4(bitangent, 0)));
	vec3 N = normalize(vec3( model * vec4(normal, 0)));
    	TBN = transpose(mat3(T,B,N));
	FragPos = TBN * vec3(model * vec4(position,1));

	vec4 pos =  viewproj *  model * vec4(position, 1.0f);
 
	gl_Position = pos;
	ourPos =  vec3(model * vec4(position, 1.0f));
 
	ourNormal =  mat3(model) * normal;


 
}