#include "CompTransform.h"
#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "CompCamera.h"
#include "ModuleFS.h"
#include "Scene.h"
#include "ModuleConsole.h"
#include "ModuleWindow.h"
#include "WindowInspector.h"
#include "WindowSceneWorld.h"
#include "ImGui/ImGuizmo.h"
#include "JSONSerialization.h"
#include <gl/GL.h>
#include <gl/GLU.h>

CompTransform::CompTransform(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	name_component = "Transform";
	position_pointer = &position;
	global_position_pointer = &position_global;
}

CompTransform::CompTransform(const CompTransform& copy, GameObject* parent) : Component(copy.GetType(), parent)
{
	freeze = copy.freeze; //To enable/disable editing transforms
	transform_mode = copy.transform_mode; //LOCAL or WORLD
	
	Init(copy.GetPos(), copy.GetRotEuler(), copy.GetScale()); //Set Local matrix

	name_component = "Transform";
}

CompTransform::~CompTransform()
{
	position_pointer = nullptr;
}

// Init from Rotation Quaternion
void CompTransform::Init(float3 p, float4 r, float3 s)
{
	SetPos(p);
	SetRot(Quat(r.x, r.y, r.z, r.w));
	SetScale(s);
}

// Init from Rotation Euler Angles
void CompTransform::Init(float3 p, float3 r, float3 s)
{
	SetPos(p);
	SetRot(r);
	SetScale(s);
}

void CompTransform::PreUpdate(float dt)
{
	updated = false;
}

void CompTransform::Update(float dt)
{
	// This code is made by disabling gizmos when editing transforms with editor
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
	{
		editing_transform = false;
	}
	if (App->mode_game == false)
	{
		// Show gizmo when object selected
		if (((Inspector*)App->gui->win_manager[INSPECTOR])->GetSelected() == parent && App->engine_state == EngineState::STOP)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::Enable(true);

			static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
			screen = ((SceneWorld*)App->gui->win_manager[SCENEWORLD])->GetWindowParams();
			ImGuizmo::SetRect(screen.x, screen.y, screen.z, screen.w);

			// Get global transform of the object and transpose it to edit with Guizmo
			global_transposed = global_transform.Transposed();

			if (io.WantTextInput == false)
			{
				// SET GUIZMO OPERATION ----------------------------------
				if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
				{
					mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
				}
				else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
				{
					mCurrentGizmoOperation = ImGuizmo::ROTATE;
				}
				else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
				{
					mCurrentGizmoOperation = ImGuizmo::SCALE;
				}
			}

			// EDIT TRANSFORM QITH GUIZMO
			ImGuizmo::Manipulate(App->camera->GetViewMatrix(), App->camera->GetProjMatrix(), mCurrentGizmoOperation, transform_mode, global_transposed.ptr());

			// Only edit transforms with guizmo if it's selected first
			if (ImGuizmo::IsUsing() && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && !editing_transform && !freeze)
			{
				global_transposed.Transpose();

				//If it's a root node, global and local transforms are the same
				if (parent->GetParent() == nullptr)
				{
					local_transform = global_transposed;
				}
				else
				{
					//Otherwise, set local matrix from parent global matrix (inverted)
					const CompTransform* transform = parent->GetParent()->GetComponentTransform();
					local_transform = transform->GetGlobalTransform().Inverted() * global_transposed;
				}

				if (transform_mode == ImGuizmo::WORLD)
				{
					global_transposed.Decompose(position_global, rotation_global, scale_global);
					rotation_euler_global = rotation_global.ToEulerXYZ()*RADTODEG;
				}

				local_transform.Decompose(position, rotation, scale);
				rotation_euler = rotation.ToEulerXYZ() * RADTODEG;
				toUpdate = true;
			}
		}
	}

	if (toUpdate)
	{
		if (parentUpdate) 
		{
			UpdateMatrix(ImGuizmo::LOCAL);
			parentUpdate = false;
		}
		else
		{
			UpdateMatrix(transform_mode);
		}
		toUpdate = false;
		updated = true;
	}
	else
	{
		updated = false;
	}
}

void CompTransform::ShowOptions()
{
	if (ImGui::MenuItem("Reset"))
	{
		ResetMatrix();
		ImGui::CloseCurrentPopup();
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
	if (ImGui::MenuItem("Copy Component", NULL, false))
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
			CopyValues(((CompTransform*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Reset Position"))
	{
		SetPos(float3::zero);
		ImGui::CloseCurrentPopup();
	}
	if (ImGui::MenuItem("Reset Rotation"))
	{
		SetRot(math::Quat::identity);
		ImGui::CloseCurrentPopup();
	}
	if (ImGui::MenuItem("Reset Scale"))
	{
		SetScale(math::float3(1.0f, 1.0f, 1.0f));
		ImGui::CloseCurrentPopup();
	}
}

void CompTransform::ShowInspectorInfo()
{
	// Reset Values Button -------------------------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsTransform");
	}
	ImGui::PopStyleVar();

	// Options Button --------------------------------------------------
	if (ImGui::BeginPopup("OptionsTransform"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	// SET TRANSFORM MODE -------------------------------------------------------
	if (ImGui::RadioButton("Local", transform_mode == ImGuizmo::LOCAL))
	{
		transform_mode = ImGuizmo::LOCAL;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("World", transform_mode == ImGuizmo::WORLD))
	{
		transform_mode = ImGuizmo::WORLD;
	}

	// SHOW OUTPUT (depending on mode and if transform is freezed) ---------------
	if (App->engine_state == EngineState::STOP)
	{
		ShowTransform(SPEED); // 0.5f = drag speed of editor variables of the transform
	}
	else
	{
		// In game mode, you can't edit transforms of static objects
		if (freeze)
		{
			ShowTransform(SPEED);  // 0.0f = no editable
		}
		else
		{
			ShowTransform(SPEED);
		}
	}
	// ------------------------------------------------------------------

	ImGui::TreePop();
}



void CompTransform::ShowTransform(float drag_speed)
{
	int op = ImGui::GetWindowWidth() / 4;

	// Set color of the disabled text when transform is freezed
	if(drag_speed == 0)
	{
		PushStyleColor(ImGuiCol_Text, GImGui->Style.Colors[ImGuiCol_TextDisabled]);
	}

	// Depending transform mode, edit local/world matrix (LOCAL for now)
	switch (transform_mode)
	{
	case (ImGuizmo::MODE::LOCAL):
	{
		ImGui::Text("Position"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat3("##pos", &position[0], drag_speed))
		{
			SetPos(position);
			editing_transform = true;
		}
		ImGui::Text("Rotation"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat3("##rot", &rotation_euler[0], drag_speed,-360,360))
		{
			SetRot(rotation_euler);
			editing_transform = true;
		}
		ImGui::Text("Scale"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat3("##scale", &scale[0], drag_speed))
		{
			SetScale(scale);
			editing_transform = true;
		}
		break;
	}
	case ImGuizmo::MODE::WORLD:
	{
		ImGui::Text("Position"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat3("##pos", &position_global[0], drag_speed))
		{
			SetPosGlobal(position_global);
			editing_transform = true;
		}
		ImGui::Text("Rotation"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat3("##rot", &rotation_euler_global[0], drag_speed))
		{
			SetRotGlobal(rotation_euler_global);
			editing_transform = true;
		}
		ImGui::Text("Scale"); ImGui::SameLine(op + 30);
		if (ImGui::DragFloat3("##scale", &scale_global[0], drag_speed))
		{
			SetScaleGlobal(scale_global);
			editing_transform = true;
		}
		break;
	}
	default:
		break;
	}

	if (drag_speed == 0)
	{
		ImGui::PopStyleColor();
	}
}

void CompTransform::SyncComponent(GameObject * sync_parent)
{
	UpdateMatrix(transform_mode);
	parentUpdate = false;
	toUpdate = false;
	updated = true;
}

void CompTransform::SetPosGlobal(float3 pos)
{
	transform_mode = ImGuizmo::WORLD;
	position_global = pos;
	toUpdate = true;
}

void CompTransform::SetRotGlobal(float3 rot_g)
{
	transform_mode = ImGuizmo::WORLD;
	rotation_global = Quat::FromEulerXYZ(rot_g[0] * DEGTORAD, rot_g[1] * DEGTORAD, rot_g[2] * DEGTORAD);
	toUpdate = true;
}

void CompTransform::SetRotGlobal(Quat rot_g)
{
	transform_mode = ImGuizmo::WORLD;
	rotation_global = rot_g;
	toUpdate = true;
}

void CompTransform::SetScaleGlobal(float3 scale)
{
	transform_mode = ImGuizmo::WORLD;
	scale_global = scale;
	toUpdate = true;
}

void CompTransform::SetPos(float3 pos_g)
{
	transform_mode = ImGuizmo::LOCAL;
	position = pos_g;
	toUpdate = true;
}

void CompTransform::Translate(float3 translation)
{
	position.x += translation.x;
	position.y += translation.y;
	position.z += translation.z;
}

void CompTransform::IncrementRot(float3 rot)
{
	rotation_euler += rot;
	rotation = Quat::FromEulerXYZ(rotation_euler[0] * DEGTORAD, rotation_euler[1] * DEGTORAD, rotation_euler[2] * DEGTORAD);
	toUpdate = true;
}

void CompTransform::RotateAroundAxis(float3 rot, float angle)
{
	transform_mode = ImGuizmo::LOCAL;

	rot.Normalize();
	Quat new_rot;
	new_rot.SetFromAxisAngle(rot, angle*DEGTORAD);
	rotation = new_rot * rotation;
	rotation_euler = rotation.ToEulerXYZ() * RADTODEG;

	toUpdate = true;
}

void CompTransform::SetRot(Quat rot)
{
	transform_mode = ImGuizmo::LOCAL;
	rotation_euler = rot.ToEulerXYZ() * RADTODEG;
	rotation = rot;
	toUpdate = true;
}

void CompTransform::SetRot(float3 rot)
{
	transform_mode = ImGuizmo::LOCAL;
	rotation = Quat::FromEulerXYZ(rot[0] * DEGTORAD, rot[1] * DEGTORAD, rot[2] * DEGTORAD);
	rotation_euler = rot;
	toUpdate = true;
}

void CompTransform::SetScale(float3 scal)
{
	transform_mode = ImGuizmo::LOCAL;
	scale = scal;
	toUpdate = true;
}

void CompTransform::LookAt(float3 target_pos)
{
	float3 lookat = { target_pos.x, target_pos.y, target_pos.z };
	float3 pos = { position.x, position.y, position.z };
	float3 objectUpVector = { 0.0f, 1.0f, 0.0f };

	float3 zaxis = lookat - pos;
	zaxis = zaxis.Normalized();
	float3 xaxis = objectUpVector.Cross(zaxis);
	xaxis = xaxis.Normalized();
	float3 yaxis = zaxis.Cross(xaxis);

	math::float4x4 pm = {
		xaxis.x, yaxis.x, zaxis.x, 0,
		xaxis.y, yaxis.y, zaxis.y, 0,
		xaxis.z, yaxis.z, zaxis.z, 0,
		0, 0, 0, 1
	};

	float3 rot = pm.ToEulerXYZ();
	rot.x = rot.x*RADTODEG;
	rot.y = rot.y*RADTODEG;
	rot.z = rot.z*RADTODEG;

	SetRot(rot);
	SetLocalTransform();
	SetGlobalTransform();
	//UpdateGlobalTransform();
}

void CompTransform::ResetMatrix()
{
	SetPos(float3::zero);
	SetRot(float3::zero);
	rotation_euler = float3::zero;
	SetScale(float3(1, 1, 1));
}

void CompTransform::SetLocalTransform()
{
	local_transform = float4x4::FromTRS(position, rotation, scale);
}

void CompTransform::SetGlobalTransform()
{
	global_transform = float4x4::FromTRS(position_global, rotation_global, scale_global);
}

void CompTransform::UpdateGlobalTransform()
{
	const GameObject* parent_object = parent->GetParent();

	if (parent_object != nullptr && parent_object->GetUUID() != 1)
	{
		global_transform = parent_object->GetComponentTransform()->GetGlobalTransform();
		global_transform = global_transform * local_transform;
	}
	else
	{
		global_transform = local_transform;
	}

	global_transform.Decompose(position_global, rotation_global, scale_global);
	updated = true;
	toUpdate = true;

	// Update Global matrices of all children
	parent->UpdateChildsMatrices();
}

void CompTransform::UpdateGlobalTransformWorld()
{
	float4x4 new_transorm = float4x4::FromTRS(position_global, rotation_global, scale_global);

	const GameObject* parent_object = parent->GetParent();

	if (parent_object != nullptr && parent_object->GetUUID() != 1)
	{
		local_transform = ((CompTransform*)parent_object->FindComponentByType(C_TRANSFORM))->GetGlobalTransform().Inverted()*new_transorm;
	}
	else
	{
		local_transform = new_transorm;
	}
	global_transform = new_transorm;

	local_transform.Decompose(position, rotation, scale);
	rotation_euler = rotation.ToEulerXYZ()*RADTODEG;
	updated = true;
	toUpdate = true;

	// Update Global matrices of all children
	parent->UpdateChildsMatrices();
}

// Update Global transform and call this function for all its childs
void CompTransform::UpdateGlobalMatrixRecursive()
{
	UpdateGlobalTransform();
	parent->UpdateChildsMatrices();
}

void CompTransform::SetToUpdate()
{
	toUpdate = true;
	parentUpdate = true;
}

void CompTransform::UpdateMatrix(ImGuizmo::MODE mode)
{
	switch (mode)
	{
	case (ImGuizmo::MODE::LOCAL): // Local Mode only for now
	{
		SetLocalTransform(); /*First, set local transform from inspector variables*/
		UpdateGlobalTransform(); /*Then, update global matrix*/
		break;
	}
	case (ImGuizmo::MODE::WORLD):
	{
		UpdateGlobalTransformWorld();
		SetLocalTransform();
	}
	default:
		break;
	}
}

float3 CompTransform::GetForwardVector() const
{
	return global_transform.Col3(2);
}

float3 CompTransform::GetBackwardVector() const
{
	return -GetForwardVector();
}

float3 CompTransform::GetUpVector() const
{
	return global_transform.Col3(1);
}

float3 CompTransform::GetDownVector() const
{
	return -GetUpVector();
}

float3 CompTransform::GetRightVector() const
{
	return global_transform.Col3(0);
}

float3 CompTransform::GetLeftVector() const
{
	return -GetRightVector();
}

void CompTransform::SetForwardVector(float3 vec)
{
	vec = vec.Normalized();

	float3 up = GetUpVector();

	float3 right = math::Cross(up, vec).Normalized();
	up = math::Cross(vec,right).Normalized();

	float4x4 new_transform = float4x4(float3x3(right, up, vec));

	const GameObject* parentparent = parent->GetParent();
	if (parentparent != nullptr)
	{
		new_transform = ((CompTransform*)parentparent->FindComponentByType(C_TRANSFORM))->GetGlobalTransform().Inverted()*new_transform;
	}
	rotation_euler = new_transform.ToEulerXYZ();
	rotation = Quat::FromEulerXYZ(rotation_euler.x, rotation_euler.y, rotation_euler.z);
	rotation_euler *= RADTODEG;
	toUpdate = true;
}

void CompTransform::SetBackwardVector(float3 vec)
{
	SetForwardVector(-vec);
}

void CompTransform::SetUpVector(float3 vec)
{
	vec = vec.Normalized();

	float3 frw = GetForwardVector();

	float3 right = math::Cross(vec, frw).Normalized();
	frw = math::Cross(right,vec).Normalized();

	float4x4 new_transform = float4x4(float3x3(right, vec, frw));

	const GameObject* parentparent = parent->GetParent();
	if (parentparent != nullptr && parentparent->GetUUID() != 1)
	{
		new_transform = ((CompTransform*)parentparent->FindComponentByType(C_TRANSFORM))->GetGlobalTransform().Inverted()*new_transform;
	}
	rotation_euler = new_transform.ToEulerXYZ();
	rotation = Quat::FromEulerXYZ(rotation_euler.x, rotation_euler.y, rotation_euler.z);
	rotation_euler *= RADTODEG;
	toUpdate = true;
}

void CompTransform::SetDownVector(float3 vec)
{
	SetUpVector(-vec);
}

void CompTransform::SetRightVector(float3 vec)
{
	vec = vec.Normalized();

	float3 frw = GetForwardVector();

	float3 up = math::Cross(frw, vec);
	frw = math::Cross(vec, up);
	float4x4 new_transform = float4x4(float3x3(vec, up, frw));

	const GameObject* parentparent = parent->GetParent();
	if (parentparent != nullptr && parentparent->GetUUID() != 1)
	{
		new_transform = ((CompTransform*)parentparent->FindComponentByType(C_TRANSFORM))->GetGlobalTransform().Inverted()*new_transform;
	}
	rotation_euler = new_transform.ToEulerXYZ();;
	rotation = Quat::FromEulerXYZ(rotation_euler.x, rotation_euler.y, rotation_euler.z);
	rotation_euler *= RADTODEG;
	toUpdate = true;
}

void CompTransform::SetLeftVector(float3 vec)
{
	SetRightVector(-vec);
}

float3 CompTransform::GetPos() const
{
	return position;
}

float3 CompTransform::GetPosGlobal() const
{
	return position_global;
}

float3 CompTransform::GetPosParent() const
{
	if (parent->GetParent() && parent->GetParent()->GetUUID() != 1)
	{
		return parent->GetParent()->GetComponentTransform()->GetPos();
	}
	else
	{
		return float3(0.f, 0.f, 0.f);
	}
}

Quat CompTransform::GetRot() const
{
	return rotation;
}

Quat CompTransform::GetRotGlobal() const
{
	return rotation_global;
}

Quat CompTransform::GetRotParent() const
{
	if (parent->GetParent() && parent->GetParent()->GetUUID() != 1)
	{
		return parent->GetParent()->GetComponentTransform()->GetRotGlobal();
	}	
	else
	{
		return Quat(0.f,0.f,0.f,1.f);
	}
}

float3 CompTransform::GetRotEuler() const
{
	return rotation_euler;
}

float3 CompTransform::GetGlobalRotEuler() const
{
	return rotation_euler_global;
}

float3 CompTransform::GetScale() const
{
	return scale;
}

float3 CompTransform::GetGlobalScale() const
{
	return scale_global;
}

float3 CompTransform::GetParentScale() const
{
	if (parent->GetParent() && parent->GetParent()->GetUUID() != 1)
	{
		return parent->GetParent()->GetComponentTransform()->GetGlobalScale();
	}
	else
	{
		return float3(1.f, 1.f, 1.f);
	}
}

float4x4 CompTransform::GetLocalTransform() const
{
	return local_transform;
}

float4x4 CompTransform::GetGlobalTransform() const
{
	return global_transform;
}

float3 CompTransform::GetEulerToDirection() const
{
	float3 dir;
	float3 rot = DEGTORAD * rotation_euler;
	dir.x = -cos(rot.z) * sin(rot.y)*sin(rot.x) - sin(rot.z)* cos(rot.x);
	dir.y = -sin(rot.z) * sin(rot.y) * sin(rot.x) + cos(rot.z) * cos(rot.x);
	dir.z = cos(rot.y) * sin(rot.x);
		
	return dir;
}

ImGuizmo::MODE CompTransform::GetMode() const
{
	return transform_mode;
}

bool CompTransform::GetToUpdate() const
{
	return toUpdate;
}

bool CompTransform::GetUpdated() const
{
	return updated;
}

float3* CompTransform::GetPosPointer()
{
	return position_pointer;
}

float3* CompTransform::GetGlobalPosPointer()
{
	return global_position_pointer;
}

void CompTransform::Freeze(bool freeze)
{
	this->freeze = freeze;
}

const float* CompTransform::GetMultMatrixForOpenGL() const
{
	return global_transform.Transposed().ptr();
}

void CompTransform::CopyValues(const CompTransform* component)
{
	//more...
}

void CompTransform::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	// TRANSFORM -----------
	json_object_dotset_number_with_std(object, name + "Type", C_TRANSFORM);
	// Position
	App->fs->json_array_dotset_float3(object, name + "Position", GetPos());
	// Rotation
	App->fs->json_array_dotset_float4(object, name + "Rotation", float4(GetRot().x, GetRot().y, GetRot().z, GetRot().w));
	// Scale
	App->fs->json_array_dotset_float3(object, name + "Scale", GetScale());
}

void CompTransform::Load(const JSON_Object* object, std::string name)
{
	float3 position = App->fs->json_array_dotget_float3_string(object, name + "Position");
	float4 rotation = App->fs->json_array_dotget_float4_string(object, name + "Rotation");
	float3 scale = App->fs->json_array_dotget_float3_string(object, name + "Scale");
	Init(position, rotation, scale);
	
	Enable();
}

void CompTransform::GetOwnBufferSize(uint& buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(float);			// Position
	buffer_size += sizeof(float);			// Position
	buffer_size += sizeof(float);			// Position

	buffer_size += sizeof(float);			// Rotation
	buffer_size += sizeof(float);			// Rotation
	buffer_size += sizeof(float);			// Rotation
	buffer_size += sizeof(float);			// Rotation

	buffer_size += sizeof(float);			// Scale
	buffer_size += sizeof(float);			// Scale
	buffer_size += sizeof(float);			// Scale
}

void CompTransform::SaveBinary(char ** cursor, int position) const
{
	Component::SaveBinary(cursor, position);
	App->json_seria->SaveFloat3Binary(cursor, GetPos());
	App->json_seria->SaveFloat4Binary(cursor, math::float4(GetRot().x, GetRot().y, GetRot().z, GetRot().w));
	App->json_seria->SaveFloat3Binary(cursor, GetScale());
}

void CompTransform::LoadBinary(char ** cursor)
{
	float3 position = App->json_seria->LoadFloat3Binary(cursor);
	float4 rotation = App->json_seria->LoadFloat4Binary(cursor);
	float3 scale = App->json_seria->LoadFloat3Binary(cursor);
	Init(position, rotation, scale);

	Enable();
}
