#include "CompCanvas.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompCanvasRender.h"
#include "EventDef.h"
#include "ModuleEventSystem.h"
#include "ModuleRenderGui.h"
#include "CompRectTransform.h"
CompCanvas::CompCanvas(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Canvas";
	my_transform = (CompRectTransform*)parent->FindComponentByType(Comp_Type::C_RECT_TRANSFORM);
	UpdateCanvasScale();

	/*
	draw_mode.type = EventType::EVENT_DRAW;
	draw_mode.draw.Dtype = draw_mode.draw.DRAW_2D;
	draw_mode.draw.ToDraw = this;
	draw_mode.draw.DistanceCamToObject = 0.0f;
	*/
}

CompCanvas::CompCanvas(const CompCanvas & copy, GameObject * parent) :Component(Comp_Type::C_CANVAS, parent)
{
}

CompCanvas::~CompCanvas()
{
}
void CompCanvas::PreUpdate(float dt)
{
	UpdateCanvasScale();
}
void CompCanvas::Update(float dt)
{
	App->render_gui->screen_space_canvas.push_back(this);
	//PushEvent(draw_mode);
	
}

void CompCanvas::Clear()
{
	for (int i = 0;i<App->render_gui->screen_space_canvas.size(); i++)
	{
		if (App->render_gui->screen_space_canvas[i] == this)
		{
			App->render_gui->screen_space_canvas.erase(App->render_gui->screen_space_canvas.begin() + i);
		}
	}
}

void CompCanvas::ShowOptions()
{
	//ImGui::MenuItem("CREATE", NULL, false, false);
	if (ImGui::MenuItem("Reset"))
	{
		// Not implmented yet.
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component"))
	{
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetComponentCopy(this);
	}
	if (ImGui::MenuItem("Paste Component As New", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (parent->FindComponentByType(((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType()) == nullptr
			|| ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType() > Comp_Type::C_CAMERA)
		{
			parent->AddComponentCopy(*((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied());
		}
	}
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompCanvas*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompCanvas::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsCanvas");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsCanvas"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	ImGui::TreePop();
}

void CompCanvas::CopyValues(const CompCanvas* component)
{
	//more...
}

void CompCanvas::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	//...
}

void CompCanvas::Load(const JSON_Object* object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	//...
	Enable();
}



void CompCanvas::SyncComponent()
{
	my_transform = (CompRectTransform*)parent->FindComponentByType(Comp_Type::C_RECT_TRANSFORM);

}
void CompCanvas::UpdateCanvasScale()
{
	ImGuiIO& io = ImGui::GetIO();

	my_transform->SetPos(float3(io.DisplaySize.x*0.5f, io.DisplaySize.y*0.5f, 0.0f));

	my_transform->SetWidth(io.DisplaySize.x);
	my_transform->SetHeight(io.DisplaySize.y);

}
void CompCanvas::AddGraphic(CompGraphic * to_add)
{
	graphic_vector.push_back(to_add);
}

void CompCanvas::RemoveGraphic(CompGraphic * to_remove)
{
	for (int i = 0; i < graphic_vector.size(); i++)
	{
		if (graphic_vector[i] == to_remove)
		{
			LOG("CanvasRender Removed from Canvas");
			graphic_vector.erase(graphic_vector.begin() + i);
			return;
		}
	}
	LOG("Error:CanvasRender not found on this Canvas");
}
void CompCanvas::DrawDebugRectTransform()
{
	if (App->engine_state == EngineState::STOP)
	{
		my_transform->DrawRectTransform();
	}
}
void CompCanvas::DrawGraphic()
{
	for (int i = 0; i < graphic_vector.size(); i++)
	{
		graphic_vector[i]->DrawGraphic();
	}
	
}
void CompCanvas::SetDefaultTexture(int texture)
{
	default_texture_id = texture;
}
int CompCanvas::GetDefaultTexture()const
{
	return default_texture_id;
}

