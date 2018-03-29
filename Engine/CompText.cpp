#include "CompText.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "SDL2_ttf/include/SDL_ttf.h"
#include "ModuleImporter.h"
#include "ImportFont.h"
#include "CompRectTransform.h"
#include "CompCanvasRender.h"
#include "ModuleFS.h"
#include "ResourceFont.h"
#include "CompCanvas.h"

CompText::CompText(Comp_Type t, GameObject * parent) :CompGraphic(t, parent)
{
	uid = App->random->Int();
	name_component = "Text";
	glGenTextures(1, &id_font);


}

CompText::CompText(const CompText & copy, GameObject * parent) : CompGraphic(Comp_Type::C_TEXT, parent)
{
}

CompText::~CompText()
{
}

void CompText::PreUpdate(float dt)
{
	// Manage Resource -------------------------------------------------
	// Before delete Resource Set this pointer to nullptr
	if (text != nullptr)
	{
		if (text->GetState() == Resource::State::WANTDELETE)
		{
			text = nullptr;
		}
		else if (text->GetState() == Resource::State::REIMPORTED)
		{
		//	uuid_resource_reimported = resource_font->GetUUID();
			text = nullptr;
		}
	}
	else
	{
		
		if (uuid_source_font != 0)
		{
			text = (ResourceFont*)App->resource_manager->GetResource(uuid_source_font);
			if (text != nullptr)
			{
				text->num_game_objects_use_me++;
				// Check if loaded!
				if (text->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iFont->LoadResource(std::to_string(text->GetUUID()).c_str(), text_size, text);
				}
				uuid_source_font = 0;
			}
		}
		
	}
	// -------------------------------------------------------------------
}
void CompText::Update(float dt)
{

	if (transform != nullptr)
	{
		if (transform->GetUpdateRect() && my_canvas_render != nullptr)
		{
			GenerateText();
			transform->SetUpdateRect(false);
		}
	}

		render = can_draw;

}
void CompText::ShowOptions()
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
			CopyValues(((CompText*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompText::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsMesh");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsMesh"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}
	if (ImGui::Button("Select Font..."))
	{
		show_resource_font_windows = true;
	}
	ImGui::Text("Name: ");

	if (text != nullptr)
	{
		ImGui::SameLine();
		ImGui::Text(text->name.c_str());

	}
	const char* horizontal[] = { "Right##h_right", "Middle##h_middle", "Left##h_left" };
	const char* vertical[] = { "Top##v_right", "Middle##v_middle", "Bottom##v_bottom" };
	ImGui::Text("Horizontal Position");
	for (int i = 0; i < 3; i++)
	{

		if (ImGui::RadioButton(horizontal[i], &horizontal_position, i))
		{
			h_position = static_cast<HorizontalPosition>(horizontal_position);
			GenerateText();
		}
		if(i < 2)
			ImGui::SameLine();
	}
	ImGui::Text("Vertical Position");
	for (int i = 0; i < 3; i++)
	{

		if (ImGui::RadioButton(vertical[i], &vertical_position, i))
		{
			v_position = static_cast<VerticalPosition>(vertical_position);
			GenerateText();

		}
		if (i < 2)
			ImGui::SameLine();
	}
	
	if (ImGui::InputTextMultiline("Text", (char*)input_text.c_str(), max_input, ImVec2(ImGui::GetWindowWidth(),100), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CtrlEnterForNewLine))
	{
		SetString(input_text.c_str());
	}

	ImGui::ColorEdit4("Color##image_rgba", color.ptr());
	if (ImGui::DragInt("Text Size", &text_size, 1.0f, 0, 200))
	{
		if (text != nullptr)
		{
			text->SetSize(text_size);
			text->ReLoadToMemory();
			UpdateText();
			GenerateText();

		}
	}
	ImGui::DragInt("Input limit", &max_input, 1.0f, 0, 50);
	if (text == nullptr || show_resource_font_windows)
	{
		if (show_resource_font_windows)
		{
			ResourceFont* temp = (ResourceFont*)App->resource_manager->ShowResources(show_resource_font_windows, Resource::Type::FONT);
			if (temp != nullptr)
			{
				if (text != nullptr)
				{
					if (text->num_game_objects_use_me > 0)
					{
						text->num_game_objects_use_me--;
					}
				}
				text = temp;
				text->num_game_objects_use_me++;
				if (text->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iFont->LoadResource(std::to_string(text->GetUUID()).c_str(), text_size,text);


				}
				SetString("New Text");
				//SetTextureID(text->GetTextureID());
				Enable();
			}
		}
	}

	ImGui::TreePop();
}

void CompText::SetRect(float x, float y, float width, float height)
{
	text_rect.x = x;
	text_rect.y = y;
	text_rect.z = width;
	text_rect.w = height;
}



void CompText::SetString(std::string input)
{
	text_str = input;
	UpdateText();
	GenerateText();
}

bool CompText::GenerateText()
{
	if (text_str.empty())
		return false;
	float4 rect_transform = parent->GetComponentRectTransform()->GetRect();
	float width = parent->GetComponentRectTransform()->GetWidth();
	float height = parent->GetComponentRectTransform()->GetHeight();
	can_draw = false;
	if (TextCanFit(rect_transform, text_rect))
	{
		can_draw = true;
		std::vector<float3> quad_pos;
		float3 right_top=float3::zero;
		float3 left_top = float3::zero;
		float3 right_bottom = float3::zero;
		float3 left_bottom = float3::zero;

		switch (h_position)
		{
		case RIGHT_HPOSITION:
			left_top.x = rect_transform.x;
			right_top.x = rect_transform.x + text_rect.z;
			left_bottom.x = rect_transform.x;
			right_bottom.x = rect_transform.x + text_rect.z;
			break;
		case MIDDLE_HPOSITION:
			left_top.x = (rect_transform.x+ width*0.5)- text_rect.z*0.5;
			right_top.x = (rect_transform.x + width*0.5) + text_rect.z*0.5;
			left_bottom.x = (rect_transform.x + width*0.5) - text_rect.z*0.5;
			right_bottom.x = (rect_transform.x + width*0.5) + text_rect.z*0.5;


			break;
		case LEFT_HPOSITION:
			left_top.x = (rect_transform.x + width) - text_rect.z;
			right_top.x = rect_transform.x + width;
			left_bottom.x = (rect_transform.x + width) - text_rect.z;
			right_bottom.x = rect_transform.x + width;
			break;
		default:
			break;
		}
		switch (v_position)
		{
		case TOP_VPOSITION:
			right_top.y = rect_transform.y + height;
			left_top.y = rect_transform.y + height;
			right_bottom.y = (rect_transform.y + height) - text_rect.w;
			left_bottom.y = (rect_transform.y + height) - text_rect.w;
			break;
		case MIDDLE_VPOSITION:

			right_top.y = (rect_transform.y + height*0.5) + text_rect.w*0.5;
			left_top.y = (rect_transform.x + height*0.5) + text_rect.w*0.5;
			left_bottom.y = (rect_transform.x + height*0.5) - text_rect.w*0.5;
			right_bottom.y = (rect_transform.x + height*0.5) - text_rect.w*0.5;
			break;
		case BOTTOM_POSITION:
			right_top.y = rect_transform.y + text_rect.w;
			left_top.y = rect_transform.y + text_rect.w;
			right_bottom.y = rect_transform.y;
			left_bottom.y = rect_transform.y;
			break;
		default:
			break;
		}
		quad_pos.push_back(left_bottom);
		quad_pos.push_back(right_bottom);
		quad_pos.push_back(right_top);
		quad_pos.push_back(left_top);
		my_canvas_render->ProcessQuad(quad_pos);
		return true;

	}

	return false;


}

bool CompText::TextCanFit(float4 rect_transform, float4 rect_text)
{
	if (abs(rect_transform.x) + abs(rect_transform.z) > rect_text.z&&abs(rect_transform.y) + abs(rect_transform.w) > rect_text.w)
	{
		return true;
	}
	return false;
}

void CompText::UpdateText()
{
	if (text == nullptr)
		return;
	if (!text->font.font || text_str.empty())
		return;
	else if (s_font != NULL && text_str.empty())
	{
		FreeFont();
		return;
	}
	if (s_font != NULL)
	{
		FreeFont();
	}
	update_text = true;
	int width = 0;
	int height = 0;
	TTF_SizeText(text->font.font, text_str.c_str(), &width, &height);
	s_font = TTF_RenderText_Blended_Wrapped(text->font.font, text_str.c_str(), SDL_Color{ (Uint8)(color.x * 255), (Uint8)(color.y * 255),(Uint8)(color.z * 255), (Uint8)(color.w * 255) }, width);

	glBindTexture(GL_TEXTURE_2D, id_font);
	SetTextureID(id_font);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s_font->w, s_font->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, s_font->pixels);
	SetRect(0.0f, 0.0f, s_font->w, s_font->h);

	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(s_font);
}

void CompText::FreeFont()
{
	glDeleteTextures(1, &uid);
}

void CompText::CopyValues(const CompText * component)
{
}
void CompText::Clear()
{
	text = nullptr;
	if (my_canvas != nullptr)
		my_canvas->RemoveGraphic(this);
	my_canvas = nullptr;
	my_canvas_render = nullptr;
	transform = nullptr;



}
void CompText::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);

	if (text != nullptr)
	{
		if (saveScene == false)
		{
			// Save Info of Resource in Prefab (next we use this info for Reimport this prefab)
			std::string temp = std::to_string(countResources++);
			json_object_dotset_number_with_std(object, "Info.Resources.Resource " + temp + ".UUID Resource", text->GetUUID());
			json_object_dotset_string_with_std(object, "Info.Resources.Resource " + temp + ".Name", text->name.c_str());
		}
		json_object_dotset_number_with_std(object, name + "Resource Font UUID", text->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Resource Font UUID", 0);
	}

	json_object_dotset_boolean_with_std(object, name + "RayCast Target", raycast_target);
	json_object_dotset_string_with_std(object, name + "Text", text_str.c_str());
	json_object_dotset_number_with_std(object, name + "Max Input", max_input);
	json_object_dotset_number_with_std(object, name + "Text Size", text_size);

	json_object_dotset_number_with_std(object, name + "HPosition", h_position);
	json_object_dotset_number_with_std(object, name + "VPosition", v_position);
	json_object_dotset_boolean_with_std(object, name + "Can Draw", can_draw);

	App->fs->json_array_dotset_float4(object, name + "Text Color", color);
}

void CompText::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");

	raycast_target=json_object_dotget_boolean_with_std(object, name + "RayCast Target");
	text_str=json_object_dotget_string_with_std(object, name + "Text");
	max_input=json_object_dotget_number_with_std(object, name + "Max Input");
	text_size=json_object_dotget_number_with_std(object, name + "Text Size" );
				   
	h_position= static_cast<CompText::HorizontalPosition>((int)json_object_dotget_number_with_std(object, name + "HPosition" ));
	v_position= static_cast<CompText::VerticalPosition>((int)json_object_dotget_number_with_std(object, name + "VPosition" ));
	can_draw=json_object_dotget_boolean_with_std(object, name + "Can Draw");

	color=App->fs->json_array_dotget_float4_string(object, name + "Text Color");

	uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Font UUID");
	if (resourceID > 0)
	{
		text = (ResourceFont*)App->resource_manager->GetResource(resourceID);
		if (text != nullptr)
		{
			text->num_game_objects_use_me++;

			// LOAD Image ----------------------------
			if (text->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iFont->LoadResource(std::to_string(text->GetUUID()).c_str(), text_size, text);
			}
		

		}
	}
	

	Enable();
}
void CompText::SyncComponent(GameObject* sync_parent)
{
	AddRectTransform();
	AddCanvasRender();
	AddCanvas();
	if (my_canvas_render != nullptr&&my_canvas != nullptr)
	{
		UpdateText();
		GenerateText();
	}
}