
#ifndef MODULE_PLAYER_ACTION_H
#define MODULE_PLAYER_ACTION_H
#include "Globals.h"
#include <vector>
#include <list>
#include "KeyBinding.h"

typedef union SDL_Event SDL_Event;
class ModuleInput;
class InputManager;
class InputAction;
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

	bool SetInputActionToChange(const char* input_action, const char* input_manager, const char* device, bool change_negative = false);

	bool GetInputManagerActive(const char* name)const;
	bool GetInputManagerBlock(const char* name)const ;

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

	const char* GetInput_ControllerActionName(const char* name, const char* input, const char* device, bool negative_key);
	const char*	GetInput_ControllerKeyBindingName(const char* name, const char* input, const char* device, bool negative_key);

	void SendNewDeviceCombinationType(DeviceCombinationType type);
private:
	DeviceCombinationType SelectDeviceCombination(const char* value);

public:
	int number_of_inputs = 0;
	std::vector<InputManager*> interactive_vector;

private:
	struct KeyChange
	{
		enum KeyState
		{
			NO_STATE = -1,
			WAIT_FOR_KEY_STATE,
			VALID_KEY_STATE,
			INVALID_KEY_STATE
		};
		KeyState ReceiveEvent(SDL_Event* input_event);
		void SetInputAction(InputAction* key_to_change);
		void Clear();

		InputAction* key_to_change = nullptr;
		bool change_negative = false;
		bool change_active = true;
		KeyState state = KeyState::NO_STATE;
	};
	KeyChange key_change;
	std::string selected_input_name;
	ModuleInput* my_module = nullptr;
	DeviceCombinationType actual_player_action = DeviceCombinationType::KEYBOARD_AND_MOUSE_COMB_DEVICE;
};
#endif //MODULE_PLAYER_ACTION_H
