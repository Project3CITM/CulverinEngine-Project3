#include "InputAction.h"



InputAction::InputAction()
{
	key_relation = new KeyRelation("default");
}

InputAction::InputAction(const char * name, KeyRelation* key_relation, ActionInputType action_type): name(name), action_type(action_type)
{
	this->key_relation = key_relation;
}

InputAction::~InputAction()
{
}
