#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 color;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 ShadowCoord;
out vec4 our_pos;
// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;
uniform mat4 depthBias;
uniform mat4 view;
uniform mat4 model;
uniform mat4 viewproj;


void main(){

	// Output position of the vertex, in clip space : MVP * position
	our_pos =  depthMVP* model*vec4(position,1);
	
	ShadowCoord = depthBias * model*vec4(position,1);
	
	// UV of the vertex. No special space for this one.
	UV = texCoord;
}

