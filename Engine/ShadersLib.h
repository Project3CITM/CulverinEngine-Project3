// MathLibrary.h - Contains declaration of Function class  
#pragma once  

#include "MathGeoLib.h"


#include <string>
#include <list>
#include <vector>
#include "GL3W\include\glew.h"
#include "Resource_.h"
#include "ResourceMaterial.h"
#include "TextEditor.h"

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

struct AttributeVar {

	char name[64] = "0";
	GLenum type = GL_ZERO;

};
struct TextureVar {
	std::string var_name;
	bool selected = false;
	ResourceMaterial* value = nullptr;
	uint uuid_to_reimport = 0;
};

struct float3Var {
	std::string var_name;
	float3 value = float3::zero;
};

struct ColorVar {
	std::string var_name;
	float4 value = float4(255,255,255,255);
};

struct floatVar {
	std::string var_name;
	float value = 1;
};

struct intVar {
	std::string var_name;
	int value = 1;
};

struct boolVar {
	std::string var_name;
	bool value = false;
};

class Shader {
public:
	Shader() {

	}
	~Shader() {

	}

	char* GetShaderText();
	std::string LogShaderLastError();

public:
	uint shaderID = 0;

	std::string name = "";

	std::string shaderPath = "";

	std::string shaderText = "";

	ShaderType shaderType = ShaderType::nullShader;

};


class ShaderProgram {
public:
	ShaderProgram() {
		
	}
	~ShaderProgram() {
		shader_editor.Clear();
	}

	void AddFragment(Shader* fragment);
	void AddVertex(Shader* vertex);
	void  AddGeometry(Shader* geometry);

	bool  LoadProgram();
	bool UpdateShaderProgram(uint fragmentID, uint vertexID, uint geometryID);

	bool  Bind();

	void  Unbind();

	void CreateMaterialFile();

	std::string LogProgramLastError();



public:
	std::string name;
	std::string path;
	uint programID = 0;

	Shader* fragment = nullptr;
	Shader* vertex = nullptr;
	Shader* geometry = nullptr;

	//Variables Vector
	

	TextEditor shader_editor;

	bool get_shader_text = false;

	bool edit_fragment = false;
	bool edit_vertex = false;
	bool edit_geometry = false;
};


