#version 330

in vec4 ourColor;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 ourPos;


uniform mat4 model;
uniform vec3 _cameraPosition;

uniform sampler2D materialTex;
uniform float materialShininess;
uniform vec4 materialSpecularColor;
uniform float val;
#define MAX_LIGHTS 10
uniform int _numLights;
uniform struct Light {
   vec3 position;
   int type;
   vec4 l_color; //a.k.a the color of the light
   float attenuation;
   float ambientCoefficient;

} _lights[MAX_LIGHTS];





out vec4 color;

vec3 ApplyLight(Light light,int type, vec3 surfaceColor, vec3 normal, vec3 surfacePos, vec3 surfaceToCamera) {
    vec3 surfaceToLight;
    float attenuation = 1.0;
    if(type != 0) {
        //directional light
        surfaceToLight = normalize(light.position);
        attenuation = 1.0; //no attenuation for directional lights
    } else {
        //point light
        surfaceToLight = normalize(light.position - surfacePos);
        float distanceToLight = length(light.position - surfacePos);
        attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));
///////

    }
//surfaceToLight = vec3(1);
    //ambient
    vec3 ambient = surfaceColor.rgb;// light.ambientCoefficient * surfaceColor.rgb * light.l_color.rgb;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = surfaceColor.rgb;// * light.l_color.rgb;//diffuseCoefficient * surfaceColor.rgb * light.l_color.rgb;
    
    //specular
    float specularCoefficient = 0.0;
    //if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), materialShininess);
    vec3 specular =specularCoefficient * materialSpecularColor.rgb;// * light.l_color.rgb;

    //linear color (color before gamma correction)
    return ambient + attenuation* (diffuse+specular);//+ specular);
}
//DONE


void main() {
    vec3 normal = normalize(transpose(inverse(mat3(model))) * ourNormal);
    vec3 surfacePos =vec3(model * vec4(ourPos, 1));
    vec4 surfaceColor = texture(materialTex, TexCoord);
    vec3 surfaceToCamera = normalize(_cameraPosition - surfacePos);

    //combine color from all the lights
    vec3 linearColor = vec3(0);
    for(int i = 0; i < _numLights; ++i){
        linearColor += ApplyLight(_lights[i],_lights[i].type, surfaceColor.rgb, ourNormal, surfacePos, surfaceToCamera);
    } //OurNormal = normal
    
    //final color (after gamma correction)
    vec3 gamma = vec3(val);
    color =vec4(pow(linearColor, gamma), surfaceColor.a);
}