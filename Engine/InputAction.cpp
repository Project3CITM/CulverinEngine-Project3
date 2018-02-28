#include "InputAction.h"



InputAction::InputAction()
{
}

InputAction::InputAction(const char * name, ActionInputType action_type): name(name), action_type(action_type)
{
}

InputAction::~InputAction()
{
}
