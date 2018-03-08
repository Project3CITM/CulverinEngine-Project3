#ifndef INPUT_ACTION_H_
#define INPUT_ACTION_H_
#include<string>
#include"Math\float2.h"
#include"ModuleKeyBinding.h"
#include"Application.h"
#include"ModuleInput.h"



enum Keystateaction
{
	KEY_IDLE_ACTION = 0,
	KEY_DOWN_ACTION,
	KEY_REPEAT_ACTION,
	KEY_UP_ACTION
};

enum ActionInputType
{
	UNKNOWN_ACTION = -1,
	AXIS_ACTION,
	CONTROLLER_AXIS_ACTION,
	KEY_ACTION,
	MOUSE_BUTTON_ACTION,
	BUTTON_ACTION
};

/*enum KeyButtonType
{
	UNKNOWN_TYPE = -1,
	KEY_TYPE,
	BUTTON_TYPE
};*/

enum AxisDirectionController
{
	NON_DIRECTION_C = -1,
	UP_DIRECTION_C,
	DOWN_DIRECTION_C,
	RIGHT_DIRECTION_C,
	LEFT_DIRECTION_C
};

class InputAction
{
public:

	InputAction();
	InputAction(const char* name, KeyRelation* key_relation, ActionInputType action_type = ActionInputType::UNKNOWN_ACTION);
	~InputAction();

	virtual bool ProcessEventAction(SDL_Event * input_event) { return false; }
	//virtual bool UpdateEventAction(const Uint8* array_kys) { return false; }
public:

	std::string name = "";
	ActionInputType action_type = ActionInputType::UNKNOWN_ACTION;
	KeyRelation* key_relation;

};


class AxisAction : public InputAction
{
public:
	AxisAction() { action_type = ActionInputType::AXIS_ACTION; }
	bool ProcessEventAction(SDL_Event * input_event)
	{
		if (key_relation->key_type == KeyBindingType::MOUSE_AXIS_DEVICE)
		{
			if (input_event->type == SDL_MOUSEMOTION)
			{
				position.x = input_event->motion.x / SCREEN_SIZE;
				position.y = input_event->motion.y / SCREEN_SIZE;

				motion.x = input_event->motion.x;
				motion.y = input_event->motion.y;
				return true;
			}
		}
		return false;
	}
	float2 position;
	float2 motion;

};

class ControllerAxisAction : public InputAction
{
public:
	ControllerAxisAction() { action_type = ActionInputType::CONTROLLER_AXIS_ACTION; }
	bool ProcessEventAction(SDL_Event * input_event)
	{
		if (key_relation->key_type == KeyBindingType::CONTROLLER_AXIS_DEVICE)
		{
			if (input_event->type == SDL_CONTROLLERAXISMOTION)
			{
				if (input_event->caxis.axis == key_relation->event_value) {
					//direction_axis = input_event->caxis.value;
					direction_axis = (float)input_event->caxis.value/ (float)maximum_axis_c;
					//LOG("joystick %f", direction_axis);

					return true;
				}
				else if (input_event->cbutton.button == key_relation->event_value) {
					//direction_axis = input_event->caxis.value;
					direction_axis = (float)input_event->caxis.value / (float)maximum_axis_c;
					//LOG("joystick %f", direction_axis);

					return true;
				}
			}
		}
		return false;
	}

	bool UpdateEventAction()
	{

		/*
		Maybe we don't need this funtion.
		*/

		return true;

	}

	AxisDirectionController direction = AxisDirectionController::NON_DIRECTION_C;
	float direction_axis = 0;

private:
	float dead_zone = 8000;
	float maximum_axis_c = 32768;

};

class MouseButtonAction : public InputAction
{
public:
	MouseButtonAction() { action_type = ActionInputType::MOUSE_BUTTON_ACTION; }
	bool OnClick() { return state == Keystateaction::KEY_DOWN_ACTION; };
	bool OnRelease() { return state == Keystateaction::KEY_UP_ACTION; };
	bool OnRepeat() { return state == Keystateaction::KEY_REPEAT_ACTION; };
	bool ProcessEventAction(SDL_Event * input_event)
	{
		if (key_relation->key_type == KeyBindingType::MOUSE_BUTTON_DEVICE)
		{
			if (input_event->type == SDL_MOUSEBUTTONDOWN)
			{
				if (key_relation->event_value == input_event->button.button) {
					state = Keystateaction::KEY_DOWN_ACTION;
					number_clicks=input_event->button.clicks;
				}
				return true;
			}

		}
		return false;
	}

	bool UpdateEventAction(int mouse_x, int mouse_y, Uint32 buttons)
	{ 
		
		if (buttons & SDL_BUTTON(key_relation->event_value))
		{
			if (state == Keystateaction::KEY_IDLE_ACTION)
				state = Keystateaction::KEY_DOWN_ACTION;
			else
				state = Keystateaction::KEY_REPEAT_ACTION;
		}
		else
		{
			if (state == Keystateaction::KEY_REPEAT_ACTION || state == Keystateaction::KEY_DOWN_ACTION)
				state = Keystateaction::KEY_UP_ACTION;
			else
				state = Keystateaction::KEY_IDLE_ACTION;
		}

		return true; 
	
	}

	Keystateaction state = Keystateaction::KEY_IDLE_ACTION;
	int number_clicks = 0;
};

class KeyAction : public InputAction
{
public:
	KeyAction() 
	{
		/*if (key_relation->key_type == KeyBindingType::KEYBOARD_DEVICE) {
			action_type = ActionInputType::KEY_ACTION;
		}
		else if (key_relation->key_type == KeyBindingType::CONTROLLER_BUTTON_DEVICE) {
			action_type = ActionInputType::BUTTON_ACTION;
			LOG("CONTROLLER");
		}*/
	}

	KeyAction(ActionInputType type)
	{
		action_type = type;
	}

	bool OnClick() { return state == Keystateaction::KEY_DOWN_ACTION; };
	bool OnRelease() { return state == Keystateaction::KEY_UP_ACTION; };
	bool OnRepeat() { return state == Keystateaction::KEY_REPEAT_ACTION; };

	bool ProcessEventAction(SDL_Event * input_event)
	{
		switch (key_relation->key_type)
		{
		case KeyBindingType::KEYBOARD_DEVICE:

			if (input_event->type == SDL_KEYDOWN)
			{
				if (key_relation->event_value == input_event->key.keysym.scancode)
				{
						state = Keystateaction::KEY_DOWN_ACTION;
						return true;
				}
			}
			return false;
		break;

		case KeyBindingType::CONTROLLER_BUTTON_DEVICE:
		

				if (input_event->type == SDL_CONTROLLERBUTTONDOWN)
				{
					if (key_relation->event_value == input_event->cbutton.button)
					{
						state = Keystateaction::KEY_DOWN_ACTION;
						return true;
					}
					
				}
			return false;
			break;
		}
	}


	bool UpdateEventAction(const Uint8* array_kys) 
	{ 

		int state_event = -1;
		if (state == Keystateaction::KEY_IDLE_ACTION)
		{
			return true;
		}

		switch (key_relation->key_type)
		{
		case KeyBindingType::KEYBOARD_DEVICE:
			state_event = array_kys[key_relation->event_value];			
			break;

		case KeyBindingType::CONTROLLER_BUTTON_DEVICE:
			state_event = SDL_GameControllerGetButton(App->input->controller, (SDL_GameControllerButton)key_relation->event_value);
			break;
		}

		if (state_event == 1)
		{
			if (state == Keystateaction::KEY_IDLE_ACTION) {
				state = Keystateaction::KEY_DOWN_ACTION;
			}
			else
				state = Keystateaction::KEY_REPEAT_ACTION;
		}
		else
		{
			if (state == Keystateaction::KEY_REPEAT_ACTION || state == Keystateaction::KEY_DOWN_ACTION) {
				state = Keystateaction::KEY_UP_ACTION;
			}
			else
				state = Keystateaction::KEY_IDLE_ACTION;
		}

		return true;
	}

	Keystateaction state = Keystateaction::KEY_IDLE_ACTION;
};

/*
class ButtonAction : public InputAction
{
public:
	ButtonAction() { action_type = ActionInputType::BUTTON_ACTION; }

	bool OnClick() { return state == Keystateaction::KEY_DOWN_ACTION; };
	bool OnRelease() { return state == Keystateaction::KEY_UP_ACTION; };
	bool OnRepeat() { return state == Keystateaction::KEY_REPEAT_ACTION; };

	bool ProcessEventAction(SDL_Event * input_event)
	{
		if (key_relation->key_type == KeyBindingType::CONTROLLER_BUTTON_DEVICE)
		{
			if (input_event->type == SDL_CONTROLLERBUTTONDOWN)
			{
				if (key_relation->event_value == input_event->cbutton.button)
				{
					state = Keystateaction::KEY_DOWN_ACTION;
				}
				return true;
			}

		}
		return false;
	}

	bool UpdateEventAction()
	{
		
		int state_event = SDL_GameControllerGetButton(App->input->controller, (SDL_GameControllerButton)key_relation->event_value);

		if (state_event == 1)
		{
			if (state == Keystateaction::KEY_IDLE_ACTION) {
				state = Keystateaction::KEY_DOWN_ACTION;
			}
			else
				state = Keystateaction::KEY_REPEAT_ACTION;
		}
		else
		{
			if (state == Keystateaction::KEY_REPEAT_ACTION || state == Keystateaction::KEY_DOWN_ACTION) {
				state = Keystateaction::KEY_UP_ACTION;
			}
			else
				state = Keystateaction::KEY_IDLE_ACTION;
		}
		return true;
	}

	Keystateaction state = Keystateaction::KEY_IDLE_ACTION;
};*/

#endif