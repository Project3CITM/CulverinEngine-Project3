
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

void ShaderProgram::CreateMaterialFile()
{
	const char* buffer;


	JSON_Object* obj_proj;
	JSON_Value* file_proj;
	App->json_seria->Create_Json_Doc(&file_proj, &obj_proj, path.c_str());
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
	json_object_clear(obj_proj);
	json_value_free(file_proj);

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



	return true;
}

