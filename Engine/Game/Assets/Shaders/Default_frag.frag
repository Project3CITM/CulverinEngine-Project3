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


in vec2 TexCoord;

in vec3 ourPos;
in mat3 TBN;
in vec3 FragPos;
in vec3 ourNormal;
in vec4 shadowCoord;

uniform vec4 diff_color;
out vec4 color;
uniform sampler2D albedo;
uniform sampler2D normal_map;

uniform sampler2D specular_map;
uniform sampler2D glossines_map;

uniform sampler2DShadow _shadowMap;

uniform vec3 _cameraPosition;
uniform float _alpha;

uniform float a_Ka;
uniform float a_Kd;
uniform float a_Ks;
uniform float a_shininess;

uniform int iterations;
uniform int shadow_blur;
uniform float bias;

uniform mat4 model;

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
};

vec4 light_colors[MAX_LIGHTS];

vec3 blinnPhongDir(Light light, float Kd, float Ks, float shininess, vec3 N)
{
    vec3 v = normalize(TBN * _cameraPosition - FragPos);

    float lightInt = light.properties[0];

    if (light.type != 0) {
        // Directional

       // Light direction
        vec3 s =  normalize(TBN * light.position );

        vec3 r = reflect(-s,N);

        float cosTheta = clamp( dot(s, N), 0,1 );
        float cosAlpha = clamp( dot( v,r ), 0,1 );


        float diffuse = Kd * lightInt * cosTheta;
        float spec =  Ks* lightInt* pow(cosAlpha,shininess);

        return vec3(diffuse,spec,1);

    }

    else {
    	// Point
        vec3 lightpos =  TBN*light.position;
        vec3 s =  normalize(lightpos - FragPos);      // Light direction
        vec3 r = reflect(-s,N);

        float cosTheta = clamp( dot( s,N ), 0,1 ) ;
        float cosAlpha = clamp( dot( v,r ), 0,1 );

        float d = length((lightpos - FragPos));
        float attenuation =1/(light.properties[1] + light.properties[2]* d + light.properties[3] * d*d);
        attenuation *= lightInt;
        float diffuse = attenuation * Kd  * cosTheta;
        float spec = attenuation * Ks * pow(cosAlpha,shininess);

        return vec3(diffuse,spec,attenuation);


    }


}

float CalcShadow(vec4 shadowPos, float usedBias)
{
    float shadow = 1.0;

    if(shadowPos.z > 1.0)
        return 0.0;

    for(int i = 0; i < iterations; ++i)
    {
        int index = int(16.0 * random(floor(mat3(model) * ourPos * 1000.0), i)) % 16;

        float shadowVal = (1.0f - texture(_shadowMap, vec3(shadowPos.xy + poissonDisk[index] / 200.0, (shadowPos.z - usedBias) / shadowPos.w)));
        float tmp = 0.05 * shadowVal;

        shadow -= tmp;
    }

    return shadow;
}


void main()
{
    vec3 color_texture = texture(albedo, TexCoord).xyz;
    vec3 N = normalize(texture(normal_map,TexCoord).xyz*2-1) ;
    vec3 spec_texture = exp2( texture(specular_map, TexCoord).xyz)+  0.5;
    vec3 gloss_texture =exp2(10 * abs(texture(glossines_map,TexCoord).xyz - vec3(1)) + 1) * 128;

    vec3 inten = vec3(0); vec3 inten_final = vec3(0);

    float final_ambient = 0.0;
    vec3 final_color = vec3(0);

    vec3 lightDir = vec3(0, 0, 0);
    // Iterate all lights to search the first directional light for now
    for(int i = 0; i < _numLights; ++i)
    {
        if(_lights[i].type != 0)
        {
            //Directional
            lightDir = _lights[0].position;
            break;
        }
    }

    vec3 l = normalize(lightDir);
    float cosTheta = clamp(dot(ourNormal,l),0,1);
    float usedBias = bias * tan(acos(cosTheta));
    usedBias = clamp(usedBias, 0, 0.01);


    // Shadow
    vec4 shadowPos = shadowCoord / shadowCoord.w;
    float shadow = CalcShadow(shadowPos, usedBias);

 for (int i = 0; i <_numLights; ++i) {

       inten = blinnPhongDir(_lights[i], a_Kd, spec_texture.r, gloss_texture.r, N);
       inten_final.xy += inten.xy;
       light_colors[i] = vec4(_lights[i].l_color.rgb,inten.z);

       final_color += vec3(light_colors[i]) * light_colors[i].a;
       final_ambient += _lights[i].ambientCoefficient;


   }

    final_ambient = final_ambient/_numLights;
    final_color =normalize(final_color);

	vec3 col = max( color_texture * vec3(0,0.2,0.2) ,
	color_texture * (inten_final.x + inten_final.y * spec_texture.r)*final_color.rgb);

    color = vec4(col, _alpha);
}
