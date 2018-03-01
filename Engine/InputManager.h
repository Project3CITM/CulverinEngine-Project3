#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
#include "Globals.h"
#include <vector>
#include"InputAction.h"

typedef union SDL_Event SDL_Event;


class InputManager
{
public:
	InputManager();
	~InputManager();

	void LookEvent(SDL_Event* input_event);
	//AxisAction* GetAxis(const char* name);
	//ButtonAction* GetButton(const char* name);
	KeyAction* GetKey(const char* name);
	void ShowInspectorInfo();

	void ActiveInput();
	void DeactiveInput();
	void ActiveBlockAction();
	void DeactiveBlockActive();
	void ActiveWindowOpen();
	void DeactiveWIndowOpen();

	void SetActiveInput(bool set);
	void SetBlockAction(bool set);
	void SetName(const char* name);

	const char* GetName()const;
	bool GetBlockAction()const;
	bool GetActiveInput()const;
	bool GetWindowOpen()const;
	InputAction* CreateNewAction(const char* new_name, const char* new_key_binding, ActionInputType new_type = ActionInputType::UNKNOWN_ACTION);


private:
public:
private:
	std::vector<InputAction*> action_vector;
	int number_of_action=0;
	bool active_input = true;
	bool block_action = false;
	bool window_open = false;
	std::string name;
	std::string selected_action_name;
	std::string selected_action_key;
	int action_type = 0;
};
#endif //INPUT_MANAGER_H
