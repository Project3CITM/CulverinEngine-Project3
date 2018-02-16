
#include "ShadersLib.h"  
#include"Application.h"
#include"ModuleFS.h"
#include"JSONSerialization.h"
#include "ModuleRenderer3D.h"
#pragma comment (lib, "GL3W/libx86/glew32.lib") 

void ShaderProgram::AddFragment(Shader* newFragment)
{
	fragment = newFragment;
}

void ShaderProgram::AddVertex(Shader* newVertex)
{
	vertex = newVertex;
}

void ShaderProgram::AddGeometry(Shader* newGeometry)
{
	geometry = newGeometry;
}

bool ShaderProgram::LoadProgram()
{

	GLint programSuccess = GL_TRUE;

	programID = glCreateProgram();

	if (vertex != nullptr)
		glAttachShader(programID, vertex->shaderID);

	if (fragment != nullptr)
		glAttachShader(programID, fragment->shaderID);

	if (geometry != nullptr)
		glAttachShader(programID, geometry->shaderID);

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &programSuccess);

	if (programSuccess != GL_TRUE)
	{
		//printProgramLog(mProgramID);
		return false;
	}

	GetProgramVariables();

	return true;

}
bool ShaderProgram::Bind()
{
	glUseProgram(programID);

	/*GLenum error = glGetError();

	if (error != GL_NO_ERROR) {

	printf("Error binding shader! %s\n", gluErrorString(error));
	return false;
	}*/
	return true;
}

void ShaderProgram::Unbind()
{
	glUseProgram(NULL);
}

UniformVar ShaderProgram::GetVariableInfo(uint index)
{
	UniformVar ret;
	int name_len = -1, num = -1;
	GLenum type = GL_ZERO;
	char name[100];
	glGetActiveUniform(programID, GLuint(index), sizeof(ret.name) - 1,
		&name_len, &num, &ret.type, ret.name);

	ret.name[name_len] = 0;
	
	return ret;
}

AttributeVar ShaderProgram::GetAttributeInfo(uint index)
{
	AttributeVar ret;
	int name_len = -1, num = -1;
	GLenum type = GL_ZERO;
	char name[100];
	glGetActiveAttrib(programID, GLuint(index), sizeof(ret.name) - 1,
		&name_len, &num, &ret.type, ret.name);

	ret.name[name_len] = 0;

	return ret;
}

GLint ShaderProgram::GetVariablesSize() const
{
	GLint total = 0;
	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &total);
	return total;
}

GLint ShaderProgram::GetAttributesSize() const
{
	GLint total = 0;
	glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTES, &total);
	return total;	
}

void ShaderProgram::CreateMaterialFile()
{
	const char* buffer;


	JSON_Object* obj_proj;
	JSON_Value* file_proj;
	App->json_seria->Create_Json_Doc(&file_proj, &obj_proj, path.c_str());
	JSON_Object* node;
	int i = 0;

	if (fragment!=nullptr) {
		json_object_set_string(obj_proj, "Fragment Shader", fragment->name.c_str());
	}

	if (vertex != nullptr) {
		json_object_set_string(obj_proj, "Vertex Shader", vertex->name.c_str());
	}

	if (geometry != nullptr) {
		json_object_set_string(obj_proj, "Geometry Shader", geometry->name.c_str());
	}

	char* serialized_string = json_serialize_to_string_pretty(file_proj);
	json_serialize_to_file(file_proj, path.c_str());

}

std::string ShaderProgram::LogProgramLastError()
{
	std::string ret = "";
	
	int infoLogLength = 0;
	int maxLength = infoLogLength;

	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

	char* infoLog = new char[maxLength];
	glGetProgramInfoLog(programID, maxLength, &infoLogLength, infoLog);

	if (infoLogLength > 0) {

		//LOG("%s\n", infoLog);
		ret = infoLog;
	}
	
	delete[] infoLog;
	return ret;
}

std::string Shader::LogShaderLastError()
{
	std::string ret = "";
	printf("Error linking program %d!\n", shaderID);
	int infoLogLength = 0;
	int maxLength = infoLogLength;

	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

	char* infoLog = new char[maxLength];
	glGetShaderInfoLog(shaderID, maxLength, &infoLogLength, infoLog);

	if (infoLogLength > 0) {

		//LOG("%s\n", infoLog);
		ret = infoLog;
	}

	delete[] infoLog;

	return ret;
}


char * Shader::GetShaderText()
{
	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;
	std::string string_buffer;
	pFile = fopen(shaderPath.c_str(), "rb");

	if (pFile == NULL) { fputs("File error", stderr); exit(1); }

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = new char[lSize + 1];
	memset(buffer, 0, lSize + 1);

	if (buffer == NULL) { fputs("Memory error", stderr); exit(2); }

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);

	fclose(pFile);

	return buffer;
}


bool ShaderProgram::UpdateShaderProgram(uint VertexID, uint FragmentID, uint GeometryID)
{

	GLint programSuccess = GL_TRUE;


	if (VertexID != 0) {


		glDetachShader(programID, vertex->shaderID);
		glAttachShader(programID, VertexID);

		vertex->shaderID = VertexID;
	}

	if (FragmentID != 0) {
		
		glDetachShader(programID, fragment->shaderID);
		glAttachShader(programID, FragmentID);

		fragment->shaderID = FragmentID;
	}

	if (GeometryID != 0) {
		
		glDetachShader(programID, geometry->shaderID);
		glAttachShader(programID, GeometryID);

		fragment->shaderID = GeometryID;
	}

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &programSuccess);

	if (programSuccess != GL_TRUE) {	

		return false;
	}


	GetProgramVariables();

	return true;
}


void ShaderProgram::GetProgramVariables()
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
		if (temp.type == GL_FLOAT_VEC3 ) 
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
			if (strcmp((*item).var_name.c_str(), (*item2).var_name.c_str())==0) {
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

void ShaderProgram::RestartIterators()
{
	it_textures = textures.begin();
	it_int_variables = int_variables.begin();
	it_float_variables = float_variables.begin();
	it_float3_variables = float3_variables.begin();
	it_color_variables =color_variables.begin();
	it_bool_variables = bool_variables.begin();
}

