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
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_0, "0"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_1, "1"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_2, "2"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_3, "3"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_4, "4"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_5, "5"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_6, "6"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_7, "7"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_8, "8"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_9, "9"));

	// LETTERS--------------------------------------------------------
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_A, "A"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_B, "B"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_C, "C"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_D, "D"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_E, "E"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F, "F"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_G, "G"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_H, "H"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_I, "I"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_J, "J"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_K, "K"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_L, "L"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_M, "M"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_N, "N"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_O, "O"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_P, "P"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_Q, "Q"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_R, "R"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_S, "S"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_T, "T"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_U, "U"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_V, "V"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_W, "W"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_X, "X"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_Y, "Y"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_Z, "Z"));

	// UTILITIES KEYS--------------------------------------------------------
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_RETURN, "Return"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_ESCAPE, "Escape"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_BACKSPACE, "Backspace"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_TAB, "Tab"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_SPACE, "Space"));

	// F-KEYS--------------------------------------------------------
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F1, "F1"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F2, "F2"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F3, "F3"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F4, "F4"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F5, "F5"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F6, "F6"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F7, "F7"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F8, "F8"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F9, "F9"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F10, "F10"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F11, "F11"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_F12, "F12"));

	// DIRECTIONAL KEYS--------------------------------------------------------
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_RIGHT, "Right"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_LEFT, "Left"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_DOWN, "Down"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_UP, "Up"));

	// SHIFT--------------------------------------------------------------------
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_LSHIFT, "Lshift"));
	key_binding_relations.push_back(Key_Relation(SDL_Scancode::SDL_SCANCODE_RSHIFT, "Rshift"));
	
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

Key_Relation ModuleKeyBinding::Find_key_binding(const char* name)
{
	for (int i = 0; i < key_binding_relations.size(); i++) 
	{

		if (key_binding_relations[i].name == name) 
		{
			return key_binding_relations[i];
		}

	}

	Key_Relation temp= Key_Relation(SDL_Scancode::SDL_SCANCODE_UNKNOWN,"None");

	return temp;
}
