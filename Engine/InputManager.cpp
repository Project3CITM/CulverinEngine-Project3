#include "InputManager.h"
#include "ImGui/imgui.h"
#include "InputAction.h"
#include"Application.h"
#define ACTION_LIMIT 50
#define MAX_INPUT 25

InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

void InputManager::UpdateInputActions()
{

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	int mouse_x = 0;
	int mouse_y = 0;
	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
	for (std::vector<InputAction*>::iterator it = action_vector.begin(); it != action_vector.end(); it++)
	{

		switch ((*it)->action_type) {

		case ActionInputType::KEY_ACTION:
			((KeyAction*)(*it))->UpdateEventAction(keys);
			break;

		case ActionInputType::MOUSE_BUTTON_ACTION:
			((MouseButtonAction*)(*it))->UpdateEventAction(mouse_x, mouse_y, buttons);
			break;


		}
	
	}
}

bool InputManager::ProcessEvent(SDL_Event * input_event)
{
	for (std::vector<InputAction*>::iterator it = action_vector.begin(); it != action_vector.end(); it++)
	{
		(*it)->ProcessEventAction(input_event);
	}
	return false;

}

void InputManager::Clear()
{
	if (action_vector.empty())
		return;
	number_of_action = 0;
	std::vector<InputAction*>::iterator it = action_vector.begin();

	while (it != action_vector.end())
	{
		InputAction* item = (*it);
		it++;
		RELEASE(item);
	}
	action_vector.clear();
}

KeyAction * InputManager::GetKey(const char * name)
{

	for (int i = 0; i < action_vector.size(); i++) 
	{
		if (action_vector[i]->name == name && action_vector[i]->action_type == ActionInputType::KEY_ACTION)
		{
			return (KeyAction*)action_vector[i];
		}
	}

	return nullptr;
}

MouseButtonAction * InputManager::GetMouseButton(const char * name)
{
	for (int i = 0; i < action_vector.size(); i++)
	{
		if (action_vector[i]->name == name && action_vector[i]->action_type == ActionInputType::MOUSE_BUTTON_ACTION)
		{
			return (MouseButtonAction*)action_vector[i];
		}
	}

	return nullptr;
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
	ImGui::Columns(2, "my_action");
	ImGui::Text("Action"); 
	ImGui::NextColumn();
	ImGui::Text("Key Binding"); 
	ImGui::NextColumn();
	static int selected = 0;
	for (int i = 0; i < action_vector.size(); i++)
	{
		InputAction* action = action_vector[i];
		std::string tree_action_name = action->name +"##"+ std::to_string(i);
		//bool active = ImGui::TreeNodeEx(tree_action_name.c_str());
		if (ImGui::Selectable(tree_action_name.c_str(), selected == i, ImGuiSelectableFlags_SpanAllColumns))
		{
			selected = i;
			selected_action_name.clear();
			selected_action_name = action->name;
			selected_action_key.clear();
			selected_action_key = action->key_relation->name;
			action_type = action_vector[selected]->action_type;

		}
		bool hovered = ImGui::IsItemHovered();

		ImGui::NextColumn();
		ImGui::Text(action->key_relation->name.c_str());
		ImGui::NextColumn();

	}
	ImGui::Columns(1); 
	if (action_vector.empty())
	{
		ImGui::End();
		return;
	}
	ImGui::Separator();

	ImGui::InputText("Action Name##name_input", (char*)selected_action_name.c_str(), MAX_INPUT);
	ImGui::InputText("Key Name##key_input", (char*)selected_action_key.c_str(), MAX_INPUT);
	std::string action_type_names;
	action_type_names += "Axis";
	action_type_names += '\0';
	action_type_names += "Controller Axis";
	action_type_names += '\0';
	action_type_names += "Keyboard";
	action_type_names += '\0';
	action_type_names += "Mouse Button";
	action_type_names += '\0';
	action_type_names += "Controller Button";
	action_type_names += '\0';
	
		ImGui::Combo("Type##type_action", &action_type, action_type_names.c_str());
		if (ImGui::Button("Apply##apply_action"))
		{
			InputAction* new_action = CreateNewAction(selected_action_name.c_str(), selected_action_key.c_str(), static_cast<ActionInputType>(action_type));
			if (new_action != nullptr && selected >= 0 && selected < action_vector.size()) {
				InputAction* temp = action_vector[selected];
				RELEASE(temp);
				action_vector[selected] = new_action;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Discard##discard_action"))
		{
			if (selected > 0 && selected < action_vector.size())
			{
				selected_action_name = action_vector[selected]->name;
				selected_action_key = action_vector[selected]->key_relation->name.c_str();
				action_type = action_vector[selected]->action_type;
			}
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

const bool InputManager::GetBlockAction() const
{
	return block_action;
}
bool InputManager::GetBlockAction() 
{
	return block_action;
}

const bool InputManager::GetActiveInput() const
{
	return active_input;
}
bool InputManager::GetActiveInput() 
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
	
	KeyRelation* new_key_relation = App->module_key_binding->Find_key_binding(new_key_binding);
	InputAction* temp = nullptr;
	bool can_create_action = true;
	switch (new_type) {

	case ActionInputType::AXIS_ACTION:
		temp = new AxisAction();
		break;

	case ActionInputType::CONTROLLER_AXIS_ACTION:
		temp = new ControllerAxisAction();
		break;

	case ActionInputType::BUTTON_ACTION:
	case ActionInputType::KEY_ACTION:
		temp = new KeyAction(new_type);
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

std::vector<InputAction*> InputManager::GetActionVector() const
{
	return action_vector;
}

std::vector<InputAction*> InputManager::GetActionVector()
{
	return action_vector;
}
