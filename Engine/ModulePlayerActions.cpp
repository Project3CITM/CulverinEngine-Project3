#include "ModulePlayerActions.h"
#include "InputManager.h"
#include "ModuleEventSystem.h"

#define INPUT_MANAGER_LIMIT 5

ModulePlayerActions::ModulePlayerActions(bool start_enabled)
{
	have_config = true;
	name = "Player Action";

}

ModulePlayerActions::~ModulePlayerActions()
{
}

update_status ModulePlayerActions::Update(float dt)
{

	/*for (int i = 0; i < interactive_vector.size(); i++)
	{
		interactive_vector[i]->GetKey("Jump");
	}*/

	return update_status::UPDATE_CONTINUE;
}

update_status ModulePlayerActions::UpdateConfig(float dt)
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

	ImGui::Text("Number of Inputs");

	uint size = interactive_vector.size();

	for (int k = 0; k < size; k++)
	{
		InputManager* input_manager = interactive_vector[k];

		ImGuiWindowFlags tree_flags = ImGuiTreeNodeFlags_Leaf;
		if (input_manager->GetWindowOpen())
			tree_flags |= ImGuiTreeNodeFlags_Selected;

		std::string name = "Input "+std::to_string(k+1);
		ImGui::AlignFirstTextHeightToWidgets();

		bool active = ImGui::TreeNodeEx(name.c_str(), tree_flags);

		if (ImGui::IsItemClicked())
		{
			input_manager->ActiveWindowOpen();
		}
		ImGui::SameLine();

		if (ImGui::Button("UP##button_up"))
		{
			if(k-1>=0)
				std::swap(interactive_vector[k], interactive_vector[k-1]);
		}
		ImGui::SameLine();

		if (ImGui::Button("DOWN##button_down"))
		{
			if (k + 1<size)
				std::swap(interactive_vector[k], interactive_vector[k + 1]);
		}
		ImGui::SameLine();
		ImGui::Checkbox("Active##input_active", &input_manager->active_input);
		ImGui::SameLine();
		ImGui::Checkbox("Block##input_block", &input_manager->block_action);

		if (input_manager->GetWindowOpen())
			input_manager->ShowInspectorInfo();
		if (active)
		{
			ImGui::TreePop();
		}
	}

	
	ImGui::PopStyleVar();
	return UPDATE_CONTINUE;
}


bool ModulePlayerActions::SaveConfig(JSON_Object * node)
{

	return true;
}

void ModulePlayerActions::UpdateInputsManager()
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		(*it)->UpdateInputActions();
	}
}

bool ModulePlayerActions::ReceiveEvent(SDL_Event * input_event)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (!(*it)->GetActiveInput())
			continue;
		bool result= (*it)->ProcessEvent(input_event);
		if (result || (*it)->GetBlockAction())
			return true;

	}
	return false;
}

InputManager * ModulePlayerActions::GetInputManager(const char * name) const
{
	for (std::vector<InputManager*>::const_iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			return (*it);
	}
	return nullptr;

}

InputManager * ModulePlayerActions::GetInputManager(const char * name)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		if (strcmp((*it)->GetName(), name) == 0)
			return (*it);
	}
	return nullptr;
}
