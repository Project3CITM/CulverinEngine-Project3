
#include "ShadersLib.h"  
#include"Application.h"
#include"ModuleFS.h"
#include"JSONSerialization.h"
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

GLint ShaderProgram::GetVariablesSize() const
{
	GLint total = 0;
	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &total);
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

