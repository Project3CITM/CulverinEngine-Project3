#include "ModulePlayerActions.h"
#include "InputManager.h"
#include "ModuleEventSystem.h"

#define INPUT_MANAGER_LIMIT 5

ModulePlayerActions::ModulePlayerActions(bool start_enabled)
{
	have_config = true;

}

ModulePlayerActions::~ModulePlayerActions()
{
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

	for (uint k = 0; k < size; k++)
	{
		InputManager* input_manager = interactive_vector[k];

		ImGuiWindowFlags tree_flags = ImGuiTreeNodeFlags_Leaf;
		if (input_manager->GetWindowOpen())
			tree_flags |= ImGuiTreeNodeFlags_Selected;

		std::string name = "Input "+std::to_string(k+1);
		bool active = ImGui::TreeNodeEx(name.c_str(), tree_flags);
		ImGui::SameLine();


		if (ImGui::Button("UP#button_up"))
		{
			if(k-1>0)
				std::swap(interactive_vector[k], interactive_vector[k-1]);
		}
		if (ImGui::Button("DOWN#button_down"))
		{
			if (k + 1<size)
				std::swap(interactive_vector[k], interactive_vector[k + 1]);
		}
		if (ImGui::IsItemClicked())
		{
			input_manager->ActiveWindowOpen();
		}
		if (input_manager->GetWindowOpen())
			input_manager->ShowInspectorInfo();

	}

	
	ImGui::PopStyleVar();
	return UPDATE_CONTINUE;
}

bool ModulePlayerActions::SetEventListenrs()
{
	
	AddListener(EventType::EVENT_BUTTON_MOUSE, this);
	AddListener(EventType::EVENT_MOTION_MOUSE, this);
	AddListener(EventType::EVENT_AXIS_CONTROLLER, this);
	AddListener(EventType::EVENT_BUTTON_CONTROLLER, this);
	AddListener(EventType::EVENT_KEYBOARD, this);
	
	return true;
}

void ModulePlayerActions::OnEvent(Event & custom_event)
{
	
	switch (custom_event.type)
	{
	case EventType::EVENT_BUTTON_MOUSE:
		/*
		RayCast2D
		HandleInteractive
		if Raycast2d don't hit Raycast3D

		UpdateActions

		
		*/
		break;
	case EventType::EVENT_MOTION_MOUSE:
		/*
		RayCast2D
		HandleInteractive
		if Raycast2d don't hit Raycast3D	

		UpdateActions
		*/

		break;
	case EventType::EVENT_AXIS_CONTROLLER:
		/*
		UpdateActions
		*/
		break;
	case EventType::EVENT_BUTTON_CONTROLLER:
		/*
		UpdateActions
		*/
		break;
	case EventType::EVENT_KEYBOARD:
		/*
		UpdateActions

		Todo: implement a GetKey without InputAction?
		Think update:
			-first loop press T
			-this frame T is pressed
			-next frame T is not pressed<---how we update this (discuss this tomorrow)		
		*/
		break;
	}

}

bool ModulePlayerActions::SaveConfig(JSON_Object * node)
{
	return true;
}

void ModulePlayerActions::ReceiveEvent(SDL_Event * input_event)
{
	for (std::vector<InputManager*>::iterator it = interactive_vector.begin(); it != interactive_vector.end(); it++)
	{
		
	}
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
