#include "ModuleAnimation.h"
#include "GameObject.h"
#include "Application.h"
#include "JSONSerialization.h"
#include "ModuleGUI.h"
#include "WindowHierarchy.h"
#include "AnimableComponent.h"
#include "Component.h"

ModuleAnimation::ModuleAnimation()
{
}


ModuleAnimation::~ModuleAnimation()
{
}

void ModuleAnimation::ShowAnimationWindow(bool & active)
{
	
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.176f, 0.176f, 0.176f, 1.0f));
		if (!ImGui::Begin("Animation UI Window", &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar))
		{
			ImGui::End();
		}
		if (((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->GetSelected() != nullptr)
		{
			GameObject* go = ((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->GetSelected();
			if (go->FindComponentByType(Comp_Type::C_ANIMATION_UI) == nullptr)
			{
				ImGui::Text("This GameObject don't have component animation create one");
				if(ImGui::Button("Create CompAnimation"))
				{
					go->AddComponent(Comp_Type::C_ANIMATION_UI);
				}
				ImGui::End();
				return;
			}
			/*
			for (std::vector<Component*>::const_iterator it = go->GetComponentVec().begin(); it != go->GetComponentVec().end(); it++)
			{
				//(*it)
				AnimableComponent* item = dynamic_cast<AnimableComponent*>((*it));

				if (item == nullptr)
					continue;
				
				

			}

			*/

		}
		ImGui::End();
	

}

void ModuleAnimation::FindAnimComponent()
{
}

void ModuleAnimation::SaveAnimation()
{
}

void ModuleAnimation::LoadAnimation()
{
}
