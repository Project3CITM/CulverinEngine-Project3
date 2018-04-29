#include "CompLight.h"
#include"Application.h"
#include"Scene.h"
#include"ModuleResourceManager.h"
#include"ResourceMesh.h"
#include"ModuleRenderer3D.h"
#include"GameObject.h"
#include"CompTransform.h"
#include"CompCamera.h"
#include"WindowInspector.h"
#include"ModuleGUI.h"
#include"Math\Quat.h"
#include"CompTransform.h"
#include"CompCamera.h"
#include"ModuleFS.h"
#include "ModuleLightning.h"
#include "ResourceMesh.h"
#include "JSONSerialization.h"

CompLight::CompLight(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	color = float4(255, 255, 255, 255);
	type = NO_LIGHT_TYPE;

	ambientCoefficient = 0.2;
	radius = 50.0f;
	

	properties[0] = 10;
	properties[1] = 1.0f;
	properties[2] = 1.0f;
	properties[3] = 1.0f;
	name_component = "CompLight";


	types_lights += "Point Light";
	types_lights += '\0';
	types_lights += "Directional Light";
	types_lights += '\0';


	color_temp[0] = 1;
	color_temp[1] = 1;
	color_temp[2] = 1;
	color_temp[3] = 1;
	





	//-Frustrum Creation----------------
	float width = 0.0f;
	float height = 0.0f;
	float aspect_ratio = 0.0f;
	float near_plane = 0.0f;
	float far_plane = 0.0f;
	float vertical_fov = 0.0f;

	width = 16;
	height = 9;
	aspect_ratio = width / height; // We set aspect ratio 16:9 by now

	near_plane = 0.2;
	far_plane = 1000;
	vertical_fov = 60; /* In degrees */

					   /* Set frustum vars */
	frustum.type = PerspectiveFrustum;
	frustum.pos = parent->GetComponentTransform()->GetPosGlobal();
	frustum.front.Set(0, -1, 0);
	frustum.up.Set(0, 0, 1);
	frustum.nearPlaneDistance = near_plane;
	frustum.farPlaneDistance = far_plane;
	frustum.verticalFov = vertical_fov * DEGTORAD;
	frustum.horizontalFov = Atan(aspect_ratio*Tan(frustum.verticalFov / 2)) * 2;

	//---------------------------------------------

	App->module_lightning->OnLightCreated(this);
	parent->box_fixed.SetFromCenterAndSize(GetGameObjectPos(), float3(radius, radius, radius));
}

CompLight::CompLight(const CompLight & copy, GameObject * parent) : Component(Comp_Type::C_LIGHT, parent)
{
	//App->module_lightning->OnLightDestroyed(this); //TODO/CHECK: Why delete the light?? Should add it to the list. Why create another method, already one to erase a light from the list on module lighting.
	this->ambientCoefficient = copy.ambientCoefficient;
	this->radius = copy.radius;
	this->color = copy.color;
	this->type = copy.type;
	this->types_lights = copy.types_lights;
	this->ui_light_type = copy.ui_light_type;



	this->properties = copy.properties;




	//-Frustrum Creation----------------
	float width = 0.0f;
	float height = 0.0f;
	float aspect_ratio = 0.0f;
	float near_plane = 0.0f;
	float far_plane = 0.0f;
	float vertical_fov = 0.0f;

	width = 16;
	height = 9;
	aspect_ratio = width / height; // We set aspect ratio 16:9 by now

	near_plane = 0.2;
	far_plane = 1000;
	vertical_fov = 60; /* In degrees */

					   /* Set frustum vars */
	frustum.type = PerspectiveFrustum;
	frustum.pos = parent->GetComponentTransform()->GetPosGlobal();
	frustum.front.Set(0, -1, 0);
	frustum.up.Set(0, 0, 1);
	frustum.nearPlaneDistance = near_plane;
	frustum.farPlaneDistance = far_plane;
	frustum.verticalFov = vertical_fov * DEGTORAD;
	frustum.horizontalFov = Atan(aspect_ratio*Tan(frustum.verticalFov / 2)) * 2;
	
	App->module_lightning->OnLightCreated(this);
	parent->box_fixed.SetFromCenterAndSize(GetGameObjectPos(), float3(radius, radius, radius));
}

CompLight::~CompLight()
{
	App->module_lightning->OnLightDestroyed(this);
}

bool CompLight::Enable()
{
	return false;
}

bool CompLight::Disable()
{
	return false;
}

void CompLight::Init()
{
}

void CompLight::PreUpdate(float dt)
{
	float3 box_pos = parent->box_fixed.CenterPoint();
	float3 parent_pos = GetGameObjectPos();
	float epsilon = 0.4f;
	if(abs(box_pos.x - parent_pos.x) > epsilon || abs(box_pos.y - parent_pos.y) > epsilon || abs(box_pos.z - parent_pos.z) > epsilon)
		parent->box_fixed.SetFromCenterAndSize(GetGameObjectPos(), float3(radius, radius, radius));
	
}

void CompLight::Update(float dt)
{
	use_light_to_render = false;

}

void CompLight::Draw()
{

	
}

void CompLight::Clear()
{
}

void CompLight::ShowOptions()
{
	if (ImGui::MenuItem("Reset", NULL, false, false))
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

}

void CompLight::ShowInspectorInfo()
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


	ImGui::ColorPicker4("Light Color", color_temp);
	color.x = color_temp[0];
	color.y = color_temp[1];
	color.z = color_temp[2];
	color.w = color_temp[3];

	ImGui::DragFloat("Intensity", &properties[0]);
	ImGui::DragFloat("Ambient Coefficient", &ambientCoefficient);
	if (ImGui::DragFloat("Radius", &radius, 1.0f, 0.0f, 500.0f)) 
	{	
			parent->box_fixed.SetFromCenterAndSize(GetGameObjectPos(), float3(radius, radius, radius));
	}
	ImGui::DragFloat("Constant", &properties[1]);
	ImGui::DragFloat("Linear", &properties[2]);
	ImGui::DragFloat("Quadratic", &properties[3]);

	ImGui::Combo("Light Type", &ui_light_type, types_lights.c_str());
	type = (Light_type)ui_light_type;

	
	ImGui::TreePop();

}

void CompLight::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_number_with_std(object, name + "UUID", GetUUID());
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	App->fs->json_array_dotset_float4(object, name + "Color", color);

	json_object_dotset_number_with_std(object, name + "Light Type", (int)type);
	json_object_dotset_number_with_std(object, name + "Intensity", properties[0]);
	json_object_dotset_number_with_std(object, name + "Ambient Coefficient", ambientCoefficient);
	json_object_dotset_number_with_std(object, name + "Radius", radius);
	json_object_dotset_number_with_std(object, name + "Constant", properties[1]);
	json_object_dotset_number_with_std(object, name + "Linear", properties[2]);
	json_object_dotset_number_with_std(object, name + "Quadratic", properties[3]);

	
}

void CompLight::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	ui_light_type =json_object_dotget_number_with_std(object, name + "Light Type");
	type = (Light_type)ui_light_type;
	ambientCoefficient = json_object_dotget_number_with_std(object, name + "Ambient Coefficient");
	radius = 50;// json_object_dotget_number_with_std(object, name + "Radius");
	properties[0] = json_object_dotget_number_with_std(object, name + "Intensity");
	properties[1] = json_object_dotget_number_with_std(object, name + "Constant");
	properties[2] = json_object_dotget_number_with_std(object, name + "Linear");
	properties[3] = json_object_dotget_number_with_std(object, name + "Quadratic");
	color = App->fs->json_array_dotget_float4_string(object, name + "Color");
	color_temp[0] = color.x;	color_temp[1] = color.y;	color_temp[2] = color.z;	color_temp[3] = color.w;

	parent->box_fixed.SetFromCenterAndSize(GetGameObjectPos(), float3(radius, radius, radius));
}

void CompLight::GetOwnBufferSize(uint& buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(int);				//UID
	buffer_size += sizeof(float);			//color.x
	buffer_size += sizeof(float);			//color.y
	buffer_size += sizeof(float);			//color.z
	buffer_size += sizeof(float);			//color.w


	buffer_size += sizeof(int);				//(int)type
	buffer_size += sizeof(float);			//properties[0]
	buffer_size += sizeof(float);			//ambientCoefficient
	buffer_size += sizeof(float);			//radius
	buffer_size += sizeof(float);			//properties[1]
	buffer_size += sizeof(float);			//properties[2]
	buffer_size += sizeof(float);			//properties[3]
}

void CompLight::SaveBinary(char** cursor, int position) const
{
	Component::SaveBinary(cursor, position);
	App->json_seria->SaveIntBinary(cursor, GetUUID());
	App->json_seria->SaveFloat4Binary(cursor, color);

	App->json_seria->SaveIntBinary(cursor, (int)type);

	App->json_seria->SaveFloatBinary(cursor, properties[0]);
	App->json_seria->SaveFloatBinary(cursor, ambientCoefficient);
	App->json_seria->SaveFloatBinary(cursor, radius);
	App->json_seria->SaveFloatBinary(cursor, properties[1]);
	App->json_seria->SaveFloatBinary(cursor, properties[2]);
	App->json_seria->SaveFloatBinary(cursor, properties[3]);
}

void CompLight::LoadBinary(char** cursor)
{
	uid = App->json_seria->LoadIntBinary(cursor);
	color = App->json_seria->LoadFloat4Binary(cursor);
	ui_light_type = App->json_seria->LoadIntBinary(cursor);
	type = (Light_type)ui_light_type;

	properties[0] = App->json_seria->LoadFloatBinary(cursor);
	ambientCoefficient = App->json_seria->LoadFloatBinary(cursor);
	radius = App->json_seria->LoadFloatBinary(cursor);
	properties[1] = App->json_seria->LoadFloatBinary(cursor);
	properties[2] = App->json_seria->LoadFloatBinary(cursor);
	properties[3] = App->json_seria->LoadFloatBinary(cursor);

	parent->box_fixed.SetFromCenterAndSize(GetGameObjectPos(), float3(radius, radius, radius));
}

void CompLight::UpdateFrustum()
{

	const CompTransform* transform = parent->GetComponentTransform();

	float4x4 trans_mat = transform->GetGlobalTransform();

	frustum.pos = trans_mat.TranslatePart();
	float3 val = -trans_mat.WorldY();
	if (val == float3::zero)val = -float3::unitY;
	frustum.front = val.Normalized();
	frustum.up = frustum.front.Cross(-frustum.WorldRight()).Normalized();

}

void CompLight::FrustumDebug()
{


	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(frustum.Edge(i).a.x, frustum.Edge(i).a.y, frustum.Edge(i).a.z);
		glVertex3f(frustum.Edge(i).b.x, frustum.Edge(i).b.y, frustum.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


}

