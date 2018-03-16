#include "InputAction.h"



InputAction::InputAction():positive_button(new KeyRelation(-1,"default", KeyBindingType::NULL_DEVICE)), negative_button(new KeyRelation(-1, "default", KeyBindingType::NULL_DEVICE))
{
}

InputAction::InputAction(const char * name, KeyRelation *key_relation, ActionInputType action_type): name(name), action_type(action_type), positive_button(key_relation)
{
}

InputAction::~InputAction()
{
}

bool MouseButtonAction::ProcessEventAction(SDL_Event * input_event)
{

	if (positive_button->key_type == KeyBindingType::MOUSE_BUTTON_DEVICE)
	{
		if (input_event->type == SDL_MOUSEBUTTONDOWN)
		{
			if (positive_button->event_value == input_event->button.button) {
				state = Keystateaction::KEY_DOWN_ACTION;
				number_clicks = input_event->button.clicks;
			}
			return true;
		}

	}

	//Negative button mouse?

	return false;
}


bool MouseButtonAction::UpdateEventAction(int mouse_x, int mouse_y, Uint32 buttons)
{

	if (buttons & SDL_BUTTON(positive_button->event_value))
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

	// negative

	return true;

}

bool AxisAction::ProcessEventAction(SDL_Event * input_event)
{
	if (positive_button->key_type == KeyBindingType::MOUSE_AXIS_DEVICE)
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

bool ControllerAxisAction::ProcessEventAction(SDL_Event * input_event)
{
	if (positive_button->key_type == KeyBindingType::CONTROLLER_AXIS_DEVICE)
	{
		if (input_event->type == SDL_CONTROLLERAXISMOTION)
		{
			if (input_event->caxis.axis == positive_button->event_value) {
				//direction_axis = input_event->caxis.value;
				direction_axis = (float)input_event->caxis.value / (float)maximum_axis_c;
				//LOG("joystick %f", direction_axis);

				return true;
			}
			else if (input_event->cbutton.button == positive_button->event_value) {
				//direction_axis = input_event->caxis.value;
				direction_axis = (float)input_event->caxis.value / (float)maximum_axis_c;
				//LOG("joystick %f", direction_axis);

				return true;
			}

		}
	}
	return false;
}

bool ControllerAxisAction::UpdateEventAction()
{

	/*
	Maybe we don't need this funtion.
	*/

	return true;

}

bool KeyAction::ProcessEventAction(SDL_Event * input_event)
{
	bool ret = false;

	ret = DetectAction(*input_event);

	return ret;
}

bool KeyAction::DetectAction(const SDL_Event &input_event) {

	switch (positive_button->key_type)
	{
	case KeyBindingType::KEYBOARD_DEVICE:

		if (input_event.type == SDL_KEYDOWN)
		{
			if (positive_button->event_value == input_event.key.keysym.scancode)
			{
				state = Keystateaction::KEY_DOWN_ACTION;
				this->direction_axis = 1.0f;
				return true;
			}
			else if (negative_button->event_value == input_event.key.keysym.scancode)
			{
				this->direction_axis = -1.0f;
			}

		}
		return false;
		break;

	case KeyBindingType::CONTROLLER_BUTTON_DEVICE:


		if (input_event.type == SDL_CONTROLLERBUTTONDOWN)
		{
			if (positive_button->event_value == input_event.cbutton.button)
			{
				state = Keystateaction::KEY_DOWN_ACTION;
				return true;
			}

		}
		return false;
		break;
	}

	return false;
}

bool KeyAction::UpdateEventAction(const Uint8 * array_kys)
{

	int state_event = -1;
	/*if (state == Keystateaction::KEY_IDLE_ACTION)
	{
		return true;
	}*/

	switch (positive_button->key_type)
	{
	case KeyBindingType::KEYBOARD_DEVICE:
		state_event = array_kys[positive_button->event_value];
		this->direction_axis = 0.0f;
		break;

	case KeyBindingType::CONTROLLER_BUTTON_DEVICE:
		state_event = SDL_GameControllerGetButton(App->input->controller, (SDL_GameControllerButton)positive_button->event_value);
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
