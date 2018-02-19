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
#include <vector>

CompImage::CompImage(Comp_Type t, GameObject * parent) :CompGraphic(t, parent)
{
	uid = App->random->Int();
	name_component = "Image";

}

CompImage::CompImage(const CompImage & copy, GameObject * parent) :CompGraphic(Comp_Type::C_IMAGE, parent)
{
}

CompImage::~CompImage()
{
}

void CompImage::PreUpdate(float dt)
{
	// Manage Resource -------------------------------------------------
	// Before delete Resource Set this pointer to nullptr
	if (source_image != nullptr)
	{
		if (uuid_source_image != 0)
		{
			source_image = (ResourceMaterial*)App->resource_manager->GetResource(uuid_source_image);
			if (source_image != nullptr)
			{
				source_image->num_game_objects_use_me++;
				// Check if loaded!
				if (source_image->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(source_image->GetUUID()).c_str(), source_image);
				}
				uuid_source_image = 0;
			}
		}
	}

	if (source_image != nullptr)
	{
		if (source_image->GetState() == Resource::State::WANTDELETE)
		{
			source_image = nullptr;
		}
		else if (source_image->GetState() == Resource::State::REIMPORTED)
		{
			uuid_source_image = source_image->GetUUID();
			source_image = nullptr;
		}
	}

	// -------------------------------------------------------------------
}



void CompImage::Update(float dt)
{

	if (transform != nullptr)
	{
		if (transform->GetUpdateRect() && my_canvas_render!=nullptr)
		{
			my_canvas_render->ProcessImage(this);
			transform->SetUpdateRect(false);
		}
	}

}
void CompImage::ShowOptions()
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
			CopyValues(((CompImage*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompImage::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsImage");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsImage"))
	{
		ShowOptions();	
		ImGui::EndPopup();
	}
	if (source_image != nullptr)
	{
		ImGui::Image((ImTextureID*)source_image->GetTextureID(), ImVec2(170, 170), ImVec2(-1, 1), ImVec2(0, 0));

	}		
	
	if (ImGui::Button("Select Sprite..."))
	{
		select_source_image = true;
	}
	ImGui::ColorEdit4("Color##image_rgba", color.ptr());
	ImGui::Checkbox("RayCast Target", &raycast_target);
	if (source_image == nullptr || select_source_image)
	{
		if (select_source_image)
		{
			ResourceMaterial* temp = (ResourceMaterial*)App->resource_manager->ShowResources(select_source_image, Resource::Type::MATERIAL);
			if (temp != nullptr)
			{
				if (source_image != nullptr)
				{
					if (source_image->num_game_objects_use_me > 0)
					{
						source_image->num_game_objects_use_me--;
					}
				}
				source_image = temp;
				source_image->num_game_objects_use_me++;
				if (source_image->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(source_image->GetUUID()).c_str(), source_image);
					

				}
				SetTextureID(source_image->GetTextureID());
				Enable();
			}
		}
	}
	int selection = type;
	ImGui::Text("Image Type:"); ImGui::SameLine();
	if (ImGui::Combo("##type", &selection, "SIMPLE\0FILLED", 2))
	{
		if (selection == Type::SIMPLE)
		{
			filler = false;
			type = Type::SIMPLE;
		}
		if (selection == Type::FILLED)
		{
			filler = true;
			type = Type::FILLED;
		}
	}
	selection = HORITZONTAL;
	if (type == Type::FILLED)
	{
		ImGui::Text("Fill Method:"); ImGui::SameLine();
		if (ImGui::Combo("##fillMethod", &selection, "None\0Horitzontal\0Vertical\0 Radial",3))
		{
			if (selection == FillMethod::HORITZONTAL)
				method = FillMethod::HORITZONTAL;
			if (selection == FillMethod::VERTICAL)
				method = FillMethod::VERTICAL;
			if (selection == FillMethod::RADIAL360)
				method = FillMethod::RADIAL360;
		}
		if (ImGui::DragFloat("##fillQuantity", &filled, 0.01f, 0.0f, 1.0f))
		{
			GenerateFilledSprite(filled, method);
		}
	}
	ImGui::TreePop();
}

void CompImage::GenerateFilledSprite(float to_fill, FillMethod Method)
{
	float4 v = parent->GetComponentRectTransform()->GetRect();
	float4 outer = { 0.0f,0.0f,1.0f,1.0f };	
	std::vector<float2> quad_pos;
	std::vector<float2> quad_uv;
	
	float tx0 = outer.x;
	float ty0 = outer.y;
	float tx1 = outer.z;
	float ty1 = outer.w;

	if (Method == HORITZONTAL || Method == VERTICAL)
	{
		if (Method == HORITZONTAL)
		{
			float fill = (tx1 - tx0) * to_fill;

			v.z = v.x + (v.z - v.x) * fill;
			tx1 = tx0 + to_fill;
		}
		else if (Method == VERTICAL)
		{
			float fill = (ty1 - ty0) * to_fill;

			v.w = v.y + (v.w - v.y) * fill;
			ty1 = ty0 + to_fill;
		}
	}
	
	quad_pos.push_back(float2({ v.x,v.y }));
	quad_pos.push_back(float2({ v.x,v.w }));
	quad_pos.push_back(float2({ v.z,v.w,}));
	quad_pos.push_back(float2({ v.z,v.y,}));

	quad_uv.push_back(float2({ tx0,ty0 }));
	quad_uv.push_back(float2({ tx0,ty1,}));
	quad_uv.push_back(float2({ tx1,ty1,}));
	quad_uv.push_back(float2({ tx1,ty0,}));

	my_canvas_render->ProcessQuad(quad_pos, quad_uv);
}

void CompImage::CopyValues(const CompImage * component)
{
}

void CompImage::Clear()
{
	source_image = nullptr;
	overwrite_image = nullptr;
}

void CompImage::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);

	if (source_image != nullptr)
	{
		if (saveScene == false)
		{
			// Save Info of Resource in Prefab (next we use this info for Reimport this prefab)
			std::string temp = std::to_string(countResources++);
			json_object_dotset_number_with_std(object, "Info.Resources.Resource " + temp + ".UUID Resource", source_image->GetUUID());
			json_object_dotset_string_with_std(object, "Info.Resources.Resource " + temp + ".Name", source_image->name);
		}
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", source_image->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", 0);
	}
}

void CompImage::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	//...
	uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Mesh UUID");
	if (resourceID > 0)
	{
		source_image = (ResourceMaterial*)App->resource_manager->GetResource(resourceID);
		if (source_image != nullptr)
		{
			source_image->num_game_objects_use_me++;

			// LOAD MESH ----------------------------
			if (source_image->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMaterial->LoadResource(std::to_string(source_image->GetUUID()).c_str(), source_image);
				SetTextureID(source_image->GetTextureID());
			}

		}
	}
	Enable();
}

void CompImage::UpdatesPriteId()
{
	if (overwrite_image == nullptr)
	{
		if (source_image == nullptr)
		{
			// return default texture;
			SetTextureID(0);
			return;
		}
		SetTextureID(source_image->GetTextureID());
		return;

	}
	SetTextureID(overwrite_image->GetTextureID());
}

void CompImage::SetSourceImage(ResourceMaterial * set_source_image)
{
	source_image = set_source_image;
}

void CompImage::SetColor(const float4 & set_rgba)
{
	color = set_rgba;
}

void CompImage::SetColor(float set_r, float set_g, float set_b, float set_a)
{
	SetColor(float4(set_r,set_g,set_b,set_a));
}

void CompImage::SetTextureID(uint uid)
{
	texture_id = uid;
}

void CompImage::SetOverwriteImage(ResourceMaterial * set_overwrite_image)
{
	overwrite_image = set_overwrite_image;
}

float4 CompImage::GetColor() const
{
	return color;
}

ResourceMaterial * CompImage::GetSourceImage() const
{
	return source_image;
}

ResourceMaterial * CompImage::GetCurrentTexture() const
{
	if (overwrite_image == nullptr)
	{
		if (source_image == nullptr)
		{
			// return default texture;
		}
		return source_image;

	}
	return overwrite_image;
}

