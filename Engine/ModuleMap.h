#ifndef __MODULEMAP__
#define __MODULEMAP__

#include "Globals.h"
#include "Module.h"

#define MAX_ARRAY 100

class ModuleMap : public Module
{
public:
	ModuleMap();
	virtual ~ModuleMap();

	bool Init(JSON_Object* node);

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void ShowEditorMap(bool & active);

public:
	//Map ----------
	std::string map_string;
	int map[MAX_ARRAY][MAX_ARRAY];

};

#endif // __MODULEMAP__