#include "InputAction.h"



InputAction::InputAction():key_relation(new KeyRelation(-1,"default", KeyBindingType::NULL_DEVICE))
{
}

InputAction::InputAction(const char * name, KeyRelation *key_relation, ActionInputType action_type): name(name), action_type(action_type), key_relation(key_relation)
{
}

InputAction::~InputAction()
{
}
