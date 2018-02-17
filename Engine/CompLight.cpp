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

CompLight::CompLight(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	color = float4(255, 255, 255, 255);
	type = NO_LIGHT_TYPE;
	attenuation = 1;
	ambientCoefficient = 1;
	
	name_component = "Light component";

	types_lights += "Point Light";
	types_lights += '\0';	
	types_lights += "Directional Light";
	types_lights += '\0';

	color_temp[0] = 1;
	color_temp[1] = 1;
	color_temp[2] = 1;
	color_temp[3] = 1;

	App->renderer3D->LoadImage_devil("Assets/Bulb_Texture.png",&texture_bulb);

	plane = App->module_lightning->light_UI_plane;
	App->module_lightning->PushLight(this);

}

CompLight::CompLight(const CompLight & copy, GameObject * parent) : Component(Comp_Type::C_LIGHT, parent)
{
	App->module_lightning->DeleteLight(this);
}

CompLight::~CompLight()
{
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
}

void CompLight::Update(float dt)
{
	
	
}

void CompLight::Draw()
{
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.8);



	App->renderer3D->default_shader->Bind();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, texture_bulb);

	Frustum camFrust = App->renderer3D->active_camera->frustum;// App->camera->GetFrustum();
	float4x4 temp = camFrust.ViewMatrix();

	GLint view2Loc = glGetUniformLocation(App->renderer3D->default_shader->programID, "view");
	GLint modelLoc = glGetUniformLocation(App->renderer3D->default_shader->programID, "model");
	GLint viewLoc = glGetUniformLocation(App->renderer3D->default_shader->programID, "viewproj");

	CompTransform* trans = parent->GetComponentTransform();
	float3 Direction = App->renderer3D->active_camera->frustum.pos - trans->GetPos();
	Quat Rotation = Quat::LookAt(float3(0.0f, 1.0f, 0.0f), Direction, float3(0.0f, 0.0f, -1.0f), float3(0.0f, 1.0f, 0.0f));
	
	auto mat = float4x4::FromTRS(trans->GetPos(), Rotation, float3(1, 1, 1)).Transposed();


	glUniformMatrix4fv(view2Loc, 1, GL_TRUE, temp.Inverted().ptr());
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, mat.ptr());
	glUniformMatrix4fv(viewLoc, 1, GL_TRUE, camFrust.ViewProjMatrix().ptr());


	int total_save_buffer = 5;

	if (plane->vertices.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, plane->id_total_buffer);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (0 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (3 * sizeof(float)));

		glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane->indices_id);
		glDrawElements(GL_TRIANGLES, plane->num_indices, GL_UNSIGNED_INT, NULL);

	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	App->renderer3D->default_shader->Unbind();
	glDisable(GL_ALPHA_TEST);
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

	ImGui::DragFloat("Atenuation", &attenuation);
	ImGui::DragFloat("Ambient Coefficient", &ambientCoefficient);
	ImGui::Combo("Light Type", &ui_light_type, types_lights.c_str());
	type = (Light_type)ui_light_type;

	ImGui::TreePop();

}

void CompLight::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{

	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());

	json_object_dotset_number_with_std(object, name + "Light Type", (int)type);
	json_object_dotset_number_with_std(object, name + "Attenuation", attenuation);
	json_object_dotset_number_with_std(object, name + "Ambient Coefficient", ambientCoefficient);

}

void CompLight::Load(const JSON_Object * object, std::string name)
{

	uid = json_object_dotget_number_with_std(object, name + "UUID");
	ui_light_type =json_object_dotget_number_with_std(object, name + "Attenuation");
	type = (Light_type)ui_light_type;
	ambientCoefficient = json_object_dotget_number_with_std(object, name + "Ambient Coefficient");
	attenuation = json_object_dotget_number_with_std(object, name + "Attenuation");
}

