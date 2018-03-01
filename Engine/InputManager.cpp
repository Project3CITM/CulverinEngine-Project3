#include "InputManager.h"
#include "ImGui/imgui.h"
#include "InputAction.h"
#include"Application.h"
#define ACTION_LIMIT 50


InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

void InputManager::LookEvent(SDL_Event * input_event)
{
}

void InputManager::ShowInspectorInfo()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	std::string window_name = "Input Manager " + name;
	ImGui::Begin(window_name.c_str(), &window_open, window_flags);

	ImGui::Text("Number of Action");
	if (ImGui::InputInt("##number_of_action", &number_of_action))
	{
		
		if (number_of_action < 0)
		{
			number_of_action = 0;
		}
		else if (number_of_action >= ACTION_LIMIT)
		{
			number_of_action = ACTION_LIMIT;
		}
		if (action_vector.size()<number_of_action)
		{
			for (int i = action_vector.size(); i < number_of_action; i++)
			{
				InputAction* action = new InputAction();
				action_vector.push_back(action);
			}
		}
		else
		{
			int size = action_vector.size() - number_of_action;
			for (int i = 0; i < size; i++)
			{

				InputAction* action = action_vector.back();
				action_vector.pop_back();
				RELEASE(action);
			}
		}
		
	}
	
	ImGui::Text("Action:");
	for (std::vector<InputAction*>::iterator it = action_vector.begin(); it != action_vector.end(); it++)
	{
		//std::string tree_action_name = (*it)->GetName() +"##"+ std::to_string(std::distance(action_vector.begin(), it));
	//	bool active = ImGui::TreeNodeEx(tree_action_name.c_str());
		//if(active)
	//	{
		//	(*it)->ShowInspectorInfo();
	//	}
	}
	

	ImGui::End();

}

void InputManager::ActiveInput()
{
	active_input = true;
}

void InputManager::DeactiveInput()
{
	active_input = false;
}

void InputManager::ActiveBlockAction()
{
	block_action = true;
}

void InputManager::DeactiveBlockActive()
{
	block_action = false;
}

void InputManager::ActiveWindowOpen()
{
	window_open = true;
}

void InputManager::DeactiveWIndowOpen()
{
	window_open = false;
}

void InputManager::SetActiveInput(bool set)
{
	active_input = set;
}

void InputManager::SetBlockAction(bool set)
{
	block_action = set;
}

void InputManager::SetName(const char * set_name)
{
	name = set_name;
}

const char * InputManager::GetName()const
{
	return name.c_str();
}

bool InputManager::GetBlockAction() const
{
	return block_action;
}

bool InputManager::GetActiveInput() const
{
	return active_input;
}

bool InputManager::GetWindowOpen() const
{
	return window_open;
}

InputAction* InputManager::CreateNewAction(const char * new_name, const char * new_key_binding, ActionInputType new_type)
{

	/*KeyRelation new_key_relation = KeyRelation(new_name);
	new_key_relation.key_type= new_type*/

	KeyRelation new_key_relation = App->module_key_binding->Find_key_binding(new_key_binding);
	InputAction* temp = nullptr;
	bool can_create_action = true;
	switch (new_type) {

	case ActionInputType::AXIS_ACTION:
		temp = new AxisAction();
		break;

	case ActionInputType::BUTTON_ACTION:
		temp = new ButtonAction();
		break;
	case ActionInputType::CONTROLLER_AXIS_ACTION:
		temp = new ControllerAxisAction();
		break;

	case ActionInputType::KEY_ACTION:
		temp = new KeyAction();
		break;

	case ActionInputType::MOUSE_BUTTON_ACTION:
		temp = new MouseButtonAction();
		break;
	}

	if (temp != nullptr)
	{
		temp->name = new_name;
		temp->key_relation = new_key_relation;
	}

	return temp;

}
