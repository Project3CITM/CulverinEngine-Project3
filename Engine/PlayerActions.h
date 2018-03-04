
#ifndef MODULE_PLAYER_ACTION_H
#define MODULE_PLAYER_ACTION_H
#include "Globals.h"
#include <vector>
typedef union SDL_Event SDL_Event;
class ModuleInput;
class InputManager;
class PlayerActions
{
public:
	PlayerActions(ModuleInput* my_module);
	~PlayerActions();
	void UpdateConfig(float dt);
	void Clear();


	void UpdateInputsManager();

	bool ReceiveEvent(SDL_Event* input_event);
	InputManager* GetInputManager(const char* name) const;
	InputManager* GetInputManager(const char* name);
	std::vector<InputManager*> GetInteractiveVector() const;
	std::vector<InputManager*> GetInteractiveVector();
public:
	int number_of_inputs = 0;
	std::vector<InputManager*> interactive_vector;

private:
	std::string selected_input_name;
	ModuleInput* my_module = nullptr;

};
#endif //MODULE_PLAYER_ACTION_H
