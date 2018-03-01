#include "InputAction.h"



InputAction::InputAction():key_relation("default")
{
}

InputAction::InputAction(const char * name, KeyRelation key_relation, ActionInputType action_type): name(name), key_relation(key_relation), action_type(action_type)
{
}

InputAction::~InputAction()
{
}
