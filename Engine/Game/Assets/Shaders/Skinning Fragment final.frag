#version 330 core																							 
																												 
																												 
#define MAX_LIGHTS 30																							 
uniform int _numLights;																						 
uniform struct Light {																							 
	vec3 position;																								 
	int type;																									 
	vec4 l_color; //a.k.a the color of the light																 
	float intensity;																							 
	float ambientCoefficient;																					 
																												 
} _lights[MAX_LIGHTS];																							 
																												 
in vec4 ourColor;																								 
in vec2 TexCoord;																								 
in vec3 ourNormal;																								 
in vec3 ourPos;																								 
																												 
uniform vec4 diff_color;																						 
out vec4 color;																								 
uniform sampler2D albedo;																						 
uniform sampler2D normal_map;																					 
uniform sampler2D occlusion_map;																				 
																												 
																												 
uniform mat4 viewproj;																							 
uniform mat4 model;																							 
uniform mat4 view;																								 
uniform vec3 _cameraPosition;																					 
																												 
uniform float a_LightInt;																						 
uniform float a_Ka;																							 
uniform float a_Kd;																							 
uniform float a_Ks;																							 
uniform float a_shininess;																						 
																												 
																												 
vec3 blinnPhongDir(Light light, float lightInt, float Ka, float Kd, float Ks, float shininess, vec3 N)				 
{																												 
																												 
																												 

																												 
	vec3 surfacePos = vec3(model * vec4(ourPos, 1));															 
	vec3 surfaceToCamera = normalize(_cameraPosition - surfacePos);												 
	vec3 lightDir;																								 
																												 
	vec3 normal = normalize(transpose(inverse(mat3(model)))*ourNormal * N);										 
																												 
	if (light.type != 0) {																						 
																												 
																												 
		lightDir = normalize(light.position);																	 
																												 
																												 
		vec3 s = normalize(lightDir);																			 
																												 
		float diffuse = Kd * lightInt *  max(dot(normal ,s),0);											 
		float spec = 0;																							 
																												 
		spec = Ks* pow(max(0, dot(surfaceToCamera,reflect(-s,normal))), shininess);								 
		return vec3(diffuse, spec,1);																			 
																												 
	}																											 
																												 
	else {																										 
																												 
																												 
																												 
		vec3 s = normalize(light.position - surfacePos);														 
																												 
		float distanceToLight = length(light.position - surfacePos);											 
		float attenuation = 1 / (1.0 + 0.1 * pow(distanceToLight,2));											 
																												 
		float diffuse = attenuation * Kd *lightInt * max(0, dot(normal , s));								 
		float spec = 0;																							 
		if (diffuse >0)																							 
			spec = attenuation * Ks *lightInt* pow(max(0, dot(surfaceToCamera, reflect(-s, normal))), shininess);
																												 
		return vec3(diffuse,spec,attenuation);																	 
																												 
																												 
	}																											 
																												 
																												 
}																												 
																												 
																												 
void main()																									 
{																												 
	// light data																								 
	vec3 lightdir = normalize(vec3(1,1, 0));																	 
																												 
	vec3 lcolor = diff_color.xyz;																				 
	vec3 viewDirection = normalize(vec3(view *vec4(1,1,1, 1.0) - model * vec4(ourPos,1)));						 
																												 
																												 
	vec3 color_texture = texture(albedo, TexCoord).xyz;															 
	vec3 N = normalize(texture(normal_map,TexCoord).xyz);														 
	vec3 occlusion_texture = texture(occlusion_map,TexCoord).xyz;												 

	vec3 normal = normalize(ourNormal);// + normal_map.x * vec3(1,0,0) + normal_map.y* vec3(0,1,0); 			 
	vec3 specularReflection;																					 
																												 
	vec3 inten = vec3(0);																						 
	vec3 inten_final = vec3(0);																					 
	vec3 tot_light_ilu_color = vec3(0);																			 
	vec4 light_colors[MAX_LIGHTS];																				 
	for (int i = 0; i <_numLights; ++i) {																		 
		inten = blinnPhongDir(_lights[i], _lights[i].intensity, _lights[i].ambientCoefficient, a_Kd, a_Ks, a_shininess, N);			
		inten_final.xy += inten.xy;																				 
		light_colors[i] = vec4(_lights[i].l_color.rgb,inten.z);													 
	}																											 
	float final_ambient = 0;																										
	vec3 final_color = vec3(0);																					 
	for (int i = 0; i<_numLights; ++i) {																		 
		final_color += vec3(light_colors[i]) * light_colors[i].a;												 
		final_ambient += _lights[i].ambientCoefficient;
	}																											 
	final_ambient = final_ambient/_numLights;
	final_color = normalize(final_color);																		 
																												 
	color = vec4(final_ambient* color_texture + color_texture * (inten_final.x + inten_final.y)*occlusion_texture*final_color.rgb , 1);		 
																												 
}		