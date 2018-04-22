#include "Materials.h"
#include "parson.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ModuleRenderer3D.h"
#include "CubeMap_Texture.h"
Material::Material()
{
	active_num = 0;
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

void Material::Save() const
{
	JSON_Value* config_file;
	JSON_Object* object;

	char ownPth[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, ownPth);
	std::string directory_assets = ownPth;
	directory_assets += "\\Assets";

	directory_assets += "/Shaders/Materials/";
	directory_assets += name;
	directory_assets += ".material";
	config_file = json_value_init_object();


	object = json_value_get_object(config_file);
	json_object_clear(object);

	json_object_dotset_string_with_std(object, name + "ShaderName:", material_shader->name.c_str());
	json_object_dotset_number_with_std(object, name + "Num Textures:", textures.size());
	json_object_dotset_number_with_std(object, name + "Alpha:", alpha);
	json_object_dotset_boolean_with_std(object, name + "Glow:", glow);

	for (int i = 0; i < textures.size(); i++)
	{
		char mat_name[128] = { 0 };
		char mat_var[128] = { 0 };

		char* num = new char[4];
		itoa(i, num, 10);
		strcat(mat_name, "Resource Material UUID ");
		strcat(mat_name, num);

		strcat(mat_var, "Resource Material Var Name ");
		strcat(mat_var, num);

		RELEASE_ARRAY(num);

		if (textures[i].value != nullptr && textures[i].value->GetTextureID() != App->renderer3D->id_checkImage)
		{
			json_object_dotset_number_with_std(object, name + mat_name, textures[i].value->GetUUID());
		}
		else
		{
			json_object_dotset_number_with_std(object, name + mat_name, 0);
		}
		json_object_dotset_string_with_std(object, name + mat_var, textures[i].var_name.c_str());
	}
	json_object_dotset_number_with_std(object, name + "Num Bools:", bool_variables.size());
	for (int i = 0; i < bool_variables.size(); i++)
	{
		std::ostringstream ss;
		ss << name << "Bool:" << i;
		std::string json_name = ss.str();

		std::ostringstream ss2;
		ss2 << name << "Bool Name:" << i;
		std::string json_name_var = ss2.str();

		json_object_dotset_boolean_with_std(object, json_name, bool_variables[i].value);
		json_object_dotset_string_with_std(object, json_name_var, bool_variables[i].var_name.c_str());

	}
	json_object_dotset_number_with_std(object, name + "Num Ints:", int_variables.size());
	for (int i = 0; i < int_variables.size(); i++)
	{
		std::ostringstream ss;
		ss << name << "Int:" << i;
		std::string json_name = ss.str();

		std::ostringstream ss2;
		ss2 << name << "Int Name:" << i;
		std::string json_name_var = ss2.str();

		json_object_dotset_number_with_std(object, json_name, int_variables[i].value);
		json_object_dotset_string_with_std(object, json_name_var, int_variables[i].var_name.c_str());

	}
	json_object_dotset_number_with_std(object, name + "Num Floats:", float_variables.size());
	for (int i = 0; i < float_variables.size(); i++)
	{
		std::ostringstream ss;
		ss << name << "Float:" << i;
		std::string json_name = ss.str();

		std::ostringstream ss2;
		ss2 << name << "Float Name:" << i;
		std::string json_name_var = ss2.str();

		json_object_dotset_number_with_std(object, json_name, float_variables[i].value);
		json_object_dotset_string_with_std(object, json_name_var, float_variables[i].var_name.c_str());

	}
	json_object_dotset_number_with_std(object, name + "Num Float3:", float3_variables.size());
	for (int i = 0; i < float3_variables.size(); i++)
	{
		std::ostringstream ss;
		ss << name << "Float3:" << i;
		std::string json_name = ss.str();

		std::ostringstream ss2;
		ss2 << name << "Float3 Name:" << i;
		std::string json_name_var = ss2.str();

		App->fs->json_array_dotset_float3(object, json_name, float3_variables[i].value);
		json_object_dotset_string_with_std(object, json_name_var, float3_variables[i].var_name.c_str());
	}

	json_object_dotset_number_with_std(object, name + "Num Colors:", color_variables.size());
	for (int i = 0; i < color_variables.size(); i++)
	{
		std::ostringstream ss;
		ss << name << "Color:" << i;
		std::string json_name = ss.str();

		std::ostringstream ss2;
		ss2 << name << "Color Name:" << i;
		std::string json_name_var = ss2.str();

		App->fs->json_array_dotset_float4(object, json_name, color_variables[i].value);
		json_object_dotset_string_with_std(object, json_name_var, color_variables[i].var_name.c_str());


	}

	json_object_dotset_number_with_std(object, name + "Num Cubes:", cube_maps.size());
	for (int i = 0; i < cube_maps.size(); i++)
	{

		std::ostringstream ss;
		ss << name << "Cube:" << i;
		std::string json_name = ss.str();

		std::ostringstream ss2;
		ss2 << name << "Cube Name:" << i;
		std::string json_name_var = ss2.str();

		if (cube_maps[i].var_name != "" && cube_maps[i].cube_name != "") {

			json_object_dotset_string_with_std(object, json_name, cube_maps[i].cube_name.c_str());
			json_object_dotset_string_with_std(object, json_name_var, cube_maps[i].var_name.c_str());

		}
		else {
			json_object_dotset_string_with_std(object, json_name, "");
			json_object_dotset_string_with_std(object, json_name_var, "");
		}
	}

	json_serialize_to_file(config_file, directory_assets.c_str());
	json_value_free(config_file);
}

void Material::Load()
{

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
	auto temp_cube_maps = cube_maps;

	textures.clear();
	float3_variables.clear();
	float_variables.clear();
	int_variables.clear();
	bool_variables.clear();
	color_variables.clear();
	cube_maps.clear();


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

		//CubeMaps
		if (temp.type == GL_SAMPLER_CUBE)
		{
			CubeMapVar cube_var;
			cube_var.var_name = temp.name;
			cube_maps.push_back(cube_var);
		}

	}

	for (auto item = temp_textures.begin(); item != temp_textures.end(); item++)
	{
		for (auto item2 = textures.begin(); item2 != textures.end(); item2++)
		{
			if (strcmp((*item).var_name.c_str(), (*item2).var_name.c_str()) == 0) {
				(*item2).value = (*item).value;
				(*item2).selected = (*item).selected;
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

	for (auto item = temp_cube_maps.begin(); item != temp_cube_maps.end(); item++)
	{
		for (auto item2 = cube_maps.begin(); item2 != cube_maps.end(); item2++)
		{
			if (strcmp((*item).var_name.c_str(), (*item2).var_name.c_str()) == 0) {
				(*item2).cube_map = (*item).cube_map;
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
	it_cube_maps = cube_maps.begin();
}
