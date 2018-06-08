#version 330 core 

#define MAX_LIGHTS 50
uniform int _numLights;
uniform struct Light {
    vec3 position;
    int type;
    vec4 l_color; //a.k.a the color of the light
    vec4 properties;
    float ambientCoefficient;
    float radius;
   
 
} _lights[MAX_LIGHTS];

in vec4 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 ourPos;
in float wave_height;
in vec3 FragPos;

uniform vec4 diff_color;			
out vec4 color;						
				
									
uniform mat4 viewproj;				
uniform mat4 model;					
uniform mat4 view;

uniform vec3 _cameraPosition;
uniform float _alpha;
uniform float _time;

uniform float a_LightInt;			
uniform float a_Ka;					
uniform float a_Kd;					
uniform float a_Ks;					
uniform float a_shininess;			
							
uniform vec4 top_col;
uniform vec4 down_col;	
uniform vec4 ambient_col;
uniform float waveHeight;	                                                                     
									                                                                     
vec3 blinnPhongDir(Light light, float Kd, float Ks, float shininess, vec3 N)
{																										 
																							 
	vec3 surfacePos = vec3(model * vec4(ourPos, 1.0));										 
	vec3 v = normalize(_cameraPosition - surfacePos);										 
		
																				 
	float lightInt =  light.properties[0];
  			
																		 
	vec3 normal =  ourNormal ;							 
																										 
	if (light.type != 0) {																				 
																										 
																					 
		
        vec3 s =  normalize(light.position );
       
        vec3 r = reflect(-s,normal);	

        float cosTheta = clamp( dot(s, normal), 0,1 );																 
		float cosAlpha = clamp( dot( v,r ), 0,1 );																								 
	
																						 
		float diffuse = Kd * lightInt * cosTheta;
        float spec =  Ks* lightInt* pow(cosAlpha, shininess);	
					 
		return vec3(diffuse,spec,1);																	 
																										 
	}																									 
																										 
	else {		
				
        vec3 lightpos =  light.position;                                                                             
        vec3 s =  normalize(lightpos - surfacePos);      
        vec3 r = reflect(-s,normal);
 
       float cosTheta = clamp( dot( s,normal ), 0.3,1 );       
        float cosAlpha = clamp( dot( v,r ), 0.3,1 ) ;	                                             
                                                                                                         
        float d = length((lightpos - surfacePos));
      int power = 2;
 float attenuation = 1 - (pow(d*2,power)/pow(light.radius,power));

attenuation = lightInt * max(0, attenuation)*0.6;                         
        float diffuse = attenuation * Kd  * cosTheta;                    
        float spec = attenuation * Ks * pow(cosAlpha,shininess);
                                                                                                                 
        return vec3(diffuse,spec,attenuation);                                                               
                                                              															 
																												 
	}																											 
																												 
																												 
}																												 
																												 
																												 
void main()																									
{																												 
																							 
													 

																		 
																												 
	vec3 inten = vec3(0); vec3 inten_final = vec3(0);																					 
																		 
	vec4 light_colors[MAX_LIGHTS];
		
	float final_ambient = 0;																								
	vec3 final_color = vec3(0);	
																		 
	for (int i = 0; i <_numLights; ++i) {																		 
		inten = blinnPhongDir(_lights[i], a_Kd, a_Ks, a_shininess, ourNormal);				
		inten_final.xy += inten.xy;																			
		light_colors[i] = vec4(_lights[i].l_color.rgb,inten.z);												
	}																										
																			
	for (int i = 0; i<_numLights; ++i) {																	
		final_color += vec3(light_colors[i]) * light_colors[i].a;											
		final_ambient += _lights[i].ambientCoefficient;
	}		
																								
	final_ambient = final_ambient/_numLights;
	final_color = normalize(final_color);	

float height = wave_height - ourPos.y;
float top_mult = clamp((wave_height + waveHeight),0,1);// / (waveHeight*1);
float down_mult = abs(top_mult-1);
																
	vec3 col = max(final_ambient*ambient_col.rgb, 
    (inten_final.x + inten_final.y )
   *final_color.rgb) * (top_col.rgb *top_mult + down_col.rgb * down_mult); 
																						
	color = vec4(col,_alpha) ;

}