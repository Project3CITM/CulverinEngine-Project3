#ifndef MODULE_SHADERS_H_
#define MODULE_SHADERS_H_
#include"Module.h"
#include "MathGeoLib.h"


#include <string>
#include <list>
#include <vector>
#include "GL3W\include\glew.h"
#include "Resource_.h"
#include "ResourceMaterial.h"
#include "ShadersLib.h"

#pragma comment (lib, "GL3W/libx86/glew32.lib") 
typedef unsigned int uint;


class ModuleShaders:public Module
{
public:

	ModuleShaders();
	~ModuleShaders();

	bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool SaveConfig(JSON_Object* node);

	bool CleanUp();

	ShaderProgram * CreateShader(char* name);

	ShaderProgram*   CreateDefaultShader();

	Shader*   CompileShader(std::string path, std::string name, ShaderType type);

	void   AddShaderList(Shader* newShader);

	void ImportShaders();


	std::string Shader_Directory_fs;
	std::vector<Shader*> shaders;
	std::vector<ShaderProgram*> programs;
};


#endif
