#include "ModulePlayerActions.h"
#include "InputManager.h"
#include "ModuleEventSystem.h"

#define INPUT_MANAGER_LIMIT 5
#define MAX_INPUT 20
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

	ImGui::Text("Number of Inputs");

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
