
#ifndef MODULE_PLAYER_ACTION_H
#define MODULE_PLAYER_ACTION_H
#include "Globals.h"
#include <vector>
#include <list>
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
	void InputManagerActive(const char* name);
	void InputManagerDeactive(const char* name);

	void InputManagerBlock(const char* name);
	void InputManagerUnblock(const char* name);
	bool InputManagerExist(const char* name);

	void SetInputManagerActive(const char* name,bool set);
	void SetInputManagerBlock(const char* name, bool set);

	InputManager* GetInputManager(const char* name) const;
	InputManager* GetInputManager(const char* name);
	std::vector<InputManager*> GetInteractiveVector() const;
	std::vector<InputManager*> GetInteractiveVector();

	bool GetInput_KeyDown(const char* key, const char* input);
	bool GetInput_KeyUp(const char* key, const char* input);
	bool GetInput_KeyRepeat(const char* key, const char* input);
	bool GetInput_MouseButtonDown(const char* name, const char* input);
	bool GetInput_MouseButtonUp(const char* name, const char* input);
	float GetInput_ControllerAxis(const char* name, const char* input);



public:
	int number_of_inputs = 0;
	std::vector<InputManager*> interactive_vector;
	std::list<InputManager*> update_interactive;

private:
	std::string selected_input_name;
	ModuleInput* my_module = nullptr;

};
#endif //MODULE_PLAYER_ACTION_H
