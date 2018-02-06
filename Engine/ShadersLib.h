// MathLibrary.h - Contains declaration of Function class  
#pragma once  

#ifdef SHADERS_EXPORTS  
#define SHADERS_API __declspec(dllexport)   
#else  
#define SHADERS_API __declspec(dllimport)   
#endif  


#include <string>
#include <list>
#include "GL3W\include\glew.h"

#pragma comment (lib, "GL3W/libx86/glew32.lib") 
typedef unsigned int uint;

namespace ShaderLib {

	enum ShaderType {
		nullShader,
		fragment,
		geometry,
		vertex,
	};
	

	struct UniformVar {
		
		char name[64] = "0";
		GLenum type = GL_ZERO;		

	};

	class Shader {
	public:
		Shader() {

		}
		~Shader() {

		}
	public:
		uint shaderID = 0;

		std::string shaderPath = "";

		std::string shaderText = "";

		ShaderType shaderType = ShaderType::nullShader;

	};


	class ShaderProgram {
	public:
		ShaderProgram() {

		}
		~ShaderProgram() {

		}

		SHADERS_API void _stdcall AddFragment(Shader* fragment);
		SHADERS_API void _stdcall AddVertex(Shader* vertex);
		SHADERS_API void _stdcall AddGeometry(Shader* geometry);

		SHADERS_API bool _stdcall LoadProgram();

		SHADERS_API bool _stdcall Bind();

		SHADERS_API void _stdcall Unbind();

		SHADERS_API UniformVar _stdcall GetVariableInfo(uint index);

		SHADERS_API GLint _stdcall GetVariablesSize()const;


	public:
		std::string name;

		uint programID = 0;

		Shader* fragment = nullptr;
		Shader* vertex = nullptr;
		Shader* geometry = nullptr;

	};


	class ShaderManager
	{
	public:

		
		 SHADERS_API ShaderProgram* _stdcall CreateShader(char* name);

		 SHADERS_API ShaderProgram*  _stdcall CreateDefaultShader();

		 SHADERS_API Shader*  _stdcall CompileShader(std::string path, ShaderType type);
	 
		 SHADERS_API void  _stdcall AddShaderList(Shader* newShader);
		

	public:
		

		 std::list<Shader*> shaders;
		 std::list<ShaderProgram*> programs;
	};

}

