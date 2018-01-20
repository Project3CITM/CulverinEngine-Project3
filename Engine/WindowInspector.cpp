#include "Application.h"
#include "WindowInspector.h"
#include "ModuleWindow.h"
#include "GameObject.h"

Inspector::Inspector() : WindowManager()
{
	active.push_back(Active());
	name = "Inspector";
}

Inspector::~Inspector()
{
	active.clear();
	selected_object = nullptr;
}

bool Inspector::Start()
{

	return true;
}

update_status Inspector::Update(float dt)
{
	if (active[0].active)
		ShowInspector();
	return UPDATE_CONTINUE;
}

void Inspector::ShowInspector()
{
	static int width;
	static int height;
	SDL_GetWindowSize(App->window->window, &width, &height);
	ImGui::SetNextWindowPos(ImVec2(width - 300, 20));
	ImGui::SetNextWindowSize(ImVec2(300, height - 20 - (height - 700)));
	
	if (!BeginDock("Inspector", NULL, ImGuiWindowFlags_NoScrollbar))
	{
		EndDock();
		return;
	}
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 3));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	if (selected_object != nullptr)
	{ 
		selected_object->ShowInspectorInfo();
	}
	ImGui::PopStyleVar(2);
	EndDock();
}

void Inspector::LinkObject(GameObject * obj)
{
	if (obj != nullptr)
	{
		selected_object = obj;
	}
}

void Inspector::SetLinkObjectNull()
{
	selected_object = nullptr;
}

GameObject* Inspector::GetSelected() const
{
	return selected_object;
}

bool Inspector::CleanUp()
{

	return true;
}
