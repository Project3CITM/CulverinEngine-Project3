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
		selected = ((Inspector*)App->gui->win_manager[INSPECTOR])->GetSelected();
		ShowOptions();
		ImGui::EndMenu();
	}
	ImGui::PopStyleVar();
	// Iterate all root and show in Hierarchy all GameObjects -----------
	App->scene->root->ShowHierarchy();
	// ----------------------------------------------------------------
	ImGui::PopStyleVar();

	// Open Window To Delete GameObject
	if (to_delete != nullptr)
	{
		if (show_confirm_delete == false && dont_ask_me_next_time)
		{
			to_delete->SettoDelete();
			to_delete = nullptr;
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
		if (selected != nullptr && selected->GetUUID() != 1)
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
		if (selected != nullptr && App->engine_state == EngineState::STOP)
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
	if (ImGui::BeginMenu("3D Object"))
	{
		if (ImGui::MenuItem("Create Cube"))
		{
			GameObject* cube = App->scene->CreateCube();
			App->gui->SetLinkInspector(cube);
		}
		ImGui::MenuItem("Create Sphere", NULL, false, false);
		ImGui::MenuItem("Create Capsule", NULL, false, false);
		ImGui::MenuItem("Create Cylinder", NULL, false, false);
		ImGui::MenuItem("Create Plane", NULL, false, false);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("UI"))
	{
		if (ImGui::MenuItem("Canvas"))
		{
			GameObject* canvas = App->scene->CreateCanvas(nullptr);
			App->gui->SetLinkInspector(canvas);

		}
		if (ImGui::MenuItem("Image"))
		{
			GameObject* canvas = App->scene->FindCanvas();
			if (canvas == nullptr)
			{
				canvas = App->scene->CreateCanvas(nullptr);
			}
			GameObject* text = App->scene->CreateImage(canvas);	
			App->gui->SetLinkInspector(text);

		}
		if (ImGui::MenuItem("Button"))
		{
			GameObject* canvas = App->scene->FindCanvas();
			if (canvas == nullptr)
			{
				canvas = App->scene->CreateCanvas(nullptr);
			}
			GameObject* button = App->scene->CreateButton(canvas);	
			App->gui->SetLinkInspector(button);

		}
		if (ImGui::MenuItem("Check Box"))
		{
			GameObject* canvas = App->scene->FindCanvas();
			if (canvas == nullptr)
			{
				canvas = App->scene->CreateCanvas(nullptr);
			}
			GameObject* check_box = App->scene->CreateCheckBox(canvas);
			App->gui->SetLinkInspector(check_box);

		}
		if (ImGui::MenuItem("Text"))
		{
			GameObject* canvas = App->scene->FindCanvas();
			if (canvas == nullptr)
			{
				canvas = App->scene->CreateCanvas(nullptr);
			}
			GameObject* text = App->scene->CreateText(canvas);
			App->gui->SetLinkInspector(text);

		}
		if (ImGui::MenuItem("Check Box"))
		{
			GameObject* canvas = App->scene->FindCanvas();
			if (canvas == nullptr)
			{
				canvas = App->scene->CreateCanvas(nullptr);
			}
			GameObject* edit_text = App->scene->CreateEditText(canvas);
			App->gui->SetLinkInspector(edit_text);

		}
		ImGui::EndMenu();
	}
}

void Hierarchy::ShowDeleteWindow()
{
	ImGui::OpenPopup("Delete GameObject");
	if (ImGui::BeginPopupModal("Delete GameObject", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Are you sure you wan't to delete '%s'", to_delete->GetName());
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::Checkbox("Don't ask me next time", &wait_to_select);
		ImGui::PopStyleVar();
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			show_confirm_delete = !wait_to_select;
			dont_ask_me_next_time = wait_to_select;
			to_delete->SettoDelete();
			to_delete = nullptr;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			show_confirm_delete = !wait_to_select;
			dont_ask_me_next_time = wait_to_select;
			to_delete = nullptr;
		}
	}
	ImGui::EndPopup();
}

void Hierarchy::SetGameObjecttoDelete(GameObject* todelete)
{
	to_delete = todelete;
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
			select->GetParent()->AddChildGameObject(copied);
			//copy = nullptr;
		}
		else
		{
			GameObject* copied = new GameObject(*copy);
			App->scene->root->AddChildGameObject(copied);
			//copy = nullptr;
		}
	}
}

const GameObject* Hierarchy::GetCopied() const
{
	return copy;
}

GameObject* Hierarchy::GetSelected()
{
	return selected;
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
	dont_ask_me_next_time = !show_confirm_delete;
	wait_to_select = false;
}


bool Hierarchy::CleanUp()
{
	return true;
}
