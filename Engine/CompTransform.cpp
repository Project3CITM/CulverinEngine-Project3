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
#include <gl/GL.h>
#include <gl/GLU.h>

CompTransform::CompTransform(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	nameComponent = "Transformation";
}

CompTransform::CompTransform(const CompTransform& copy, GameObject* parent) : Component(Comp_Type::C_TRANSFORM, parent)
{
	freeze = copy.freeze; //To enable/disable editing transforms
	transform_mode = copy.transform_mode; //LOCAL or WORLD
	
	Init(copy.GetPos(), copy.GetRotEuler(), copy.GetScale()); //Set Local matrix

	nameComponent = "Transformation";
}

CompTransform::~CompTransform()
{
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

void CompTransform::Update(float dt)
{
	// This code is made by disabling gizmos when editing transforms with editor
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
	{
		editing_transform = false;
	}

	// Show gizmo when object selected
	if (((Inspector*)App->gui->winManager[INSPECTOR])->GetSelected() == parent && App->engineState == EngineState::STOP)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::Enable(true);

		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
		screen = ((SceneWorld*)App->gui->winManager[SCENEWORLD])->GetWindowParams();
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

			local_transform.Decompose(position, rotation, scale);
			rotation_euler = rotation.ToEulerXYZ() * RADTODEG;
			toUpdate = true;
		}
	}

	if (toUpdate)
	{
		UpdateMatrix(transform_mode);
		toUpdate = false;
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
	if (ImGui::MenuItem("Copy Component", NULL, false, false))
	{
		// Component* Copy = this;
	}
	if (ImGui::MenuItem("Paste Component", NULL, false, false))
	{
		//parent->AddComponent(App->scene->copyComponent->GetType())
		// Create contructor Component Copy or add funtion to add info
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
	//ImGui::SameLine();
	//if (ImGui::RadioButton("World", transform_mode == ImGuizmo::WORLD))
	//{
	//	transform_mode = ImGuizmo::WORLD;
	//}

	// SHOW OUTPUT (depending on mode and if transform is freezed) ---------------
	if (App->engineState == EngineState::STOP)
	{
		ShowTransform(0.5f); // 0.5f = drag speed of editor variables of the transform
	}
	else
	{
		// In game mode, you can't edit transforms of static objects
		if (freeze)
		{
			ShowTransform(0.0f);  // 0.0f = no editable
		}
		else
		{
			ShowTransform(0.5f);
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
		if (ImGui::DragFloat3("##rot", &rotation_euler[0], drag_speed))
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
	default:
		break;
	}

	if (drag_speed == 0)
	{
		ImGui::PopStyleColor();
	}
}

void CompTransform::SetPosGlobal(float3 pos)
{
	position_global = pos;
	toUpdate = true;
}

void CompTransform::SetRotGlobal(float3 rot_g)
{
	rotation_global = Quat::FromEulerXYZ(rot_g[0] * DEGTORAD, rot_g[1] * DEGTORAD, rot_g[2] * DEGTORAD);
	toUpdate = true;
}

void CompTransform::SetScaleGlobal(float3 scale)
{
	scale_global = scale;
	toUpdate = true;
}

void CompTransform::SetPos(float3 pos_g)
{
	position = pos_g;
	toUpdate = true;
}

void CompTransform::IncrementRot(float3 rot)
{
	rotation_euler += rot;
	rotation = Quat::FromEulerXYZ(rotation_euler[0] * DEGTORAD, rotation_euler[1] * DEGTORAD, rotation_euler[2] * DEGTORAD);
	toUpdate = true;
}

void CompTransform::SetRot(Quat rot)
{
	rotation_euler = rot.ToEulerXYZ() * RADTODEG;
	rotation = rot;
	toUpdate = true;
}

void CompTransform::SetRot(float3 rot)
{
	rotation = Quat::FromEulerXYZ(rot[0] * DEGTORAD, rot[1] * DEGTORAD, rot[2] * DEGTORAD);
	toUpdate = true;
}

void CompTransform::SetScale(float3 scal)
{
	scale = scal;
	toUpdate = true;
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
	global_transform = float4x4::FromTRS(position_global, rotation_global, scale);
}

void CompTransform::UpdateLocalTransform()
{
	local_transform = float4x4::identity;
	const GameObject* parent_object = parent;

	//If it has no parents, local transform is the same as the global transform
	if (parent_object->GetParent() == nullptr)
	{
		local_transform = global_transform;		
	}
	//If not, apply parent global transform (Inverted) to get its local transform
	else
	{
		CompTransform* parent_transform = parent_object->GetParent()->GetComponentTransform();
		local_transform = global_transform * parent_transform->GetGlobalTransform().Inverted();
	}
}

void CompTransform::UpdateGlobalTransform()
{
	global_transform = float4x4::identity;
	std::list<const GameObject*> parents;
	const GameObject* parent_object = parent;

	// Put all parents of the game object to pass from local to global matrix
	while (parent_object != nullptr)
	{
		parents.push_back(parent_object);
		parent_object = parent_object->GetParent();
	}

	std::list<const GameObject*>::reverse_iterator item = parents.rbegin();
	
	// Multiply parents local transforms to get the Global transform of this game object
	while (item !=  parents.crend())
	{
		float4x4 matrix = (*item)->GetComponentTransform()->GetLocalTransform();
		global_transform = global_transform * matrix;
		item++;
	}
}

// Update Global transform and call this function for all its childs
void CompTransform::UpdateGlobalMatrixRecursive()
{
	UpdateGlobalTransform();
	parent->UpdateChildsMatrices();
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
	//case (ImGuizmo::MODE::WORLD):
	//{
	//	SetGlobalTransform(); /*First, set global transform from inspector variables*/
	//	UpdateLocalTransform(); /*Then, update local matrix*/
	//	break;
	//}
	default:
		break;
	}

	// Update Global matrices of all children
	parent->UpdateChildsMatrices();
}

float3 CompTransform::GetPos() const
{
	return position;
}

float3 CompTransform::GetPosGlobal() const
{
	return position_global;
}

Quat CompTransform::GetRot() const
{
	return rotation;
}

float3 CompTransform::GetRotEuler() const
{
	return rotation_euler;
}

float3 CompTransform::GetScale() const
{
	return scale;
}

float4x4 CompTransform::GetLocalTransform() const
{
	return local_transform;
}

float4x4 CompTransform::GetGlobalTransform() const
{
	return global_transform;
}

ImGuizmo::MODE CompTransform::GetMode() const
{
	return transform_mode;
}

void CompTransform::Freeze(bool freeze)
{
	this->freeze = freeze;
}

const float* CompTransform::GetMultMatrixForOpenGL() const
{
	return global_transform.Transposed().ptr();
}

void CompTransform::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
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

