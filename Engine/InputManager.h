#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
#include "Globals.h"
#include <vector>
#include"InputAction.h"

typedef union SDL_Event SDL_Event;


class InputManager
{
public:
	InputManager(PlayerActions* my_player_action);
	~InputManager();
	void UpdateInputActions();
	bool ProcessEvent(SDL_Event* input_event);
	void Clear();
	ControllerAxisAction* GetAxis(const char* name);
	std::vector<ControllerAxisAction*> GetAxisVector(const char* name);

	//ButtonAction* GetButton(const char* name);
	std::vector<KeyAction*> GetKey(const char* name);
	MouseButtonAction* GetMouseButton(const char* name);

	void ShowInspectorInfo();

	void ActiveInput();
	void DeactiveInput();
	void ActiveBlockAction();
	void DeactiveBlockActive();
	void ActiveWindowOpen();
	void DeactiveWIndowOpen();
	bool ActionExist(const char* name);
	void SetActiveInput(bool set);
	void SetBlockAction(bool set);
	void SetName(const char* name);

	const char* GetName()const;
	const bool GetBlockAction()const;
	bool GetBlockAction();
	const bool GetActiveInput()const;
	bool GetActiveInput();
	bool GetWindowOpen()const;
	InputAction* CreateNewAction(const char* new_name, const char* new_key_positive, const char* new_key_negative, ActionInputType new_type = ActionInputType::UNKNOWN_ACTION);
	std::vector<InputAction*> GetActionVector() const;
	std::vector<InputAction*> GetActionVector();

private:
public:
	int number_of_action = 0;
	std::vector<InputAction*> action_vector;
	std::list<InputAction*> active_action;
	bool active_input = true;
	bool block_action = false;
private:
	PlayerActions* my_player_action = nullptr;
	bool window_open = false;
	std::string name;
	char selected_action_name[50] = "default";
	char selected_action_key_positive[50] = "default";
	char selected_action_key_negative[50] = "default";

	int action_type = 0;
};
#endif //INPUT_MANAGER_H
