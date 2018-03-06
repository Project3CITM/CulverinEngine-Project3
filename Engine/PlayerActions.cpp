#include "PlayerActions.h"
#include "InputManager.h"
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
				InputManager* input_manager = new InputManager();
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
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		(*it)->UpdateInputActions();
	}
}

bool PlayerActions::ReceiveEvent(SDL_Event * input_event)
{
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

void PlayerActions::SetInputManagerActive(const char * name, bool set)
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			(*it)->ActiveInput();
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
		if (strcmp((*it)->GetName(), input) == 0) 
		{
			KeyAction* key_t= (*it)->GetKey(key);
			if (key_t->state == Keystateaction::KEY_DOWN_ACTION) 
			{
				return true;
			}
			else 
			{
				return false;
			}
		}

	}

	return false;
}

bool PlayerActions::GetInput_KeyUp(const char * key, const char * input)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), input) == 0)
		{

			KeyAction* key_t = (*it)->GetKey(key);
			if (key_t != nullptr) {
				if (key_t->state == Keystateaction::KEY_UP_ACTION)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}

	}

	return false;
}

bool PlayerActions::GetInput_KeyRepeat(const char * key, const char * input)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), input) == 0)
		{

			KeyAction* key_t = (*it)->GetKey(key);
			if (key_t->state == Keystateaction::KEY_REPEAT_ACTION)
			{
				return true;
			}
			else
			{
				return false;
			}

		}

	}

	return false;
}

bool PlayerActions::GetInput_MouseButtonDown(const char* name, const char * input)
{

	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
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

		}

	}

	return false;
}

bool PlayerActions::GetInput_MouseButtonUp(const char * name, const char * input)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
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

	}

	return false;
}

float PlayerActions::GetInput_ControllerAxis(const char * name, const char * input)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), input) == 0)
		{

			ControllerAxisAction* axis_t = (*it)->GetAxis(name);
			if (axis_t!=nullptr)
			{
				return axis_t->direction_axis;
			}
		}

	}
	return 0;
}
