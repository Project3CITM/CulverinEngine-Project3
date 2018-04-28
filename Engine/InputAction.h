#ifndef INPUT_ACTION_H_
#define INPUT_ACTION_H_
#include<string>
#include"Math\float2.h"
#include"KeyBinding.h"
#include"Application.h"
#include"ModuleInput.h"

enum Keystateaction
{
	KEY_IDLE_ACTION = 0,
	KEY_DOWN_ACTION,
	KEY_REPEAT_ACTION,
	KEY_UP_ACTION
};

enum ActionInputType
{
	UNKNOWN_ACTION = -1,
	AXIS_ACTION,
	CONTROLLER_AXIS_ACTION,
	KEY_ACTION,
	MOUSE_BUTTON_ACTION,
	BUTTON_ACTION
};

enum AxisDirectionController
{
	NON_DIRECTION_C = -1,
	UP_DIRECTION_C,
	DOWN_DIRECTION_C,
	RIGHT_DIRECTION_C,
	LEFT_DIRECTION_C
};

class InputAction
{
public:

	InputAction();
	InputAction(const char* name, KeyRelation* key_relation, ActionInputType action_type = ActionInputType::UNKNOWN_ACTION, InputManager* my_manager=nullptr);
	~InputAction();

	virtual bool ProcessEventAction(SDL_Event * input_event) { return false; }
	virtual bool PositiveReaction(SDL_Event * input_event) { return false; }

	//virtual bool UpdateEventAction(const Uint8* array_kys) { return false; }

private:
	
public:

	std::string name = "default";
	ActionInputType action_type = ActionInputType::UNKNOWN_ACTION;
	DeviceCombinationType key_device = DeviceCombinationType::NULL_COMB_DEVICE;
	KeyRelation* positive_button = nullptr;
	KeyRelation* negative_button = nullptr;
	InputManager* my_manager = nullptr;
	//KeyRelation* alt_positive_button;
	//KeyRelation* alt_negative_button;

};

class MouseButtonAction : public InputAction
{
public:
	MouseButtonAction() { action_type = ActionInputType::MOUSE_BUTTON_ACTION; }
	bool OnClick() { return state == Keystateaction::KEY_DOWN_ACTION; };
	bool OnRelease() { return state == Keystateaction::KEY_UP_ACTION; };
	bool OnRepeat() { return state == Keystateaction::KEY_REPEAT_ACTION; };
	bool ProcessEventAction(SDL_Event * input_event);
	bool PositiveReaction(SDL_Event * input_event);
	bool UpdateEventAction(int mouse_x, int mouse_y, Uint32 buttons);

public:
	Keystateaction state = Keystateaction::KEY_IDLE_ACTION;
	int number_clicks = 0;
};

class AxisAction : public InputAction
{
public:
	AxisAction() { action_type = ActionInputType::AXIS_ACTION; }
	bool ProcessEventAction(SDL_Event * input_event);
public:
	float2 position;
	float2 motion;

};

class ControllerAxisAction : public InputAction
{
public:
	ControllerAxisAction() { action_type = ActionInputType::CONTROLLER_AXIS_ACTION; }
	bool ProcessEventAction(SDL_Event * input_event);
	bool PositiveReaction(SDL_Event * input_event);

	bool UpdateEventAction();

public:
	AxisDirectionController direction = AxisDirectionController::NON_DIRECTION_C;
	float direction_axis = 0;

private:
	float dead_zone = 8000;
	float maximum_axis_c = 32768;

};

class KeyAction : public ControllerAxisAction
{
public:
	KeyAction() {}

	KeyAction(ActionInputType type)
	{
		action_type = type;
	}

	bool OnClick() { return state == Keystateaction::KEY_DOWN_ACTION; };
	bool OnRelease() { return state == Keystateaction::KEY_UP_ACTION; };
	bool OnRepeat() { return state == Keystateaction::KEY_REPEAT_ACTION; };
	bool ProcessEventAction(SDL_Event * input_event);
	bool PositiveReaction(SDL_Event * input_event);

	bool DetectAction(const SDL_Event &input_event);
	bool UpdateEventAction(const Uint8* array_kys);
	int UpdateKeyRelation(KeyRelation* to_update, const Uint8 * array_kys);
	bool UpdateStateEvent(int state,float axis_value);

public:
	Keystateaction state = Keystateaction::KEY_IDLE_ACTION;
};


#endif