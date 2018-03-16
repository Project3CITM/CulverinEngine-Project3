#version 330 core
 
#define MAX_LIGHTS 40
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
in mat3 TBN;
in vec3 FragPos;
 
uniform vec4 diff_color;           
out vec4 color;                    
uniform sampler2D albedo;          
uniform sampler2D normal_map;      
uniform sampler2D occlusion_map;
uniform sampler2D specular_map;                    
                               
uniform float a;
uniform float b;
uniform float c;
uniform float e;
    
uniform bool invert_norms;

uniform mat4 viewproj;             
uniform mat4 model;                
uniform mat4 view;
 
uniform vec3 _cameraPosition;
uniform float _alpha;
       
uniform float a_LightInt;          
uniform float a_Ka;                
uniform float a_Kd;                
uniform float a_Ks;                
uniform float a_shininess;         
                                                                                                         
                                                                                                         
vec3 blinnPhongDir(Light light, float Kd, float Ks, float shininess, vec3 N)
{                                                                                                        
                                                                                             
    vec3 surfacePos = TBN* vec3(model * vec4(ourPos, 1.0));                                      
    vec3 v = normalize(TBN * _cameraPosition - surfacePos);                                      
       
                                                                                 
    float lightInt = light.properties[0];
           
                                                                         
    vec3 normal =  N ;                           
                                                                                                         
    if (light.type != 0) {                                                                               
                                                                                                         
                                                                                     
       
        vec3 s =  normalize(TBN * light.position );
       
        vec3 r = reflect(-s,normal);   
 
        float cosTheta = clamp( dot(s, normal), 0,1 );                                                               
        float cosAlpha = clamp( dot( v,r ), 0,1 );                                                                                               
   
                                                                                         
        float diffuse = Kd * lightInt * cosTheta;
        float spec =  Ks* lightInt* pow(cosAlpha, shininess);  
                     
        return vec3(diffuse,spec,1);                                                                     
                                                                                                         
    }                                                                                                    
                                                                                                         
    else {     
        vec3 lightpos =  TBN*light.position;                                                                             
        vec3 s =  normalize(lightpos - surfacePos);      
        vec3 r = reflect(-s,normal);
 
        float cosTheta = clamp( dot( s,normal ), 0,1 );      
        float cosAlpha = clamp( dot( v,r ), 0,1 );                                               
                                                                                                         
        float d = length((lightpos - surfacePos));
        float attenuation =1/(light.properties[1] + light.properties[2]* d + light.properties[3] * d*d);
        attenuation *= lightInt;                                   
        float diffuse = attenuation * Kd  * cosTheta;                    
        float spec = attenuation * Ks * pow(cosAlpha, shininess);
                                                                                                                 
        return vec3(diffuse,spec,attenuation);                                                               
                                                                                                                 
                                                                                                                 
    }                                                                                                            
                                                                                                                 
                                                                                                                 
}                                                                                                                
                                                                                                                 
                                                                                                                 
void main()                                                                                                
{                                                                                                                
                                                                                             
                                                     
    vec3 color_texture = texture(albedo, TexCoord).xyz;                                                          
    vec3 N = normalize(texture(normal_map,TexCoord).xyz*2-1);                                                        
    vec3 occlusion_texture = texture(occlusion_map,TexCoord).xyz;                                                
    vec3 spec_texture = texture(specular_map, TexCoord).xyz;

    if(invert_norms)
    N.g = -N.g;    
    N.r = -N.r;                                                  
                                                                                                                 
    vec3 inten = vec3(0); vec3 inten_final = vec3(0);                                                                                    
                                                                         
    vec4 light_colors[MAX_LIGHTS];
       
    float final_ambient = 0.0;                                                                                             
    vec3 final_color = vec3(0);
                                                                         
    for (int i = 0; i <_numLights; ++i) {                                                                        
        inten = blinnPhongDir(_lights[i], a_Kd, a_Ks, a_shininess, N);             
        inten_final.xy += inten.xy;                                                                        
        light_colors[i] = vec4(_lights[i].l_color.rgb,inten.z);                                            
    }                                                                                                      
                                                                           
    for (int i = 0; i<_numLights; ++i) {                                                                   
        final_color += vec3(light_colors[i]) * light_colors[i].a;                                          
        final_ambient += _lights[i].ambientCoefficient;
    }      
                                                                                               
    final_ambient = final_ambient/_numLights;
    final_color =final_color;  
                                                               
    vec3 col = max(0.5 * final_ambient* color_texture* vec3(0,1,1),
     color_texture * (inten_final.x + inten_final.y * spec_texture.r)*occlusion_texture*final_color.rgb);
                                                                                       
    color = vec4(col,_alpha);
 
}