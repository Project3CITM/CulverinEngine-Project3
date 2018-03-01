#ifndef INPUT_ACTION_H_
#define INPUT_ACTION_H_
#include<string>
#include"Math\float2.h"
#include"ModuleInput.h"
class Key_Relation;
//enum KEY_STATE;

enum ActionInputType
{
	UNKNOWN_ACTION=-1,
	AXIS_ACTION,
	KEY_ACTION,
	CONTROLLER_ACTION
};

enum AxisDirectionController
{
	NON_DIRECTION_C=-1,
	UP_DIRECTION_C,
	DOWN_DIRECTION_C,
	RIGHT_DIRECTION_C,
	LEFT_DIRECTION_C
};

class InputAction
{
public:

	InputAction();
	InputAction(const char* name, ActionInputType action_type = ActionInputType::UNKNOWN_ACTION);
	~InputAction();

public:

	std::string name = "";
	ActionInputType action_type = ActionInputType::UNKNOWN_ACTION;
	//key_relation

};


class AxisAction: public InputAction
{
public:

	float2 position;
	float2 motion;

};

class ControllerAxisAction : public InputAction
{
public:

	float2 motion;
	AxisDirectionController direction = AxisDirectionController::NON_DIRECTION_C;

};

class MouseButtonAction : public InputAction
{
public:

	KEY_STATE state = KEY_STATE::KEY_IDLE;
	int number_clicks = 0;
};

class KeyAction : public InputAction
{
public:

	KEY_STATE state = KEY_STATE::KEY_IDLE;
	bool OnClick() { return state == KEY_STATE::KEY_DOWN; };
	bool OnRelease() { return state == KEY_STATE::KEY_UP; };
	bool OnRepeat() { return state == KEY_STATE::KEY_REPEAT; };

};

class ButtonAction : public InputAction
{
public:

	KEY_STATE state = KEY_STATE::KEY_IDLE;
	bool OnClick() { return state == KEY_STATE::KEY_DOWN; };
	bool OnRelease() { return state == KEY_STATE::KEY_UP; };
	bool OnRepeat() { return state == KEY_STATE::KEY_REPEAT; };

};





#endif

