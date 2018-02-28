#ifndef INPUT_ACTION_H_
#define INPUT_ACTION_H_
#include<string>

class Key_Relation;

enum ActionInputType
{
	UNKNOWN_ACTION=-1,
	AXIS_ACTION,
	KEY_ACTION,
	CONTROLLER_ACTION
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
	

};


class AxisAction
{
public:




};









#endif

