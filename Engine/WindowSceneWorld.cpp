#include "WindowSceneWorld.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ImGui/ImGuizmo.h"
#include "ModuleCamera3D.h"
#include "Scene.h"
#include "ImGui\ImGuizmo.h"
#define PICK_OFFSET_Y -15

SceneWorld::SceneWorld() : WindowManager()
{
	active.push_back(Active());
	name = "SceneWorld";
}

SceneWorld::~SceneWorld()
{
	active.clear();
}

bool SceneWorld::Start()
{
	return true;
}

update_status SceneWorld::Update(float dt)
{
	if (active[0].active)
	{
		ShowSceneWorld();
	}

	return UPDATE_CONTINUE;
}

void SceneWorld::Draw()
{
	if (active[0].active)
	{
		ImGui::Image((void*)App->scene->scene_buff->GetTexture(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	}
}

void SceneWorld::ShowSceneWorld()
{
	if (!BeginDock("Scene", NULL, ImGuiWindowFlags_NoScrollWithMouse))
	{
		EndDock();
		return;
	}

	//Get from dock the parameters of this window to access them externaly
	SetWindowParams();
	//LOG("X:%f Y:%f W:%f H:%f", SceneWindow.x, SceneWindow.y, SceneWindow.z, SceneWindow.w);
	ImGuizmo::SetDrawlist();
	// Generate mouse ray ---------------------------
	bool test = ImGuizmo::IsOver();
	if (!test) {
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT)
		{
			mouse_pos.x = ImGui::GetMousePos().x - scene_window.x;
			mouse_pos.y = ImGui::GetMousePos().y - scene_window.y;

			if (mouse_pos.x >= 0 && mouse_pos.y >= 0 &&
				mouse_pos.x <= scene_window.z && mouse_pos.y <= scene_window.w && ImGui::IsMouseHoveringWindow())
			{
				App->camera->MousePick(mouse_pos.x, mouse_pos.y + PICK_OFFSET_Y, scene_window.z, scene_window.w);
			
			}
		}
	}
	App->camera->can_move_cam = ImGui::IsMouseHoveringWindow(); //TODO ELLIOT CHange to variable in WindowManager.h
	
	ImGui::Image((void*)App->scene->scene_buff->GetTexture(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0)); // Set texture to buffer
	ImGui::Image((void*)App->scene->glow_buff->GetTexture(), ImVec2(512,512), ImVec2(0, 1), ImVec2(1, 0)); // Set texture to buffer

	EndDock();
}

bool SceneWorld::CleanUp()
{
	return true;
}

void SceneWorld::SetWindowParams() 
{
	scene_window = GetDockParams("Scene");
}

float4 SceneWorld::GetWindowParams() const
{
	return scene_window;
}
