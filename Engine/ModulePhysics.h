#ifndef MODULEPHYSICS_H_
#define MODULEPHYSICS_H_

#include "Module.h"


class ModulePhysics : public Module
{
public:
	ModulePhysics(bool start_enabled = true);
	~ModulePhysics();

	bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	//update_status Update(float dt);
	update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool SaveConfig(JSON_Object* node);
	bool CleanUp();

private:

};

#endif