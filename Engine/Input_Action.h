#ifndef INPUT_ACTION_H_
#define INPUT_ACTION_H_
#include<string>

class Key_Relation;

enum Action_input_type
{
	UNKNOWN_ACTION=-1,
	AXIS_ACTION,
	KEY_ACTION,
	CONTROLLER_ACTION
};


class Input_Action
{
public:

	Input_Action();
	Input_Action(const char* name, Action_input_type action_type = Action_input_type::UNKNOWN_ACTION);
	~Input_Action();

public:

	std::string name = "";
	Action_input_type action_type = Action_input_type::UNKNOWN_ACTION;
	

};


class AxisAction
{
public:




};









#endif

