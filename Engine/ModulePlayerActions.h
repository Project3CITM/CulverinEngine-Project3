
#ifndef _MODULEPLAYERACTION_
#define _MODULEPLAYERACTION_
#include "Module.h"
#include "Globals.h"


class InputManager;
class ModulePlayerActions : public Module
{
public:
	ModulePlayerActions(bool start_enabled = true);
	~ModulePlayerActions();


	bool SaveConfig(JSON_Object* node);

private:

	std::vector<InputManager*> iteractive_vector;

};
#endif //_MODULEPLAYERACTION_
