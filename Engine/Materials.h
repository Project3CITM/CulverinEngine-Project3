#ifndef _MATERIALS_
#define _MATERIALS_

#include <string>
#include <vector>
#include "ShadersLib.h"


enum MaterialBlendModes
{
	MGlZero = 0,
	MGlOne,
	MGlSrcColor,
	MGlOneMinusSrcColor,
	MGlDstColor,
	MGlOneMinusDstColor,
	MGlSrcAlpha,
	MGlOneMinusSrcAlpha,
	MGlDstAlpha,
	MGlOneMinusDstAlpha,
	MGlSrcAlphaSaturate
};

class Material {
public:
	Material();

	~Material();


	bool  Bind();

	void  Unbind();

	void Save();
	void Load();

	UniformVar  GetVariableInfo(uint index);
	AttributeVar GetAttributeInfo(uint index);

	GLint  GetVariablesSize()const;
	GLint  GetAttributesSize()const;

	void GetProgramVariables();
	uint GetProgramID() const;
	
	void RestartIterators();

	// BLEND MODES ------------------------
	void SetSourceBlendMode();
	void SetDestinyBlendMode();
	//----------------

public:

	ShaderProgram* material_shader;
	std::string name = "";
	std::string path = "";

	//ALPHA
	float alpha = 1.0f;

	//GLOW
	bool glow = false;

	//BLEND MODES
	MaterialBlendModes source_type = MaterialBlendModes::MGlOne;
	int m_source_type = GL_ONE; 
	MaterialBlendModes destiny_type = MaterialBlendModes::MGlOneMinusSrcAlpha;
	int m_destiny_type = GL_ONE_MINUS_SRC_ALPHA;

	//Variables Vector
	std::vector<TextureVar>	textures;
	std::vector<intVar>		int_variables;
	std::vector<floatVar>	float_variables;
	std::vector<float3Var>	float3_variables;
	std::vector<ColorVar>	color_variables;
	std::vector<boolVar>	bool_variables;
	std::vector<CubeMapVar>	cube_maps;


	std::vector<TextureVar>::iterator	it_textures;
	std::vector<intVar>::iterator		it_int_variables;
	std::vector<floatVar>::iterator		it_float_variables;
	std::vector<float3Var>::iterator	it_float3_variables;
	std::vector<ColorVar>::iterator		it_color_variables;
	std::vector<boolVar>::iterator		it_bool_variables;
	std::vector<CubeMapVar>::iterator	it_cube_maps;

	uint active_num = 0;
};


#endif


