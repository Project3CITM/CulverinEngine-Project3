#include "CompImage.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "ImportMaterial.h"
#include "ResourceMaterial.h"
#include "CompCanvasRender.h"
#include "CompRectTransform.h"
#include "CompCanvas.h"
#include "ModuleFS.h"
#include <vector>
#include "CompSlider.h"
#include "ModuleInput.h"

CompSlider::CompSlider(Comp_Type t, GameObject * parent) : CompInteractive (t, parent)
{
	uid = App->random->Int();
	name_component = "CompSlider";
	dragable = true;
}

CompSlider::CompSlider(const CompImage & copy, GameObject * parent) : CompInteractive(Comp_Type::C_SLIDER, parent)
{

}

CompSlider::~CompSlider()
{
}

void CompSlider::ShowOptions()
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
			CopyValues(((CompSlider*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompSlider::ShowInspectorInfo()
{
	int op = ImGui::GetWindowWidth() / 4;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("Options Slider");
	}
	ImGui::PopStyleVar();

	if (ImGui::Button("Sync Min/Max", ImVec2(120, 0)))
	{
		SyncBar();
	}
	ImGui::Text("Min pos: %f", min_pos);
	ImGui::Text("Max pos: %f", max_pos);

	if (ImGui::DragFloat("##fillQuantity", &fill, 0.01f, 0.0f, 1.0f))
	{
		fill = CAP(fill);
		SetNewPositions();
	}
	if (ImGui::DragFloat("##Drag Speed", &speed, 0.01f, 0.0f, 1.0f))
	{
		//Space for normalize if using number higher that 0 to 1
	}

	int selected_opt = current_transition_mode;
	ImGui::Text("Transition"); ImGui::SameLine((ImGui::GetWindowWidth() / 4) + 30);

	if (ImGui::Combo("##transition", &selected_opt, "Color tint transition\0Sprite transition\0"))
	{
		if (selected_opt == Transition::TRANSITION_COLOR)
			current_transition_mode = Transition::TRANSITION_COLOR;
		if (selected_opt == Transition::TRANSITION_SPRITE)
			current_transition_mode = Transition::TRANSITION_SPRITE;
	}

	switch (selected_opt)
	{
	case 0:
		ShowInspectorColorTransition();
		break;
	case 1:
		ShowInspectorSpriteTransition();
		break;
	default:
		break;
	}
	int navigation_opt = navigation.current_navigation_mode;
	ImGui::Text("Navigation"); ImGui::SameLine(op + 30);
	if (ImGui::Combo("##navegacion", &navigation_opt, "Desactive Navigation\0Navigation Extrict\0Navigation Automatic\0"))
	{
		if (navigation_opt == Navigation::NavigationMode::NAVIGATION_NONE)
			navigation.current_navigation_mode = Navigation::NavigationMode::NAVIGATION_NONE;
		if (navigation_opt == Navigation::NavigationMode::NAVIGATION_EXTRICTE)
			navigation.current_navigation_mode = Navigation::NavigationMode::NAVIGATION_EXTRICTE;
		if (navigation_opt == Navigation::NavigationMode::NAVIGATION_AUTOMATIC)
			navigation.current_navigation_mode = Navigation::NavigationMode::NAVIGATION_AUTOMATIC;

	}
	if (navigation.current_navigation_mode != Navigation::NavigationMode::NAVIGATION_NONE)
	{
		ShowNavigationInfo();
	}
	ImGui::TreePop();
}

void CompSlider::SetNewPositions()
{
	SetSlideBarPos();
	SetSlideBarBall();
}

void CompSlider::SetSlideBarPos()
{
	if (slide_bar == nullptr)
		return;
	slide_bar->FillAmount(fill);

}

void CompSlider::SetSlideBarBall()
{
	if (target_graphic == nullptr)
		return;

	float new_pos = Lerp(min_pos, max_pos, fill);
	target_graphic->GetRectTrasnform()->SetUIPos(float3(new_pos, 0.0f, 0.0f));
}

void CompSlider::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	CompInteractive::Save(object, name, saveScene, countResources);

	json_object_dotset_number_with_std(object, name + "Slide BG UUID", SaveSliderCompUID(slide_bg));
	json_object_dotset_number_with_std(object, name + "Slide Bar UUID", SaveSliderCompUID(slide_bar));
	json_object_dotset_number_with_std(object, name + "Slide Fill", fill);
	json_object_dotset_number_with_std(object, name + "Slide Speed", speed);

}

uint CompSlider::SaveSliderCompUID(CompImage * img) const
{
	return (img==nullptr)?0:img->GetUUID();
}

void CompSlider::Load(const JSON_Object * object, std::string name)
{
	CompInteractive::Load(object, name);

	uuid_reimported_slide_bg = json_object_dotget_number_with_std(object, name + "Slide BG UUID");
	uuid_reimported_slide_bar = json_object_dotget_number_with_std(object, name + "Slide Bar UUID");
	fill = json_object_dotget_number_with_std(object, name + "Slide Fill");
	speed = json_object_dotget_number_with_std(object, name + "Slide Speed");
	Enable();
}
void CompSlider::SyncComponent(GameObject* sync_parent)
{
	CompInteractive::SyncComponent(sync_parent);
	SyncSliderComponents(sync_parent);
}

void CompSlider::OnSubmit(Event event_data)
{
	on_drag = true;
	UpdateSelectionState(event_data);
	PrepareHandleTransition();
}

void CompSlider::OnCancel(Event event_data)
{
	on_drag = false;
	UpdateSelectionState(event_data);
	PrepareHandleTransition();
}

void CompSlider::OnMove(Event event_data)
{
	if (on_drag)
	{
		switch (event_data.gui_axis.direction)
		{
		case EGUIAxis::Direction::DIRECTION_RIGHT:
			fill += speed;
			fill = CAP(fill);
			SetNewPositions();
			break;
		case EGUIAxis::Direction::DIRECTION_LEFT:
			fill -= speed;
			fill = CAP(fill);
			SetNewPositions();
			break;
		default:
			break;
		}
	}
	else
	{
		CompInteractive::OnMove(event_data);
	}
}

void CompSlider::SetFillBar(float amount)
{
	fill = CAP(amount);
	SetNewPositions();
}

void CompSlider::SyncSliderComponents(GameObject* sync_parent)
{
	//checkbox para el compimage tick (+ save)

	if (sync_parent == nullptr)
		return;

		if (uuid_reimported_slide_bg != 0)
		{
			SetSliderBg((CompImage*)sync_parent->GetComponentsByUID(uuid_reimported_slide_bg, true));
		}
		if (uuid_reimported_slide_bar != 0)
		{

			SetSliderBar((CompImage*)sync_parent->GetComponentsByUID(uuid_reimported_slide_bar,true));
		}
		if (slide_bg != nullptr)
			slide_bg->SyncComponent(nullptr);
		if (slide_bar != nullptr)
			slide_bar->SyncComponent(nullptr);
		if (target_graphic != nullptr)
			target_graphic->SyncComponent(nullptr);

	SyncBar();
	SetNewPositions();

}

void CompSlider::SyncBar()
{
	min_pos = - slide_bar->GetRectTrasnform()->GetWidth() *0.5f;
	max_pos = + slide_bar->GetRectTrasnform()->GetWidth()*0.5f;
	slide_bar->SetToFilled(true);
}

void CompSlider::OnDrag(Event event_input)
{
	if (slide_bar == nullptr)
		return;
	if (point_down)
	{
		ImGuiIO& io = ImGui::GetIO();

		float4 rect = slide_bar->GetRectTrasnform()->GetGlobalRect();
		float mouse_x = 0.0f;
		if (!App->mode_game)
		{
			mouse_x = event_input.pointer.position.x - GetPositionDock("Scene").x;

			mouse_x = (mouse_x*io.DisplaySize.x) / GetSizeDock("Scene").x;

		}
		else
		{
			mouse_x = event_input.pointer.position.x;
			
		}
		if (mouse_x < rect.x)
			fill = 0.0f;
		else if (mouse_x > rect.x + rect.z)
			fill = 1.0f;
		else
		{
			float a = mouse_x - rect.x;
			float b = (rect.x + rect.z) - rect.x;
			fill = (a / b);
		}

		SetNewPositions();

	}
}

void CompSlider::SetSliderBg(CompImage * bg)
{
	if (bg == nullptr)
		return;
	slide_bg = bg;
	uuid_reimported_slide_bg = slide_bg->GetUUID();

}

void CompSlider::SetSliderBar(CompImage * bar)
{
	if (bar == nullptr)
		return;
	slide_bar = bar;

}

bool CompSlider::IsPressed()
{
	if (IsActivate())
		return false;
	if (point_down && point_inside)
		return true;
	else if (on_drag)
		return true;
	else
		return false;
}