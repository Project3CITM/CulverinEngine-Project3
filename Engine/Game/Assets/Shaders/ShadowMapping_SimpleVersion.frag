#version 330 core


// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 ShadowCoord;
in vec3 ourNormal;
// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform sampler2DShadow shadowMap;
uniform sampler2D _shadowText;
uniform mat4 model;
uniform vec3 _light_dir;
void main(){

vec3 n = mat3(model)*ourNormal;
/*
vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
	
	
	
	
	float distanceFromLight = texture2D(shadowMap,ShadowCoord.xy).x;
	
	
 	float shadow = 1.0;
 	if (ShadowCoord.w > 0.0)
 		shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.0 : 1.0 ;
  	
	
  	color =	 vec4(vec3(shadow),1);

*/

float cosTheta = dot(n,_light_dir);
float bias = 0.005*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
bias = clamp(bias, 0,0.01);
  float visibility = 1.0f;
/* vec4 ShadowPos = ShadowCoord/ShadowCoord.w;

 if (( ShadowPos.x < 0 || ShadowPos.x > 1 ||  ShadowPos.y < 0 ||  ShadowPos.y > 1 ||  ShadowPos.z < 0 || ShadowPos.z > 1)){
       visibility = 1.0f;
     }else{
        float shadowDepth = texture(shadowMap,  ShadowPos.xy).r;
        if(shadowDepth< ShadowPos.z-bias)
            visibility = 0.0f;
     }*/

vec4 ShadowPos = ShadowCoord/ShadowCoord.w;
for(int i =0;i<4;i++){
   visibility -=0.2*(1-texture( shadowMap, vec3(ShadowPos.xy , (ShadowCoord.z-bias)/ShadowCoord.w) ));
}
vec4 ambient = vec4(0.1,0.1,0.1,1);
	color =	ambient + vec4(1) * visibility * cosTheta;
/*
	// Light emission properties
	vec3 LightColor = vec3(1,0,1);
	
	// Material properties
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
vec3 val = texture(_shadowText,ShadowCoord.xy).xyz;
	float visibility = texture( shadowMap, vec3(ShadowCoord.xy, (ShadowCoord.z)/ShadowCoord.w) );
float col = 1;

//color = vec4(vec3(gl_FragCoord.z),1);;
	color = vec4(MaterialDiffuseColor * vec3(visibility),1);
*/
}