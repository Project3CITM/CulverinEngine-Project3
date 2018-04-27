#ifndef _DEFAULT_SHADERS_
#define _DEFAULT_SHADERS_



#include "GL3W\include\glew.h"

#pragma comment (lib, "GL3W/libx86/glew32.lib") 
typedef unsigned int uint;

static const GLchar* vertexShaderSource[] =
{
	"	#version 330 core					\n"
"	layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec2 texCoord;\n"
"layout(location = 2) in vec3 normal;\n"
"layout(location = 5) in vec3 tangent;\n"
"layout(location = 6) in vec3 bitangent;\n"
"\n"
"out float ourTime;\n"
"out vec4 ourColor;\n"
"out vec3 ourNormal;\n"
"out vec2 TexCoord;\n"
"out vec3 ourPos;\n"
"out mat3 TBN;\n"
"out vec3 FragPos;\n"
"\n"
"uniform float _time;\n"
"uniform vec4 _color;\n"
"uniform mat4 model;\n"
"uniform mat4 viewproj;\n"
"uniform mat4 view;\n"
"uniform mat4 modelview;\n"
"\n"
"\n"
"uniform sampler2D height_map;\n"
"\n"
"void main()\n"
"{\n"
"	TexCoord = texCoord;\n"
"	ourTime = _time;\n"
"	ourPos = position;\n"
"	ourNormal = normal;\n"
"\n"
"\n"
"	vec3 vertexTangent_cameraspace = mat3(modelview) * tangent;\n"
"	vec3 vertexBitangent_cameraspace = mat3(modelview) * bitangent;\n"
"	vec3 vertexNormal_cameraspace = mat3(modelview) * normal;\n"
"\n"
"	vec3 T = normalize(vec3(model * vec4(tangent, 0)));\n"
"	vec3 B = normalize(vec3(model * vec4(bitangent, 0)));\n"
"	vec3 N = normalize(vec3(model * vec4(normal, 0)));\n"
"	TBN = transpose(mat3(T,B,N));\n"
"	FragPos = TBN * vec3(model) * position;\n"
"\n"
"\n"
"	ourColor = vec4(B,1);\n"
"	gl_Position = viewproj *  model * vec4(position, 1.0f);\n"
"}\n"
"\n"
};

static const GLchar* fragmentShaderSource[] =
{
	"	#version 330 core\n"
	"\n"
	"#define MAX_LIGHTS 40	\n"
	"	uniform int _numLights;\n"
	"uniform struct Light {	\n"
	"	vec3 position;		\n"
	"	int type;\n"
	"	vec4 l_color; //a.k.a the color of the light\n"
	"	vec4 properties;\n"
	"	float ambientCoefficient;\n"
	"	float radius;\n"
	"\n"
	"\n"
	"} _lights[MAX_LIGHTS];\n"
	"					\n"
	"in vec4 ourColor;	\n"
	"in vec2 TexCoord;	\n"
	"in vec3 ourNormal;	\n"
	"in vec3 ourPos;	\n"
	"in mat3 TBN;		\n"
	"in vec3 FragPos;	\n"
	"					\n"
	"uniform vec4 diff_color;\n"
	"out vec4 color;\n"
	"uniform sampler2D albedo;\n"
	"uniform sampler2D normal_map;\n"
	"uniform sampler2D occlusion_map;\n"
	"uniform sampler2D specular_map;\n"
	"\n"
	"uniform float a;\n"
	"uniform float b;\n"
	"uniform float c;\n"
	"uniform float e;\n"
	"\n"
	"uniform bool invert_norms;\n"
	"\n"
	"uniform mat4 viewproj;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"\n"
	"uniform vec3 _cameraPosition;\n"
	"uniform float _alpha;\n"
	"\n"
	"uniform float a_LightInt;\n"
	"uniform float a_Ka;\n"
	"uniform float a_Kd;\n"
	"uniform float a_Ks;\n"
	"uniform float a_shininess;\n"
	"\n"
	"\n"
	"vec3 blinnPhongDir(Light light, float Kd, float Ks, float shininess, vec3 N) \n"
	"{\n"
	"\n"
	"	vec3 surfacePos = TBN* vec3(model * vec4(ourPos, 1.0));	\n"
	"	vec3 v = normalize(TBN * _cameraPosition - surfacePos);	\n"
	"\n"
	"\n"
	"	float lightInt = light.properties[0];\n"
	"\n"
	"\n"
	"	vec3 normal = N;\n"
	"\n"
	"	if (light.type != 0) {\n"
	"\n"
	"\n"
	"\n"
	"		vec3 s = normalize(TBN * light.position);\n"
	"\n"
	"		vec3 r = reflect(-s,normal);\n"
	"\n"
	"		float cosTheta = clamp(dot(s, normal), 0,1);\n"
	"		float cosAlpha = clamp(dot(v,r), 0,1);\n"
	"\n"
	"\n"
	"		float diffuse = Kd * lightInt * cosTheta;\n"
	"		float spec = Ks* lightInt* pow(cosAlpha, shininess);\n"
	"\n"
	"		return vec3(diffuse,spec,1);\n"
	"\n"
	"	}\n"
	"\n"
	"	else {\n"
	"		vec3 lightpos = TBN*light.position;\n"
	"		vec3 s = normalize(lightpos - surfacePos);\n"
	"		vec3 r = reflect(-s,normal);\n"
	"\n"
	"		float cosTheta = clamp(dot(s,normal), 0,1);\n"
	"		float cosAlpha = clamp(dot(v,r), 0,1);\n"
	"\n"
	"		float d = length((lightpos - surfacePos));\n"
	"		float attenuation = 1 / (light.properties[1] + light.properties[2] * d + light.properties[3] * d*d);  \n"
	"		attenuation *= lightInt;\n"
	"		float diffuse = attenuation * Kd  * cosTheta;\n"
	"		float spec = attenuation * Ks * pow(cosAlpha, shininess);\n"
	"\n"
	"		return vec3(diffuse,spec,attenuation);\n"
	"\n"
	"\n"
	"}\n"
	"\n"
	"\n"
	"}\n"
	"\n"
	"\n"
	"void main()\n"
	"{\n"
	"\n"
	"\n"
	"	vec3 color_texture = texture(albedo, TexCoord).xyz;			\n"
	"	vec3 N = normalize(texture(normal_map,TexCoord).xyz * 2 - 1);\n"
	"	vec3 occlusion_texture = texture(occlusion_map,TexCoord).xyz;\n"
	"	vec3 spec_texture = texture(specular_map, TexCoord).xyz * 255;\n"
	"\n"
	"	if (invert_norms)\n"
	"		N.g = -N.g;	\n"
	"	N.r = -N.r;\n"
	"\n"
	"	vec3 inten = vec3(0); vec3 inten_final = vec3(0);\n"
	"\n"
	"	vec4 light_colors[MAX_LIGHTS];\n"
	"\n"
	"	float final_ambient = 0.0;\n"
	"	vec3 final_color = vec3(0);\n"
	"\n"
	"	for (int i = 0; i <_numLights; ++i) {\n"
	"		inten = blinnPhongDir(_lights[i], a_Kd, a_Ks,spec_texture.r, N);\n"
	"		inten_final.xy += inten.xy;\n"
	"		light_colors[i] = vec4(_lights[i].l_color.rgb,inten.z);\n"
	"	}\n"
	"\n"
	"	for (int i = 0; i<_numLights; ++i) {\n"
	"		final_color += vec3(light_colors[i]) * light_colors[i].a;\n"
	"		final_ambient += _lights[i].ambientCoefficient;\n"
	"	}\n"
	"\n"
	"	final_ambient = final_ambient / _numLights;\n"
	"	final_color = final_color;\n"
	"\n"
	"	vec3 col = max( color_texture,\n"
	"		color_texture * (inten_final.x + inten_final.y)*occlusion_texture*final_color.rgb);\n"
	"\n"
	/*
	"	vec3 col = max(final_ambient* color_texture* vec3(0,1,1),\n"
	"		color_texture * (inten_final.x + inten_final.y)*occlusion_texture*final_color.rgb);\n"
	"\n"
	*/
	/*
	Default_frag
	vec3 col = max(final_ambient* color_texture* vec3(0,0.9,0.9),
	color_texture * (inten_final.x + inten_final.y )*occlusion_texture*final_color.rgb);
	*/
	"	color = vec4(col,_alpha); \n"
	"\n"
	"}\n"
};

static const GLchar* DefaultVert[] =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"
	"layout(location = 2) in vec3 normal;\n"
	"layout(location = 3) in vec4 color;\n"
	"out float ourTime;\n"
	"out vec4 ourColor;\n"
	"out vec3 ourNormal;\n"
	"out vec2 TexCoord;\n"
	"uniform float _time;\n"
	"uniform vec4 _color;\n"
	"uniform mat4 model;\n"
	"uniform mat4 viewproj;\n"
	"uniform mat4 view;\n"
	"void main()\n"
	"{\n"
	"gl_Position = viewproj *  model * vec4(position.x,position.y,position.z, 1.0f);\n"
	"ourColor = _color;\n"
	"TexCoord = texCoord;\n"
	"ourTime = _time;\n"
	"ourNormal = mat3(model) * normal;"
	"}\n"

};

static const GLchar* TextureVert[] =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"


	"out vec2 TexCoord;\n"

	"void main()\n"
	"{\n"
	"gl_Position = vec4(position, 1.0f);\n"
	"TexCoord = texCoord;\n"


	"}\n"
};

static const GLchar* ParticleVert[] =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"
	"layout(location = 2) in mat4 transform;\n"
	"layout(location = 3) in vec4 _color;\n"
		
	"out vec4 ourColor;\n"
	"out vec2 TexCoord;\n"

	"uniform mat4 viewproj;\n"
	

	"void main()\n"
	"{\n"
	"gl_Position = viewproj * transform *vec4(position.x,position.y,position.z, 1.0f) ;\n"
	"ourColor = _color;\n"
	"TexCoord = texCoord;\n"
	"}\n"
};

static const GLchar* BlurFrag[] =
{
	"#version 330 core\n"

	"in vec2 TexCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D albedo;\n"
	"uniform int _orientation;\n"
	"uniform int BlurAmount;\n"
	"uniform float BlurScale;\n"
	"uniform float BlurStrength;\n"
	"float Gaussian(float x, float deviation)\n"
	"{\n"
	"	return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));\n"
	"}\n"

	"void main()\n"
	"{\n"

	"float halfBlur = float(BlurAmount) * 0.5;\n"
	"vec4 colour = vec4(0.0);\n"
	"vec4 texColour = vec4(0.0);\n"


	"float deviation = halfBlur * 0.35;\n"
	"deviation *= deviation;\n"
	"float strength = 1.0 - BlurStrength;\n"

	"if(_orientation == 0){\n"
	"	// Horizontal blur\n"
	"	for (int i = 0; i < BlurAmount; ++i)\n"
	"	{\n"
	"		if (i >= BlurAmount)\n"
	"			break;\n"
	"\n"
	"		float offset = float(i) - halfBlur;\n"
	"		texColour = texture2D(albedo, TexCoord + vec2(BlurScale * offset/128 , 0)) * Gaussian(offset * strength, deviation);\n"
	"		colour += texColour;\n"
	"	}\n"
	"}\n"
	"else {\n"
	"	// Horizontal blur\n"
	"	for (int i = 0; i < BlurAmount; ++i)\n"
	"	{\n"
	"		if (i >= BlurAmount)\n"
	"			break;\n"
	"\n"
	"		float offset = float(i) - halfBlur;\n"
	"		texColour = texture2D(albedo, TexCoord + vec2(0,BlurScale * offset /128)) * Gaussian(offset * strength, deviation);\n"
	"		colour += texColour;\n"
	"	}\n"
	"}\n"

	"color= vec4(colour);\n"
	"}\n"
};

static const GLchar* FinalFrag[] =
{
	"#version 330 core\n"
	"in vec4 ourColor;\n"
	"in float ourTime;\n"
	"in vec2 TexCoord;\n"
	"in vec3 ourNormal;\n"
	"in vec4 gl_FragCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D _albedo;\n"
	"uniform sampler2D _glow_tex;\n"
	"uniform sampler2D _dmg_tex;\n"
	"uniform vec4 _my_color;\n"
	"uniform bool damage;\n"
	"uniform float alpha; \n"
	"uniform float mult_dead;\n"
	"void main()\n"
	"{\n"
	"vec4 dst = texture2D(_albedo, TexCoord);\n" // rendered scene
	"vec4 src = texture2D(_glow_tex, TexCoord); \n" // glowmap
	"vec4 dmg = texture(_dmg_tex,TexCoord);\n"
	"if(damage)\n"
	"color = mix(min(vec4(src.rgb * mult_dead,1) + vec4(dst.rgb * mult_dead,1), 1.0) , dmg, dmg.a * alpha );\n"
	"else\n"
	"color= min(vec4(src.rgb * mult_dead,1) + vec4(dst.rgb * mult_dead,1) , 1.0);\n"

	"}\n"
};

static const GLchar* DefaultFrag[] =
{
	"#version 330 core\n"
	"in vec4 ourColor;\n"
	"in float ourTime;\n"
	"in vec2 TexCoord;\n"
	"in vec3 ourNormal;\n"
	"in vec4 gl_FragCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D albedo;\n"
	"uniform vec4 _my_color;\n"
	"void main()\n"
	"{\n"

	//Z-Buffer Line Shader
	"color= _my_color * texture(albedo, TexCoord);\n"
	"}\n"
};

static const GLchar* ParticleFrag[] =
{
	"#version 330 core\n"
	"in vec4 ourColor;\n"	
	"in vec2 TexCoord;\n"
	
	"out vec4 color;\n"

	"uniform sampler2D albedo;\n"


	"void main()\n"
	"{\n"

	//Z-Buffer Line Shader
	"color= ourColor * texture(albedo, TexCoord);\n"
	"}\n"
};

static const GLchar* NonGlowFrag[] =
{
	"#version 330 core\n"
	"in vec4 ourColor;\n"
	"in float ourTime;\n"
	"in vec2 TexCoord;\n"
	"in vec3 ourNormal;\n"
	"in vec4 gl_FragCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D albedo;\n"
	"uniform vec4 _my_color;\n"
	"void main()\n"
	"{\n"

	//Z-Buffer Line Shader
	"color= vec4(vec3(0),1);\n"
	"}\n"
};


static const GLchar* ShadowMapVert[] =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"
	"layout(location = 2) in vec3 normal;\n"
	"layout(location = 3) in vec4 color;\n"

	"uniform mat4 depthMVP;\n"
	"uniform mat4 model;\n"

	"void main()\n"
	"{\n"
	" gl_Position =  depthMVP *model * vec4(position,1);\n"
	"}\n"
};


static const GLchar* ShadowMapFrag[] =
{
	"#version 330 core\n"
	"layout(location = 0) out vec4 fragmentdepth;\n"

	"void main()\n"
	"{\n"
	"fragmentdepth = vec4(gl_FragCoord.z);\n"
	"}\n"
};
static const GLchar* CubeMapVert[] =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"
	"layout(location = 2) in vec3 normal;\n"
	"layout(location = 3) in vec4 color;\n"

	"uniform mat4 viewproj;\n"
	"uniform mat4 model;\n"

	"void main()\n"
	"{\n"
	" gl_Position =  viewproj *model * vec4(position,1);\n"
	"}\n"
};


static const GLchar* CubeMapFrag[] =
{
	"#version 330 core\n"
	"out vec4 color;\n"

	"void main()\n"
	"{\n"
	"color = vec4(1);\n"
	"}\n"
};


static const  GLchar* UIShaderVert[] =
{
	"#version 330\n"
	"uniform mat4 ProjMtx;\n"
	"uniform mat4 model;\n"
	"uniform vec4 Color_UI_ME;\n"
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec2 texCoord;\n"
	"out vec2 Frag_UV;\n"

	"void main()\n"
	"{\n"
	"	Frag_UV = texCoord;\n"
	"	gl_Position = ProjMtx * model * vec4(position.xy,0,1);\n"
	"}\n"
};


static const GLchar* UIShaderFrag[] =
{
	"#version 330\n"
	"uniform sampler2D Texture;\n"
	"uniform vec4 Color_UI_ME;\n"

	"in vec2 Frag_UV;\n"
	"in vec4 Frag_Color;\n"
	"out vec4 Out_Color;\n"
	"void main()\n"
	"{\n"
	"	Out_Color = Color_UI_ME*texture(Texture,vec2(Frag_UV.s, 1.0 - Frag_UV.t).st);\n"
	"}\n"
};

static const GLchar* PointShadowMapVert[] =
{
	"#version 330 core\n"
	"layout (location = 0) in vec3 position;\n"
	"uniform mat4 model;\n"
	"uniform mat4 viewproj;\n"
	"out vec4 FragPos;\n"
	"void main()\n"
	"{\n"
	"	FragPos = model * vec4(position, 1.0);\n"
	"	gl_Position = viewproj * model * vec4(position, 1.0);\n"
	"}\n"
};

static const GLchar* PointShadowMapGeo[] =
{
	"#version 330 core\n"
	"layout (triangles) in;\n"
	"layout (triangle_strip, max_vertices=18) out;\n"
	"uniform mat4 shadow_matrices[6];\n"
	"out vec4 FragPos;\n"
	"void main()\n"
	"{\n"
	"	for(int face = 0; face < 6; ++face)\n"
	"	{\n"
	"		gl_Layer = face;\n"
	"		for(int i = 0; i < 3; ++i)\n"
	"		{\n"
	"			FragPos = gl_in[i].gl_Position;\n"
	"			gl_Position = shadow_matrices[face] * FragPos;\n"
	"			EmitVertex();\n"
	"		}\n"
	"		EndPrimitive();\n"
	"	}\n"
	"}\n"
};

static const GLchar* PointShadowMapFrag[] =
{
	"#version 330 core\n"
	"in vec4 FragPos;\n"
	
	"uniform vec3 _light_pos;\n"
	"uniform float _far_plane;\n"
	"void main()\n"
	"{\n"
	"	float light_dist = length(FragPos.xyz - _light_pos);\n"
	"	light_dist = (light_dist-1) / (_far_plane-1); // Normalize it\n"
	"	gl_FragDepth = light_dist;\n"
	"}\n"
};

#endif 