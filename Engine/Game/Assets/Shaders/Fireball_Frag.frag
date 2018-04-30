#version 330 core 

#define MAX_LIGHTS 30
uniform int _numLights;

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
									                                                                     
vec3 blinnPhongDir( float Kd, float Ks, float shininess, vec3 N)
{																										 
																							 
	vec3 surfacePos = vec3(model * vec4(ourPos, 1.0));										 
	vec3 v = normalize(_cameraPosition - surfacePos);
																		 
	vec3 normal =  ourNormal ;							 
	
        vec3 s =  normalize(vec3(1) );
       
        vec3 r = reflect(-s,normal);	

        float cosTheta = clamp( dot(s, normal)+0.6, 0.4,1 );																 
		float cosAlpha = clamp( dot( v,r ), 0,1 );																								 
	
																						 
		float diffuse = Kd *2* cosTheta;
        float spec =  Ks* pow(cosAlpha, shininess);	
					 
		return vec3(diffuse,spec,1);																	 
																										 
																											 
																												 
}																												 
																												 
																												 
void main()																									
{																												 
		
 vec2 Tile = vec2(2,2);
  vec2 xy = TexCoord.xy;
vec2 phase = fract(xy*Tile);																					 
													 

																		 
																												 
	vec3 inten = vec3(0); vec3 inten_final = vec3(0);																					 
																		 
	vec4 light_colors[MAX_LIGHTS];
		
	float final_ambient = 0;																								
	vec3 final_color = vec3(0);	
																		 
																		 
	inten = blinnPhongDir( a_Kd, a_Ks, a_shininess, ourNormal);				
																		
										
	


float height = wave_height - ourPos.y;
float top_mult = clamp((wave_height + waveHeight),0,1);// / (waveHeight*1);
float down_mult = abs(top_mult-1);
																
	vec3 col = max(0.3*ambient_col.rgb, 
     (inten.x + inten.y )) 
* (top_col.rgb *top_mult + down_col.rgb * down_mult); 
																						
	color = vec4(col,_alpha) ;

}