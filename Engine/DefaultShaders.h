#ifndef _DEFAULT_SHADERS_
#define _DEFAULT_SHADERS_



#include "GL3W\include\glew.h"

#pragma comment (lib, "GL3W/libx86/glew32.lib") 
typedef unsigned int uint;

static const GLchar* vertexShaderSource[] =
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

static const GLchar* fragmentShaderSource[] =
{
	"#version 330 core\n"
	"in vec4 ourColor;\n"
	"in float ourTime;\n"
	"in vec2 TexCoord;\n"
	"in vec3 ourNormal;\n"
	"in vec4 gl_FragCoord;\n"
	"out vec4 color;\n"
	"uniform sampler2D albedo;\n"

	"void main()\n"
	"{\n"
	"vec3 lightDir = vec3(1);\n"
	"float angle = dot(lightDir, ourNormal);\n"

	//Z-Buffer Line Shader
	"color= vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1) *texture(albedo, TexCoord);\n"
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

#endif // !_DEFAULT_SHADERS_