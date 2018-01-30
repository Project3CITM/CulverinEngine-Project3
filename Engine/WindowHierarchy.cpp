#include "WindowHierarchy.h"
#include "Application.h"
#include "Scene.h"
#include "GameObject.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"

Hierarchy::Hierarchy() : WindowManager()
{
	active.push_back(Active());
	name = "Hierarchy";
}

Hierarchy::~Hierarchy()
{
	active.clear();
}

bool Hierarchy::Start()
{

	return true;
}

update_status Hierarchy::Update(float dt)
{
	if (active[0].active)
	{
		ShowHierarchy();

		// Ctr+Copy - Ctr+Paste
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			{
				if (selected != nullptr)
				{
					//copy = selected;
				}
			}
			if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
			{
				if (selected != nullptr && copy != nullptr)
				{
					//CopyGameObject(selected);
				}
			}
		}
	}
	return UPDATE_CONTINUE;
}

void Hierarchy::ShowHierarchy()
{
	if (!BeginDock("Hierarchy", NULL, ImGuiWindowFlags_HorizontalScrollbar))
	{
		EndDock();
		return;
	}

	if (ImGui::IsMouseHoveringRect(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y)))
	{
		if (ImGui::GetIO().MouseClicked[1] && ImGui::IsMouseHoveringWindow())
		{
			ImGui::OpenPopup("OptionsHyerarchy");
		}
	}
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 3));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 6));
	if (ImGui::BeginPopup("OptionsHyerarchy"))
	{
		selected = ((Inspector*)App->gui->winManager[INSPECTOR])->GetSelected();
		ShowOptions();
		ImGui::EndMenu();
	}
	ImGui::PopStyleVar();
	for (uint i = 0; i < App->scene->gameobjects.size(); i++)
	{
		ImGui::PushID(i);
		App->scene->gameobjects[i]->ShowHierarchy();
		ImGui::PopID();
	}
	ImGui::PopStyleVar();

	// Open Window To Delete GameObject
	if (toDelete != nullptr)
	{
		if (showconfirmDelete == false && dont_ask_me_next_time)
		{
			toDelete->SettoDelete();
			toDelete = nullptr;
		}
		else
		{
			ShowDeleteWindow();
		}
	}

	if (App->scene->drag && ImGui::IsMouseReleased(0))
		App->scene->drag = nullptr;

	EndDock();
}

void Hierarchy::ShowOptions()
{
	if (ImGui::MenuItem("Copy"))
	{
		if (selected != nullptr)
		{
			copy = selected;
		}
	}
	if (ImGui::MenuItem("Paste"))
	{
		if (selected != nullptr && copy != nullptr)
		{
			CopyGameObject(selected);
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Rename", NULL, false, false))
	{
		// Not functional
	}
	if (ImGui::MenuItem("Duplicate", NULL, false, false))
	{
		if (selected != nullptr)
		{
			// selected->GetParent()->AddChildGameObject_Copy(selected);
		}
	}
	if (ImGui::MenuItem("Delete"))
	{
		if (selected != nullptr && App->engineState == EngineState::STOP)
		{
			SetGameObjecttoDelete(selected);
		}
		else
		{
			LOG("Deleting a GameObject while PlayMode may cause crashes... you can't delete now.");
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Create Empty"))
	{
		GameObject* empty = App->scene->CreateGameObject();
		App->gui->SetLinkInspector(empty);
	}
	if (ImGui::MenuItem("Create Cube"))
	{
		GameObject* cube = App->scene->CreateCube();
		App->gui->SetLinkInspector(cube);
	}
	//if (ImGui::MenuItem("Create Sphere"))
	//{
	//	GameObject* sphere = App->scene->CreateSphere();
	//	App->gui->SetLinkInspector(sphere);
	//}
}

void Hierarchy::ShowDeleteWindow()
{
	ImGui::OpenPopup("Delete GameObject");
	if (ImGui::BeginPopupModal("Delete GameObject", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Are you sure you wan't to delete '%s'", toDelete->GetName());
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::Checkbox("Don't ask me next time", &wait_toSelect);
		ImGui::PopStyleVar();
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			showconfirmDelete = !wait_toSelect;
			dont_ask_me_next_time = wait_toSelect;
			toDelete->SettoDelete();
			toDelete = nullptr;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			showconfirmDelete = !wait_toSelect;
			dont_ask_me_next_time = wait_toSelect;
			toDelete = nullptr;
		}
	}
	ImGui::EndPopup();
}

void Hierarchy::SetGameObjecttoDelete(GameObject* todelete)
{
	toDelete = todelete;
}

void Hierarchy::SetGameObjectCopy(GameObject* copy_)
{
	if (copy_ != nullptr)
	{
		copy = copy_;
	}
}

void Hierarchy::CopyGameObject(GameObject* select)
{
	if (select != nullptr && copy != nullptr)
	{
		if (select->HaveParent())
		{
			GameObject* copied = new GameObject(*copy, true, select->GetParent());
			select->GetParent()->AddChildGameObject_Load(copied);
			//copy = nullptr;
		}
		else
		{
			GameObject* copied = new GameObject(*copy);
			App->scene->gameobjects.push_back(copied);
			//copy = nullptr;
		}
	}
}

const GameObject* Hierarchy::GetCopied() const
{
	return copy;
}

void Hierarchy::SetGameObjectSelected(GameObject* select)
{
	if (select != nullptr)
	{
		selected = select;
	}
}

void Hierarchy::ChangeShowConfirmDelete()
{
	dont_ask_me_next_time = !showconfirmDelete;
	wait_toSelect = false;
}


bool Hierarchy::CleanUp()
{
	return true;
}
