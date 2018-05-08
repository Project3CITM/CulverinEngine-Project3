#include "KeyBinding.h"
#include "Application.h"
#include "InputManager.h"
#define KEY_NONE_NUM 0
KeyBinding::KeyBinding()
{
}


KeyBinding::~KeyBinding()
{
}

bool KeyBinding::InitKeyBinding()
{
	// NONE-----------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(-1, "None", "None", KeyBindingType::NULL_DEVICE));

	// NUMBERS--------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_0, "0", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_1, "1", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_2, "2", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_3, "3", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_4, "4", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_5, "5", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_6, "6", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_7, "7", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_8, "8", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_9, "9", "keyboard", KeyBindingType::KEYBOARD_DEVICE));

	// LETTERS-------------------------------------------------------
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_A, "A", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_B, "B", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_C, "C","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_D, "D","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_E, "E","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_F, "F","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_G, "G","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_H, "H","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_I, "I","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_J, "J","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_K, "K","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_L, "L","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_M, "M","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_N, "N","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_O, "O","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_P, "P","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_Q, "Q","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_R, "R","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_S, "S","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_T, "T","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_U, "U","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_V, "V","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_W, "W","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_X, "X","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_Y, "Y","keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_Z, "Z","keyboard", KeyBindingType::KEYBOARD_DEVICE));

	// UTILITIES KEYS--------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_RETURN, "Return", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_ESCAPE, "Escape", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_BACKSPACE, "Backspace", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_TAB, "Tab", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_SPACE, "Space", "keyboard", KeyBindingType::KEYBOARD_DEVICE));

	// F-KEYS-------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F1, "F1", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F2, "F2", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F3, "F3", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F4, "F4", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F5, "F5", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F6, "F6", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F7, "F7", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F8, "F8", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F9, "F9", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F10, "F10", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F11, "F11", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F12, "F12", "keyboard", KeyBindingType::KEYBOARD_DEVICE));

	// DIRECTIONAL KEYS-------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_RIGHT, "Right", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_LEFT, "Left", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_DOWN, "Down", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_UP, "Up", "keyboard", KeyBindingType::KEYBOARD_DEVICE));

	// SHIFT--------------------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_LSHIFT, "Lshift", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_RSHIFT, "Rshift", "keyboard", KeyBindingType::KEYBOARD_DEVICE));
	
	// MOUSE INPUT --------------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_EventType::SDL_MOUSEMOTION, "MouseAxis", "mouse", KeyBindingType::MOUSE_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(1, "ButtonLeft", "mouse", KeyBindingType::MOUSE_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(2, "ButtonWheel", "mouse", KeyBindingType::MOUSE_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(3, "ButtonRight","mouse", KeyBindingType::MOUSE_BUTTON_DEVICE));

	// CONTROLLER INPUT------------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A, " A", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B, " B", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X, " X", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y, " Y", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));

	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START, "Start", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_GUIDE, "CGuide", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_BACK, "CBack", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));

	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP, "CPadUp", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN, "CPadDown", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT, "CPadLeft", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT, "CPadRight", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));

	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT, "L2", "controller", KeyBindingType::CONTROLLER_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER, "L1", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));

	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT, "R2", "controller", KeyBindingType::CONTROLLER_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, "R1", "controller", KeyBindingType::CONTROLLER_BUTTON_DEVICE));

	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX, "CRightX", "controller", KeyBindingType::CONTROLLER_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY, "CRightY", "controller", KeyBindingType::CONTROLLER_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX, "CLeftX", "controller", KeyBindingType::CONTROLLER_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY, "CLeftY", "controller", KeyBindingType::CONTROLLER_AXIS_DEVICE));


	return true;
}



bool KeyBinding::CleanUp()
{
	key_binding_relations.clear();
	return true;
}

KeyRelation * KeyBinding::FindKeyBinding(const char * name)
{
	for (int i = 0; i < key_binding_relations.size(); i++)
	{

		if (strcmp(key_binding_relations[i].name.c_str(),name)==0)
		{
			return &key_binding_relations[i];
		}

	}


	return &key_binding_relations[KEY_NONE_NUM];
}

KeyRelation * KeyBinding::FindKeyBinding(DeviceCombinationType device, int event_value)
{
	for (int i = 0; i < key_binding_relations.size(); i++)
	{

		if (key_binding_relations[i].device == device&&key_binding_relations[i].event_value == event_value)
		{
			return &key_binding_relations[i];
		}

	}


	return &key_binding_relations[KEY_NONE_NUM];
}

KeyRelation * KeyBinding::GetNullKeyBinding()
{
	return  &key_binding_relations[KEY_NONE_NUM];
}

DeviceCombinationType KeyRelation::SelectDeviceCombination(const char* value)
{
	if (strcmp(value, "mouse")==0|| strcmp(value, "keyboard") == 0)
	{
		return DeviceCombinationType::KEYBOARD_AND_MOUSE_COMB_DEVICE;
	}
	else if(strcmp(value, "controller") == 0)
	{
		return DeviceCombinationType::CONTROLLER_COMB_DEVICE;
	}
	return DeviceCombinationType::NULL_COMB_DEVICE;
}
