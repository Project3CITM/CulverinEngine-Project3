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
#include "ModuleInput.h"

CompImage::CompImage(Comp_Type t, GameObject * parent) :CompGraphic(t, parent)
{
	uid = App->random->Int();
	name_component = "CompImage";

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
			GenerateMesh();
			transform->SetUpdateRect(false);
		}
	}
	if (device_swap)
	{
		if (App->input->GetUpdateNewDevice())
		{
			DeviceCheck();
			
		}
	}

	render = CheckRender();

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
		if (!select_controller_source_image)
			select_source_image = true;
	}
	ImGui::Checkbox("Sprite Swap(Controller)", &device_swap);
	if (device_swap)
	{
		if (controller_image != nullptr)
		{
		ImGui::Image((ImTextureID*)controller_image->GetTextureID(), ImVec2(170, 170), ImVec2(-1, 1), ImVec2(0, 0));

		}
	
		if (ImGui::Button("Select Controller Sprite..."))
		{
			if(!select_source_image)
				select_controller_source_image = true;
		}
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
	if (controller_image == nullptr || select_controller_source_image)
	{
		if (select_controller_source_image)
		{
			ResourceMaterial* temp = (ResourceMaterial*)App->resource_manager->ShowResources(select_controller_source_image, Resource::Type::MATERIAL);
			if (temp != nullptr)
			{
				if (controller_image != nullptr)
				{
					if (controller_image->num_game_objects_use_me > 0)
					{
						controller_image->num_game_objects_use_me--;
					}
				}
				controller_image = temp;
				controller_image->num_game_objects_use_me++;
				if (controller_image->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(controller_image->GetUUID()).c_str(), controller_image);


				}
				SetTextureID(controller_image->GetTextureID());
				Enable();
			}
		}
		
	}
	int selection = type;
	ImGui::Text("Image Type:"); ImGui::SameLine();
	if (ImGui::Combo("##type", &selection, "SIMPLE\0FILLED"))
	{
		if (selection == Type::SIMPLE)
		{
			type = Type::SIMPLE;
		}
		if (selection == Type::FILLED)
		{
			type = Type::FILLED;
		}
	}
	selection = method;
	if (type == Type::FILLED)
	{
		ImGui::Text("Fill Method:"); ImGui::SameLine();
		if (ImGui::Combo("##fillMethod", &selection, "Horitzontal\0Vertical\0Radial"))
		{
			method = (FillMethod)selection;
			
			if (selection == FillMethod::HORITZONTAL)
				method = FillMethod::HORITZONTAL;
			if (selection == FillMethod::VERTICAL)
				method = FillMethod::VERTICAL;
			if (selection == FillMethod::RADIAL360)
				method = FillMethod::RADIAL360;
			
		}
		ShowMethodInfo();
		if (ImGui::DragFloat("##fillQuantity", &filled, 0.01f, 0.0f, 1.0f))
		{
			CorrectFillAmount();
			GenerateMesh();
		}
	}
	ImGui::TreePop();
}

void CompImage::ShowMethodInfo()
{
	if (method == FillMethod::RADIAL360)
	{
		ImGui::Checkbox("Invert Radial##radial invert", &radial_inverse);
	}
}



void CompImage::FillAmount(float value)
{
	if (value < 0.0f)
	{
		filled = 0.0f;
	}
	else if (value > 1.0f)
	{
		filled = 1.0f;
	}
	filled = value;

	GenerateMesh();
}
void CompImage::DeviceCheck()
{
	if (DeviceCombinationType::KEYBOARD_AND_MOUSE_COMB_DEVICE == App->input->GetActualDeviceCombo())
	{
		device_swap_active = false;
	}
	else if (DeviceCombinationType::CONTROLLER_COMB_DEVICE == App->input->GetActualDeviceCombo())
	{
		device_swap_active = true;
	}
	UpdateSpriteId();
}
void CompImage::GenerateFilledSprite()
{
	if (filled < 0.001f)
		return;

	if (filled > 1.0f)
		filled = 1.0f;
	float4 vertex = parent->GetComponentRectTransform()->GetRect();
	float4 outer = { 0.0f,0.0f,1.0f,1.0f };	
	std::vector<float3> quad_pos;
	quad_pos.reserve(4);
	quad_pos.push_back(float3::zero);
	quad_pos.push_back(float3::zero);
	quad_pos.push_back(float3::zero);
	quad_pos.push_back(float3::zero);

	std::vector<float3> quad_uv;
	quad_uv.reserve(4);
	quad_uv.push_back(float3::zero);
	quad_uv.push_back(float3::zero);
	quad_uv.push_back(float3::zero);
	quad_uv.push_back(float3::zero);

	float uv_x0 = outer.x;
	float uv_y0 = outer.y;
	float uv_x1 = outer.z;
	float uv_y1 = outer.w;

	if (method == FillMethod::HORITZONTAL || method == FillMethod::VERTICAL)
	{
		if (method == FillMethod::HORITZONTAL)
		{
			float fill = (uv_x1 - uv_x0) * filled;

			vertex.z = vertex.x + (vertex.z - vertex.x) * filled;
			uv_x1 = uv_x0 + fill;
		}
		else if (method == FillMethod::VERTICAL)
		{
			float fill = (uv_y1 - uv_y0) * filled;

			vertex.w = vertex.y + (vertex.w - vertex.y) * filled;
			uv_y1 = uv_y0 + fill;
		}
	}
	quad_pos[0]=(float3({ vertex.x,vertex.y ,0}));
	quad_pos[1]=(float3({ vertex.x,vertex.w ,0}));
	quad_pos[2]=(float3({ vertex.z,vertex.w,0 }));
	quad_pos[3]=(float3({ vertex.z,vertex.y,0 }));

	quad_uv[0] = float3({ uv_x0,uv_y0 ,0});
	quad_uv[1] = float3({ uv_x0,uv_y1,0 });
	quad_uv[2] = float3({ uv_x1,uv_y1,0 });
	quad_uv[3] = float3({ uv_x1,uv_y0,0 });

	if (filled < 1.0f && method != FillMethod::HORITZONTAL && method != FillMethod::VERTICAL)
	{
		if (method == FillMethod::RADIAL360)
		{
			for (int box_corner = 0; box_corner < 4; box_corner++)
			{
				float pos_x0, pos_x1, pos_y0, pos_y1;
				if (box_corner < 2)
				{
					pos_x0 = 0.0f;
					pos_x1 = 0.5f;
				}
				else
				{
					pos_x0 = 0.5f;
					pos_x1 = 1.0f;
				}

				if (box_corner == 0 || box_corner == 3)
				{
					pos_y0 = 0.0f;
					pos_y1 = 0.5f;
				}
				else
				{
					pos_y0 = 0.5f;
					pos_y1 = 1.0f;
				}

				quad_pos[0].x = Lerp(vertex.x, vertex.z, pos_x0);
				quad_pos[1].x = quad_pos[0].x;
				quad_pos[2].x = Lerp(vertex.x, vertex.z, pos_x1);
				quad_pos[3].x = quad_pos[2].x;

				quad_pos[0].y = Lerp(vertex.y, vertex.w, pos_y0);
				quad_pos[1].y = Lerp(vertex.y, vertex.w, pos_y1);
				quad_pos[2].y = quad_pos[1].y;
				quad_pos[3].y = quad_pos[0].y;


				quad_uv[0].x = Lerp(uv_x0, uv_x1, pos_x0);
				quad_uv[1].x = quad_uv[0].x;
				quad_uv[2].x = Lerp(uv_x0, uv_x1, pos_x1);
				quad_uv[3].x = quad_uv[2].x;

				quad_uv[0].y = Lerp(uv_y0, uv_y1, pos_y0);
				quad_uv[1].y = Lerp(uv_y0, uv_y1, pos_y1);
				quad_uv[2].y = quad_uv[1].y;		
				quad_uv[3].y = quad_uv[0].y;

				float value = (radial_inverse)
					?filled*4.0f - (box_corner % 4)
					:filled*4.0f - (3-((box_corner) % 4));


				if (RadialCut(quad_pos, quad_uv, CorrectValue01(value), ((box_corner + 2) % 4), radial_inverse))
				{
					ProcesQuad(quad_pos, quad_uv);
					//my_canvas_render->ProcessQuad(quad_pos, quad_uv);
				}
			}
		}
	}
	else
	{
		ProcesQuad(quad_pos, quad_uv);
	}
}

void CompImage::CopyValues(const CompImage * component)
{
}

void CompImage::Clear()
{
	source_image = nullptr;
	overwrite_image = nullptr;
	if (my_canvas != nullptr)
		my_canvas->RemoveGraphic(this);
	my_canvas = nullptr;
	my_canvas_render = nullptr;
	transform = nullptr;


	
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
			json_object_dotset_string_with_std(object, "Info.Resources.Resource " + temp + ".Name", source_image->name.c_str());
		}
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", source_image->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", 0);
	}
	json_object_dotset_boolean_with_std(object, name + "Device Swap", device_swap);
	json_object_dotset_boolean_with_std(object, name + "Device Swap Active", device_swap_active);
	json_object_dotset_boolean_with_std(object, name + "Invalid", invalid);
	json_object_dotset_boolean_with_std(object, name + "Can draw", can_draw);

	if (device_swap)
	{
		if (controller_image != nullptr)
		{
			if (saveScene == false)
			{
				// Save Info of Resource in Prefab (next we use this info for Reimport this prefab)
				std::string temp = std::to_string(countResources++);
				json_object_dotset_number_with_std(object, "Info.Resources.ControllerResource " + temp + ".UUID Resource", controller_image->GetUUID());
				json_object_dotset_string_with_std(object, "Info.Resources.ControllerResource " + temp + ".Name", controller_image->name.c_str());
			}
			json_object_dotset_number_with_std(object, name + "Resource Controller UUID", controller_image->GetUUID());
		}
		else
		{
			json_object_dotset_number_with_std(object, name + "Resource Controller UUID", 0);
		}
	}
	json_object_dotset_boolean_with_std(object, name + "RayCast Target", raycast_target);
	json_object_dotset_number_with_std(object, name + "Fill Amount", filled);
	json_object_dotset_number_with_std(object, name + "Image Type", type);
	json_object_dotset_number_with_std(object, name + "Fill Method", method);
	if (method == FillMethod::RADIAL360)
	{
		json_object_dotset_boolean_with_std(object, name + "Fill Radian Inverse", radial_inverse);
	}
	App->fs->json_array_dotset_float4(object, name + "Image Color", color);

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

			// LOAD Image ----------------------------
			if (source_image->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMaterial->LoadResource(std::to_string(source_image->GetUUID()).c_str(), source_image);
			}
			UpdateSpriteId();
		}
	}

	device_swap = json_object_dotget_boolean_with_std(object, name + "Device Swap");
	device_swap_active= json_object_dotget_boolean_with_std(object, name + "Device Swap Active");

	invalid = json_object_dotget_boolean_with_std(object, name + "Invalid");
	can_draw = json_object_dotget_boolean_with_std(object, name + "Can draw");
	if (device_swap)
	{
		uint resource_controllerID = json_object_dotget_number_with_std(object, name + "Resource Controller UUID");
		if (resource_controllerID > 0)
		{
			controller_image = (ResourceMaterial*)App->resource_manager->GetResource(resourceID);
			if (controller_image != nullptr)
			{
				controller_image->num_game_objects_use_me++;

				// LOAD Image ----------------------------
				if (controller_image->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(controller_image->GetUUID()).c_str(), controller_image);
				}
				UpdateSpriteId();
			}
		}
	}
	raycast_target=json_object_dotget_boolean_with_std(object, name + "RayCast Target");
	filled=json_object_dotget_number_with_std(object, name + "Fill Amount");
	type = static_cast<CompImage::Type>((int)json_object_dotget_number_with_std(object, name + "Image Type"));
	method = static_cast<FillMethod>((int)json_object_dotget_number_with_std(object, name + "Fill Method"));
	if (method == FillMethod::RADIAL360)
	{
		radial_inverse = json_object_dotget_boolean_with_std(object, name + "Fill Radian Inverse");
	}
	color = App->fs->json_array_dotget_float4_string(object, name + "Image Color");

	Enable();
}

void CompImage::UpdateSpriteId()
{
	if (overwrite_image == nullptr)
	{
		
		if (source_image == nullptr)
		{
			// return default texture;
			SetTextureID(my_canvas->GetDefaultTexture());
			return;
		}
		if (device_swap_active)
		{
			if (controller_image != nullptr)
			{
			// return default texture;
				SetTextureID(controller_image->GetTextureID());
				return;
			}
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
	if (texture_id == uid)
		return;
	texture_id = uid;
}

void CompImage::SetOverwriteImage(ResourceMaterial * set_overwrite_image)
{
	overwrite_image = set_overwrite_image;
}

void CompImage::SetToFilled(bool filled)
{
	if (filled)
	{
		type = Type::FILLED;
	}
	else 
	{
		type = Type::SIMPLE;
	}

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

void CompImage::CorrectFillAmount()
{
	if (filled < 0.0f)
	{
		filled = 0.0f;
	}
	else if (filled > 1.0f)
	{
		filled = 1.0f;
	}
}

float CompImage::CorrectValue01(float value)
{
	if (value > 1.0f)
		value = 1.0f;
	else if (value < 0.0f)
		value = 0.0f;

	return value;
}




bool CompImage::RadialCut(std::vector<float3>& position, std::vector<float3>& texture_cord, float fill_value, int box_corner,bool invert)
{
	if (fill_value < 0.001f)
	{

		return false;

	}

	if ((box_corner & 1) == 1) 
		invert = !invert;

	if (!invert && fill_value > 0.999f)
	{

		return true;

	}

	
	float angle = CorrectValue01(fill_value);
	
	if (invert)
		angle = 1.0f - angle;
	angle *= DegToRad(90.0f);

	float cos = Cos(angle);
	float sin = Sin(angle);

	RadialCut(position, cos, sin, box_corner, invert);
	RadialCut(texture_cord, cos, sin, box_corner, invert);
	return true;
}

void CompImage::RadialCut(std::vector<float3>& modify, float cos, float sin, int box_corner, bool invert)
{
	int pos0 = box_corner;
	int pos1 = ((box_corner + 1) % 4);
	int pos2 = ((box_corner + 2) % 4);
	int pos3 = ((box_corner + 3) % 4);

	if ((box_corner & 1) == 1)
	{
		if (sin > cos)
		{
			cos /= sin;
			sin = 1.0f;

			if (invert)
			{
				modify[pos1].x = Lerp(modify[pos0].x, modify[pos2].x, cos);
				modify[pos2].x = modify[pos1].x;
			}
		}
		else if (cos > sin)
		{
			sin /= cos;
			cos = 1.0f;

			if (!invert)
			{
				modify[pos2].y = Lerp(modify[pos0].y, modify[pos2].y, sin);
				modify[pos3].y = modify[pos2].y;
			}
		}
		else
		{
			cos = 1.0f;
			sin = 1.0f;
		}

		if (!invert)
			modify[pos3].x = Lerp(modify[pos0].x, modify[pos2].x, cos);
		else
			modify[pos1].y = Lerp(modify[pos0].y, modify[pos2].y, sin);
	}
	else
	{
		if (cos > sin)
		{
			sin /= cos;
			cos = 1.0f;

			if (!invert)
			{
				modify[pos1].y = Lerp(modify[pos0].y, modify[pos2].y, sin);
				modify[pos2].y = modify[pos1].y;
			}
		}
		else if (sin > cos)
		{
			cos /= sin;
			sin = 1.0f;

			if (invert)
			{
				modify[pos2].x = Lerp(modify[pos0].x, modify[pos2].x, cos);
				modify[pos3].x = modify[pos2].x;
			}
		}
		else
		{
			cos = 1.0f;
			sin = 1.0f;
		}

		if (invert) 
			modify[pos3].y = Lerp(modify[pos0].y, modify[pos2].y, sin);
		else 
			modify[pos1].x = Lerp(modify[pos0].x, modify[pos2].x, cos);
	}
	
}

void CompImage::ExpandMesh()
{
	switch (type)
	{
	case Type::SIMPLE:
	{
		ProcesQuad(transform->GenerateQuadVertices());
	}
		break;
	case Type::FILLED:
		GenerateFilledSprite();

		break;

	}
}

