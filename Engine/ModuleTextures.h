#ifndef _MODULETEXTURE_
#define _MODULETEXTURE_

#include "Module.h"
#include "Globals.h"
#include "GL3W/include/glew.h"

class BaseObject;

class ModuleTextures : public Module
{
public:
	ModuleTextures(bool start_enabled = true);
	virtual ~ModuleTextures();

	bool Init(JSON_Object* node);
	//bool Start();
	//update_status PreUpdate(float dt);
	//update_status Update(float dt);
	//update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool CleanUp();

	GLuint LoadTexture(const char* filename);
	GLuint LoadSkyboxTexture(const char* filename);
};

#endif