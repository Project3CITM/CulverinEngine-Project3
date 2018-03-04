#include "ModuleKeyBinding.h"


ModuleKeyBinding::ModuleKeyBinding()
{
}


ModuleKeyBinding::~ModuleKeyBinding()
{
}

bool ModuleKeyBinding::Init(JSON_Object * node)
{
	
	

	// NUMBERS--------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_0, "0", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_1, "1", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_2, "2", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_3, "3", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_4, "4", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_5, "5", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_6, "6", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_7, "7", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_8, "8", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_9, "9", KeyBindingType::KEYBOARD_DEVICE));

	// LETTERS-------------------------------------------------------
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_A, "A", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_B, "B", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_C, "C", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_D, "D", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_E, "E", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_F, "F", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_G, "G", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_H, "H", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_I, "I", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_J, "J", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_K, "K", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_L, "L", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_M, "M", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_N, "N", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_O, "O", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_P, "P", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_Q, "Q", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_R, "R", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_S, "S", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_T, "T", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_U, "U", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_V, "V", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_W, "W", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_X, "X", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_Y, "Y", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation((int)SDL_Scancode::SDL_SCANCODE_Z, "Z", KeyBindingType::KEYBOARD_DEVICE));

	// UTILITIES KEYS--------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_RETURN, "Return", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_ESCAPE, "Escape", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_BACKSPACE, "Backspace", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_TAB, "Tab", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_SPACE, "Space", KeyBindingType::KEYBOARD_DEVICE));

	// F-KEYS-------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F1, "F1", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F2, "F2", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F3, "F3", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F4, "F4", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F5, "F5", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F6, "F6", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F7, "F7", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F8, "F8", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F9, "F9", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F10, "F10", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F11, "F11", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_F12, "F12", KeyBindingType::KEYBOARD_DEVICE));

	// DIRECTIONAL KEYS-------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_RIGHT, "Right", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_LEFT, "Left", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_DOWN, "Down", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_UP, "Up", KeyBindingType::KEYBOARD_DEVICE));

	// SHIFT--------------------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_LSHIFT, "Lshift", KeyBindingType::KEYBOARD_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_Scancode::SDL_SCANCODE_RSHIFT, "Rshift", KeyBindingType::KEYBOARD_DEVICE));
	
	// MOUSE INPUT --------------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_EventType::SDL_MOUSEMOTION, "MouseAxis", KeyBindingType::MOUSE_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(1, "ButtonLeft", KeyBindingType::MOUSE_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(2, "ButtonWheel", KeyBindingType::MOUSE_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(3, "ButtonRight", KeyBindingType::MOUSE_BUTTON_DEVICE));

	// CONTROLLER INPUT------------------------------------------------------------
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A, "CButtonA", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B, "CButtonB", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X, "CButtonX", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y, "CButtonY", KeyBindingType::CONTROLLER_BUTTON_DEVICE));

	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSTICK, "CButtonLeftStick", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER, "CButtonLeftShoulder", KeyBindingType::CONTROLLER_BUTTON_DEVICE));

	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK, "CButtonRightStick", KeyBindingType::CONTROLLER_BUTTON_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, "CButtonRightShoulder", KeyBindingType::CONTROLLER_BUTTON_DEVICE));

	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX, "CRightX", KeyBindingType::CONTROLLER_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY, "CRightY", KeyBindingType::CONTROLLER_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX, "CLeftX", KeyBindingType::CONTROLLER_AXIS_DEVICE));
	key_binding_relations.push_back(KeyRelation(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY, "CLeftY", KeyBindingType::CONTROLLER_AXIS_DEVICE));



	return true;
}

bool ModuleKeyBinding::Start()
{
	return true;
}

update_status ModuleKeyBinding::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleKeyBinding::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleKeyBinding::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleKeyBinding::UpdateConfig(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleKeyBinding::SaveConfig(JSON_Object * node)
{
	return true;
}

bool ModuleKeyBinding::CleanUp()
{
	return true;
}

KeyRelation* ModuleKeyBinding::Find_key_binding(const char* name)
{
	for (int i = 0; i < key_binding_relations.size(); i++) 
	{

		if (key_binding_relations[i].name == name) 
		{
			return &key_binding_relations[i];
		}

	}

	KeyRelation temp= KeyRelation(-1,"None", KeyBindingType::NULL_DEVICE);

	return &temp;
}
