// MathLibrary.h - Contains declaration of Function class  
#pragma once  



#include <string>
#include <list>
#include <vector>
#include "GL3W\include\glew.h"

#pragma comment (lib, "GL3W/libx86/glew32.lib") 
typedef unsigned int uint;

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

	void AddFragment(Shader* fragment);
	void AddVertex(Shader* vertex);
	void  AddGeometry(Shader* geometry);

	bool  LoadProgram();

	bool  Bind();

	void  Unbind();

	UniformVar  GetVariableInfo(uint index);

	GLint  GetVariablesSize()const;


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


	ShaderProgram * CreateShader(char* name);

	ShaderProgram*   CreateDefaultShader();

	Shader*   CompileShader(std::string path, ShaderType type);

	void   AddShaderList(Shader* newShader);


public:


	std::vector<Shader*> shaders;
	std::vector<ShaderProgram*> programs;
};


