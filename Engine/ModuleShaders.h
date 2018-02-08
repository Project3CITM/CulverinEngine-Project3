#ifndef MODULE_SHADERS_H_
#define MODULE_SHADERS_H_
#include"Module.h"

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

private:

};


#endif
