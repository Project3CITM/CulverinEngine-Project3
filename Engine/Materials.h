#ifndef _MATERIALS_
#define _MATERIALS_

#include <string>
#include <vector>
#include "ShadersLib.h"

class Material {
public:
	Material();

	~Material();


	bool  Bind();

	void  Unbind();

	void Save()const;
	void Load();

	UniformVar  GetVariableInfo(uint index);
	AttributeVar GetAttributeInfo(uint index);

	GLint  GetVariablesSize()const;
	GLint  GetAttributesSize()const;

	void GetProgramVariables();
	uint GetProgramID() const;

	
	void RestartIterators();


public:

	ShaderProgram* material_shader;
	std::string name = "";
	std::string path = "";

	//ALPHA
	float alpha = 1.0f;

	//Variables Vector
	std::vector<TextureVar>	textures;
	std::vector<intVar>		int_variables;
	std::vector<floatVar>	float_variables;
	std::vector<float3Var>	float3_variables;
	std::vector<ColorVar>	color_variables;
	std::vector<boolVar>	bool_variables;

	std::vector<TextureVar>::iterator	it_textures;
	std::vector<intVar>::iterator		it_int_variables;
	std::vector<floatVar>::iterator		it_float_variables;
	std::vector<float3Var>::iterator	it_float3_variables;
	std::vector<ColorVar>::iterator		it_color_variables;
	std::vector<boolVar>::iterator		it_bool_variables;


};

#endif


