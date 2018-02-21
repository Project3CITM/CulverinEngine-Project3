#include "Materials.h"

Material::Material()
{
}

Material::~Material()
{

}

bool Material::Bind()
{
	glUseProgram(GetProgramID());
	
	return true;
}

void Material::Unbind()
{
	glUseProgram(NULL);
}

UniformVar Material::GetVariableInfo(uint index)
{
	UniformVar ret;
	int name_len = -1, num = -1;
	GLenum type = GL_ZERO;
	char name[100];
	glGetActiveUniform(GetProgramID(), GLuint(index), sizeof(ret.name) - 1,
		&name_len, &num, &ret.type, ret.name);

	ret.name[name_len] = 0;

	return ret;
}

AttributeVar Material::GetAttributeInfo(uint index)
{
	AttributeVar ret;
	int name_len = -1, num = -1;
	GLenum type = GL_ZERO;
	char name[100];
	glGetActiveAttrib(GetProgramID(), GLuint(index), sizeof(ret.name) - 1,
		&name_len, &num, &ret.type, ret.name);

	ret.name[name_len] = 0;

	return ret;
}

GLint Material::GetVariablesSize() const
{
	GLint total = 0;
	glGetProgramiv(GetProgramID(), GL_ACTIVE_UNIFORMS, &total);
	return total;
}

GLint Material::GetAttributesSize() const
{
	GLint total = 0;
	glGetProgramiv(GetProgramID(), GL_ACTIVE_ATTRIBUTES, &total);
	return total;
}

void Material::GetProgramVariables()
{
	auto temp_textures = textures;
	auto temp_float = float_variables;
	auto temp_float3 = float3_variables;
	auto temp_color = color_variables;
	auto temp_bool = bool_variables;
	auto  temp_int = int_variables;

	textures.clear();
	float3_variables.clear();
	float_variables.clear();
	int_variables.clear();
	bool_variables.clear();
	color_variables.clear();


	uint var_size = GetVariablesSize();
	for (int i = 0; i < var_size; i++)
	{
		UniformVar temp = GetVariableInfo(i);

		// Variables started with '_' reserved for global variables
		if (temp.name != nullptr && temp.name[0] == '_') continue;

		bool exist_variable = false;
		//Textures
		if (temp.type == GL_SAMPLER_2D)
		{
			TextureVar texture_var;
			texture_var.var_name = temp.name;
			textures.push_back(texture_var);
		}

		//Vec3
		if (temp.type == GL_FLOAT_VEC3)
		{
			float3Var float3_var;
			float3_var.var_name = temp.name;
			float3_variables.push_back(float3_var);
		}

		//Int
		if (temp.type == GL_INT)
		{
			intVar int_var;
			int_var.var_name = temp.name;
			int_variables.push_back(int_var);
		}

		//Float
		if (temp.type == GL_FLOAT)
		{
			floatVar float_var;
			float_var.var_name = temp.name;
			float_variables.push_back(float_var);
		}

		//Bool
		if (temp.type == GL_BOOL)
		{
			boolVar bool_var;
			bool_var.var_name = temp.name;
			bool_variables.push_back(bool_var);
		}


		//Color
		if (temp.type == GL_FLOAT_VEC4)
		{
			ColorVar color_var;
			color_var.var_name = temp.name;
			color_variables.push_back(color_var);
		}

	}

	for (auto item = temp_textures.begin(); item != temp_textures.end(); item++)
	{
		for (auto item2 = textures.begin(); item2 != textures.end(); item2++)
		{
			if (strcmp((*item).var_name.c_str(), (*item2).var_name.c_str()) == 0) {
				(*item2).value = (*item).value;
			}
		}
	}
	for (auto item = temp_float.begin(); item != temp_float.end(); item++)
	{
		for (auto item2 = float_variables.begin(); item2 != float_variables.end(); item2++)
		{
			if (strcmp((*item).var_name.c_str(), (*item2).var_name.c_str()) == 0) {
				(*item2).value = (*item).value;
			}
		}
	}

	for (auto item = temp_int.begin(); item != temp_int.end(); item++)
	{
		for (auto item2 = int_variables.begin(); item2 != int_variables.end(); item2++)
		{
			if (strcmp((*item).var_name.c_str(), (*item2).var_name.c_str()) == 0) {
				(*item2).value = (*item).value;
			}
		}
	}

	for (auto item = temp_bool.begin(); item != temp_bool.end(); item++)
	{
		for (auto item2 = bool_variables.begin(); item2 != bool_variables.end(); item2++)
		{
			if (strcmp((*item).var_name.c_str(), (*item2).var_name.c_str()) == 0) {
				(*item2).value = (*item).value;
			}
		}
	}

	for (auto item = temp_color.begin(); item != temp_color.end(); item++)
	{
		for (auto item2 = color_variables.begin(); item2 != color_variables.end(); item2++)
		{
			if (strcmp((*item).var_name.c_str(), (*item2).var_name.c_str()) == 0) {
				(*item2).value = (*item).value;
			}
		}
	}

	for (auto item = temp_float3.begin(); item != temp_float3.end(); item++)
	{
		for (auto item2 = float3_variables.begin(); item2 != float3_variables.end(); item2++)
		{
			if (strcmp((*item).var_name.c_str(), (*item2).var_name.c_str()) == 0) {
				(*item2).value = (*item).value;
			}
		}
	}
}

uint Material::GetProgramID()const
{
	return material_shader->programID;
}

void Material::RestartIterators()
{
	it_textures = textures.begin();
	it_int_variables = int_variables.begin();
	it_float_variables = float_variables.begin();
	it_float3_variables = float3_variables.begin();
	it_color_variables = color_variables.begin();
	it_bool_variables = bool_variables.begin();
}
