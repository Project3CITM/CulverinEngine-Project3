
#ifndef MODULE_PLAYER_ACTION_H
#define MODULE_PLAYER_ACTION_H
#include "Module.h"
#include "Globals.h"

typedef union SDL_Event SDL_Event;

class InputManager;
class ModulePlayerActions : public Module
{
public:
	ModulePlayerActions(bool start_enabled = true);
	~ModulePlayerActions();
	update_status Update(float dt);
	update_status UpdateConfig(float dt);



	bool SaveConfig(JSON_Object* node);
	void UpdateInputsManager();

	bool ReceiveEvent(SDL_Event* input_event);
	InputManager* GetInputManager(const char* name) const;
	InputManager* GetInputManager(const char* name);

private:
	int number_of_inputs = 0;
	std::vector<InputManager*> interactive_vector;

};
#endif //MODULE_PLAYER_ACTION_H
