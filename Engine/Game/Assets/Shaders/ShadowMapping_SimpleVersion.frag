#version 330 core


// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 ShadowCoord;
in vec3 ourNormal;
in vec3 ourPos;
// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.

uniform sampler2DShadow shadowMap;
uniform sampler2D myTextureSampler;
uniform mat4 model;
uniform vec3 _light_dir;
uniform int iterations;
uniform int shadow_blur;
uniform float bias;


vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main(){

vec4 color_texture = texture(myTextureSampler, UV);
vec3 n = normalize(mat3(model)*ourNormal);
vec3 l = normalize(_light_dir);

float cosTheta = dot(n,l);
float s_bias = bias * cosTheta;
// cosTheta is dot( n,l ), clamped between 0 and 1
s_bias = clamp(s_bias, 0.000,0.0045);
  float visibility = 1.0f;

vec4 ShadowPos = ShadowCoord/ShadowCoord.w;


for(int i =0;i<iterations;i++){

		
		//int index = i%16;
		
		//int index = int(16.0*random(gl_FragCoord.xyy, i))%16;
		
		int index = int(16.0*random(floor(mat3(model)* ourPos*1000.0), i))%16;
	float shadow_val = (1-texture( shadowMap, vec3(ShadowPos.xy + poissonDisk[index]/shadow_blur , (ShadowCoord.z-bias)/ShadowCoord.w) ));

   visibility -=(0.007)*shadow_val;
}

visibility = clamp(visibility, 0,1);
vec4 ambient = vec4(0.1,0.1,0.1,1);
color = color_texture * visibility * cosTheta;

}