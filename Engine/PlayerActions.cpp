#include "PlayerActions.h"
#include "InputManager.h"
#include "InputAction.h"
#include "ModuleInput.h"

#define INPUT_MANAGER_LIMIT 5
#define MAX_INPUT 20


PlayerActions::PlayerActions(ModuleInput * my_module):my_module(my_module)
{
}

PlayerActions::~PlayerActions()
{
}




void PlayerActions::UpdateConfig(float dt)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));
	
	ImGui::Text("Number of Inputs");
	if (ImGui::InputInt("##number_of_inputs", &number_of_inputs))
	{
		if (number_of_inputs < 0)
		{
			number_of_inputs = 0;
		}
		else if (number_of_inputs >= INPUT_MANAGER_LIMIT)
		{
			number_of_inputs = INPUT_MANAGER_LIMIT;
		}
		if (interactive_vector.size()<number_of_inputs)
		{
			for (int i = interactive_vector.size(); i < number_of_inputs; i++)
			{
				InputManager* input_manager = new InputManager(this);
				interactive_vector.push_back(input_manager);
				std::string temp_name = "Input " + std::to_string(number_of_inputs);
				input_manager->SetName(temp_name.c_str());
			}
		}
		else
		{
			int size = interactive_vector.size() - number_of_inputs;
			for (int i = 0; i < size; i++)
			{
				InputManager* input_manager = interactive_vector.back();

				interactive_vector.pop_back();
				RELEASE(input_manager);
			}
		}
	}

	ImGui::Text("Inputs:");

	uint size = interactive_vector.size();
	ImGui::Columns(4, "my_input");
	ImGui::SetColumnWidth(1, 50);
	ImGui::SetColumnWidth(2, 50);

	ImGui::Text("Name");
	ImGui::NextColumn();
	ImGui::Text("Active");
	ImGui::NextColumn();
	ImGui::Text("Block");
	ImGui::NextColumn();	
	ImGui::Text("Input name");
	ImGui::NextColumn();
	ImGui::Separator();
	static int selected = 0;

	for (int i = 0; i < size; i++)
	{
		ImGui::PushID(i);
		InputManager* input_manager = interactive_vector[i];
		ImGui::AlignFirstTextHeightToWidgets();
		if (ImGui::Selectable(input_manager->GetName(), selected == i, ImGuiSelectableFlags_AllowDoubleClick))
		{
			selected = i;
			if (ImGui::IsMouseDoubleClicked(0))
				input_manager->ActiveWindowOpen();
		}
		selected_input_name = input_manager->GetName();
		ImGui::NextColumn();
		std::string temp = "##input_active" + std::to_string(i);
		ImGui::Checkbox(temp.c_str() , &input_manager->active_input);
		ImGui::NextColumn();
		temp = "##input_block" + std::to_string(i);
		ImGui::Checkbox(temp.c_str(), &input_manager->block_action);
		ImGui::NextColumn();
		temp = "##name_input" + std::to_string(i);

		if (ImGui::InputText(temp.c_str(), (char*)selected_input_name.c_str(), MAX_INPUT, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			input_manager->SetName(selected_input_name.c_str());
		}
		ImGui::NextColumn();

		
		if (input_manager->GetWindowOpen())
			input_manager->ShowInspectorInfo();
		ImGui::PopID();

	}
	ImGui::Columns(1);
	ImGui::Separator();

	if (ImGui::Button("UP##button_up"))
	{
		if (selected - 1 >= 0)
			std::swap(interactive_vector[selected], interactive_vector[selected - 1]);
	}
	ImGui::SameLine();

	if (ImGui::Button("DOWN##button_down"))
	{
		if (selected + 1<size)
			std::swap(interactive_vector[selected], interactive_vector[selected + 1]);
	}
	
	ImGui::PopStyleVar();
}

void PlayerActions::Clear()
{
	if (interactive_vector.empty())
		return;
	number_of_inputs = 0;
	std::vector<InputManager*>::iterator it = interactive_vector.begin();

	while (it != interactive_vector.end())
	{
		InputManager* item = (*it);
		item->Clear();
		it++;
		RELEASE(item);
	}
	interactive_vector.clear();
}

void PlayerActions::UpdateInputsManager()
{
	if (!key_change.change_active)
	{
		if (key_change.state != KeyChange::KeyState::WAIT_FOR_KEY_STATE)
		{
			key_change.Clear();
		}
	}
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		(*it)->UpdateInputActions();
	}
}

bool PlayerActions::ReceiveEvent(SDL_Event * input_event)
{
	if (key_change.change_active)
	{
		key_change.state = key_change.ReceiveEvent(input_event);
		
		if (key_change.state != KeyChange::KeyState::WAIT_FOR_KEY_STATE)
		{
			if (key_change.state == KeyChange::KeyState::VALID_KEY_STATE
				|| key_change.state == KeyChange::KeyState::INVALID_KEY_STATE)
			{
				key_change.change_active = false;
				return true;
			}		
		}
	}
	
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (!(*it)->GetActiveInput())
			continue;
		bool result = (*it)->ProcessEvent(input_event);
		if (result || (*it)->GetBlockAction())
			return true;

	}
	return false;
}

void PlayerActions::InputManagerActive(const char * name)
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			(*it)->ActiveInput();
	}
}

void PlayerActions::InputManagerDeactive(const char * name)
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			(*it)->DeactiveInput();
	}
}

void PlayerActions::InputManagerBlock(const char * name)
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			(*it)->ActiveBlockAction();
	}
}

void PlayerActions::InputManagerUnblock(const char * name)
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			(*it)->DeactiveBlockActive();
	}
}

bool PlayerActions::InputManagerExist(const char * name)
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			return true;
	}
	return false;

}

void PlayerActions::SetInputManagerActive(const char * name, bool set)
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
		{
			(*it)->SetActiveInput(set);
		}
	}
}

void PlayerActions::SetInputManagerBlock(const char * name, bool set)
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			(*it)->SetBlockAction(set);
	}
}

bool PlayerActions::SetInputActionToChange(const char * input_action, const char * input_manager, int device, bool change_negative)
{
	DeviceCombinationType this_device = static_cast<DeviceCombinationType>((int)device);
	if (this_device == DeviceCombinationType::NULL_COMB_DEVICE)
		return nullptr;
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		InputManager* item = (*it);
		if (strcmp(item->GetName(), input_manager) == 0)
		{
			for (std::vector<InputAction*>::iterator action_it = item->action_vector.begin(); action_it != item->action_vector.end(); action_it++)
			{
				InputAction* action_item = (*action_it);
				if (strcmp(action_item->name.c_str(), input_action) == 0)
				{
					if (action_item->key_device == this_device)
					{
						key_change.SetInputAction(action_item);
						
						return true;
					}					
				}
			}

		}

	}
	return false;
}

bool PlayerActions::GetInputManagerActive(const char * name) const
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			return (*it)->GetActiveInput();
	}
	return false;
}

bool PlayerActions::GetInputManagerBlock(const char * name) const
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			return (*it)->GetBlockAction();
	}
	return false;

}

InputManager * PlayerActions::GetInputManager(const char * name) const
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			return (*it);
	}
	return nullptr;

}

InputManager * PlayerActions::GetInputManager(const char * name)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			return (*it);
	}
	return nullptr;
}

std::vector<InputManager*> PlayerActions::GetInteractiveVector() const
{
	return interactive_vector;
}

std::vector<InputManager*> PlayerActions::GetInteractiveVector()
{
	return interactive_vector;
}

bool PlayerActions::GetInput_KeyDown(const char * key, const char * input)
{

	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (!(*it)->GetActiveInput())
			continue;
		if (strcmp((*it)->GetName(), input) == 0) 
		{

			std::vector<KeyAction*> key_t= (*it)->GetKey(key);
			for (int i = 0; i < key_t.size(); i++)
			{
				if (key_t[i]->OnClick())
				{
					return true;
				}
			}

			if ((*it)->GetBlockAction())
				return false;

		}

	}

	return false;
}

bool PlayerActions::GetInput_KeyUp(const char * key, const char * input)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (!(*it)->GetActiveInput())
			continue;
		if (strcmp((*it)->GetName(), input) == 0)
		{

			std::vector<KeyAction*> key_t = (*it)->GetKey(key);
			for (int i = 0; i < key_t.size(); i++)
			{
				if (key_t[i]->OnRelease())
				{
					return true;
				}
			}

			if ((*it)->GetBlockAction())
				return false;

		}

	}

	return false;
}

bool PlayerActions::GetInput_KeyRepeat(const char * key, const char * input)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{

		if (!(*it)->GetActiveInput())
			continue;

		if (strcmp((*it)->GetName(), input) == 0)
		{

			std::vector<KeyAction*> key_t = (*it)->GetKey(key);
			for (int i = 0; i < key_t.size(); i++)
			{
				if (key_t[i]->OnRepeat())
				{
					return true;
				}
			}

			if ((*it)->GetBlockAction())
				return false;

		}

	}

	return false;
}

bool PlayerActions::GetInput_MouseButtonDown(const char* name, const char * input)
{

	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{

		if (!(*it)->GetActiveInput())
			continue;

		if (strcmp((*it)->GetName(), input) == 0)
		{

			MouseButtonAction* mouse_t = (*it)->GetMouseButton(name);
			if (mouse_t->state == Keystateaction::KEY_DOWN_ACTION)
			{
				return true;
			}
			else 
			{
				return false;
			}

			if ((*it)->GetBlockAction())
				return false;

		}

	}

	return false;
}

bool PlayerActions::GetInput_MouseButtonUp(const char * name, const char * input)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{

		if (!(*it)->GetActiveInput())
			continue; 

		if (strcmp((*it)->GetName(), input) == 0)
		{

			MouseButtonAction* mouse_t = (*it)->GetMouseButton(name);
			if (mouse_t->state == Keystateaction::KEY_UP_ACTION)
			{
				return true;
			}
			else {
				return false;
			}

		}

		if ((*it)->GetBlockAction())
			return false;

	}

	return false;
}

float PlayerActions::GetInput_ControllerAxis(const char * name, const char * input)
{
	float ret = 0.0f;
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{

		if (!(*it)->GetActiveInput())
			continue;

		if (strcmp((*it)->GetName(), input) == 0)
		{

			(*it)->GetAxis(name);

			//InputAction* axis_t = (*it)->GetAxis(name);
			std::vector<ControllerAxisAction*> axis_v = (*it)->GetAxisVector(name);
			for (int i = 0; i < axis_v.size(); i++)
			{
				ControllerAxisAction* axis_t = axis_v[i];
				if (axis_t != nullptr) {
					if (axis_t->action_type == ActionInputType::CONTROLLER_AXIS_ACTION) {
					
							ret = ((ControllerAxisAction*)axis_t)->direction_axis;
					
					}
					else if (axis_t->action_type == ActionInputType::KEY_ACTION) {
					
							ret = ((KeyAction*)axis_t)->direction_axis;
					
					}
				}
				if (ret != 0.0f)
				{
					return ret;
				}
			}
		
			
		}
		if ((*it)->GetBlockAction())
			return false;


	}
	return 0;
}

const char * PlayerActions::GetInput_ControllerActionName(const char * name, const char * input, int device, bool negative_key)
{
	DeviceCombinationType this_device = static_cast<DeviceCombinationType>((int)device);
	if (this_device == DeviceCombinationType::NULL_COMB_DEVICE)
		return "";

	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		InputManager* item = (*it);
		if (strcmp(item->GetName(), input) == 0)
		{
			for (std::vector<InputAction*>::iterator action_it = item->action_vector.begin(); action_it != item->action_vector.end(); action_it++)
			{
				InputAction* action_item = (*action_it);
				if (strcmp(action_item->name.c_str(), name) == 0)
				{					
					if (action_item->key_device == this_device)
					{
						return action_item->name.c_str();
					}
				}			
			}
			
		}

	}
	return "";
}

const char * PlayerActions::GetInput_ControllerKeyBindingName(const char * name, const char * input, int device, bool negative_key)
{
	DeviceCombinationType this_device = static_cast<DeviceCombinationType>((int)device);
	if (this_device == DeviceCombinationType::NULL_COMB_DEVICE)
		return "";
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		InputManager* item = (*it);
		if (strcmp(item->GetName(), input) == 0)
		{
			for (std::vector<InputAction*>::iterator action_it = item->action_vector.begin(); action_it != item->action_vector.end(); action_it++)
			{
				InputAction* action_item = (*action_it);
				if (strcmp(action_item->name.c_str(), name) == 0)
				{
					if (action_item->key_device == this_device)
					{
						KeyRelation* key = nullptr;
						if (negative_key&&action_item->negative_button != nullptr)
						{
							key = action_item->negative_button;
						}
						key = action_item->positive_button;
						return key->name.c_str();
					}

					
				}
			}

		}

	}
	return "";
}

void PlayerActions::SendNewDeviceCombinationType(DeviceCombinationType type)
{
	if (actual_player_action == type)
		return;
	actual_player_action = type;
	my_module->UpdateDeviceType(actual_player_action);
}

bool PlayerActions::GetChangeInputActive() const
{
	return key_change.change_active;
}

int PlayerActions::GetChangeInputState() const
{
	return (int)key_change.state;
}

DeviceCombinationType PlayerActions::SelectDeviceCombination(const char * value)
{

		if (strcmp(value, "mouse") == 0 || strcmp(value, "keyboard") == 0)
		{
			DeviceCombinationType::KEYBOARD_AND_MOUSE_COMB_DEVICE;
		}
		else if (strcmp(value, "controller") == 0)
		{
			DeviceCombinationType::CONTROLLER_COMB_DEVICE;
		}
		return DeviceCombinationType::NULL_COMB_DEVICE;
}

PlayerActions::KeyChange::KeyState PlayerActions::KeyChange::ReceiveEvent(SDL_Event * input_event)
{
	if (key_to_change == nullptr)
		return KeyState::NO_STATE;
	switch (input_event->type)
	{
	case SDL_KEYDOWN:
		if (key_to_change->key_device == DeviceCombinationType::KEYBOARD_AND_MOUSE_COMB_DEVICE)
		{
			KeyRelation* key = nullptr;

			if (change_negative)
				key = key_to_change->negative_button;			
			key = key_to_change->positive_button;

			if (key == nullptr || key->event_value == input_event->key.keysym.scancode||
				input_event->key.keysym.scancode==SDL_SCANCODE_Q || 
				input_event->key.keysym.scancode == SDL_SCANCODE_R || 
				input_event->key.keysym.scancode == SDL_SCANCODE_W || 
				input_event->key.keysym.scancode == SDL_SCANCODE_A || 
				input_event->key.keysym.scancode == SDL_SCANCODE_S || 
				input_event->key.keysym.scancode == SDL_SCANCODE_D )
				return KeyState::INVALID_KEY_STATE;
			
			if (key_to_change->my_manager == nullptr)
				return KeyState::INVALID_KEY_STATE;
			key_to_change->my_manager->ClearSameEvent(input_event, key_to_change->key_device);
			key = App->input->FindKeyBinding(key_to_change->key_device, input_event->key.keysym.scancode);
			if (change_negative)
				key_to_change->negative_button = key;
			key_to_change->positive_button = key;
			return KeyState::VALID_KEY_STATE;

		}
		break;
	case SDL_CONTROLLERBUTTONDOWN:
		if (key_to_change->key_device == DeviceCombinationType::CONTROLLER_COMB_DEVICE)
		{

			KeyRelation* key = nullptr;

			if (change_negative)
				key = key_to_change->negative_button;
			key = key_to_change->positive_button;

			if (key == nullptr || key->event_value == input_event->cbutton.button)
				return KeyState::INVALID_KEY_STATE;

			if (key_to_change->my_manager == nullptr)
				return KeyState::INVALID_KEY_STATE;

			key_to_change->my_manager->ClearSameEvent(input_event, key_to_change->key_device);

			key = App->input->FindKeyBinding(key_to_change->key_device, input_event->cbutton.button);
			if (change_negative)
				key_to_change->negative_button = key;
			key_to_change->positive_button = key;
			return KeyState::VALID_KEY_STATE;

		}

		break;
	case SDL_CONTROLLERAXISMOTION:
		if (input_event->caxis.value < -5000 || input_event->caxis.value>5000)
		{
			return KeyState::INVALID_KEY_STATE;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (key_to_change->key_device == DeviceCombinationType::KEYBOARD_AND_MOUSE_COMB_DEVICE)
		{
			KeyRelation* key = nullptr;

			if (change_negative)
				key = key_to_change->negative_button;
			key = key_to_change->positive_button;

			if (key == nullptr || key->event_value == input_event->button.button)
				return KeyState::INVALID_KEY_STATE;

			if (key_to_change->my_manager == nullptr)
				return KeyState::INVALID_KEY_STATE;

			key_to_change->my_manager->ClearSameEvent(input_event, key_to_change->key_device);
			key = App->input->FindKeyBinding(key_to_change->key_device, input_event->button.button);
			if (change_negative)
				key_to_change->negative_button = key;
			key_to_change->positive_button = key;
			return KeyState::VALID_KEY_STATE;

		}
		break;
	default:
		return 	KeyState::WAIT_FOR_KEY_STATE;
		break;
	}
	return 	KeyState::WAIT_FOR_KEY_STATE;
}

void PlayerActions::KeyChange::SetInputAction(InputAction * action_item)
{
	key_to_change = action_item;
	change_active = true;
	state = KeyState::WAIT_FOR_KEY_STATE;
}



void PlayerActions::KeyChange::Clear()
{
	key_to_change = nullptr;
	change_negative = false;
	change_active = false;
	state = KeyState::NO_STATE;
}
