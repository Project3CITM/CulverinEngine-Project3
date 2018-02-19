#include "CompCollider.h"
#include "GameObject.h"
#include "CompTransform.h"
#include "CompScript.h"
#include "CSharpScript.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "Scene.h"
#include "MathGeoLib.h"

#include "jpPhysicsRigidBody.h"
#include "CompRigidBody.h"

CompCollider::CompCollider(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Collider";


	if (parent)
	{
		rigid_body_comp = (CompRigidBody*)parent->FindComponentByType(Comp_Type::C_RIGIDBODY);
		if (rigid_body_comp != nullptr)
		{
			LOG("Collider using the RigidBody comp...");
		}
		else
		{
			LOG("Creating a new physics body for the collider...");
			body = App->physics->GetNewRigidBody(this);
		}
		transform = parent->GetComponentTransform();
		if (!transform->GetToUpdate())
		{
			SetColliderPosition();
		}
	}
}

CompCollider::CompCollider(const CompCollider& copy, GameObject* parent) : Component(Comp_Type::C_COLLIDER, parent)
{
	uid = App->random->Int();
	name_component = "Collider";

	//Same as regular constructor since this properties depend on the parent
	if (parent)
	{
		rigid_body_comp = (CompRigidBody*)parent->FindComponentByType(Comp_Type::C_RIGIDBODY);
		if (rigid_body_comp != nullptr)
		{
			LOG("Collider using the RigidBody comp...");
		}
		else
		{
			LOG("Creating a new physics bbody for the collider...");
			body = App->physics->GetNewRigidBody(this);
		}
		transform = parent->GetComponentTransform();
		if (!transform->GetToUpdate())
		{
			SetColliderPosition();
		}
	}

	//Copy
	collider_type	= copy.collider_type;
	curr_type		= copy.curr_type;

	local_quat	= copy.local_quat;
	position	= copy.position;
	angle		 = copy.angle;

	material	= copy.material;
	size		= copy.size;
	rad			= copy.rad;


}

CompCollider::~CompCollider()
{
}

void CompCollider::Update(float dt)
{
	if (transform->GetUpdated())
	{
		SetColliderPosition();
	}
}

void CompCollider::Clear()
{
	if (body)
	{
		delete body;
		body = nullptr;
	}
}

// -----------------------------------------------------------------
void CompCollider::ShowOptions()
{
	if (ImGui::MenuItem("Reset", NULL, false, false))
	{
		// Not implemented yet.
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implemented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implemented yet.
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implemented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implemented yet.
	}
	if (ImGui::MenuItem("Copy Component"))
	{
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetComponentCopy(this);
	}
	if (ImGui::MenuItem("Paste Component As New", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (parent->FindComponentByType(((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType()) == nullptr
			|| ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType() > Comp_Type::C_UNIQUE_SEPARATOR)
		{
			parent->AddComponentCopy(*((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied());
		}
	}
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompCollider*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompCollider::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsCollider");
	}
	ImGui::PopStyleVar();
	if (ImGui::BeginPopup("OptionsCollider"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	// Set as Trigger
	if (ImGui::Checkbox("Trigger", &trigger))
	{
		// change later (check if body is static)
		body->SetAsTrigger(trigger);
		if (trigger == false)
		{
			listener = nullptr;
		}
	}
	
	// Listener selection
	if (trigger && listener == nullptr)
	{
		CompScript* sc = (CompScript*)App->scene->BlitSceneComponentsAsButtons(Comp_Type::C_SCRIPT);
		if (sc != nullptr)
		{
			listener = sc;
		}
	}

	// Collider type
	if (ImGui::Combo("Collider", (int*)&collider_type, "Sphere\0Plane\0Capsule\0Box\0", 4) && curr_type != collider_type) 
	{
		curr_type = collider_type;
		ChangeCollider();
	}
	// Collider Position
	if (ImGui::InputFloat3("Position", &position.x, 2, ImGuiInputTextFlags_EnterReturnsTrue) && body) 
	{
		SetColliderPosition();
	}

	// Collider Angle
	if (ImGui::InputFloat3("Angle", &angle.x, 2, ImGuiInputTextFlags_EnterReturnsTrue) && body) 
	{
		// Set Good Rotation
		local_quat = Quat::FromEulerXYZ(angle.x*DEGTORAD, angle.y*DEGTORAD, angle.z*DEGTORAD);
		SetColliderPosition();
	}

	// Collider options
	switch (curr_type)
	{
	case COLL_BOX:
		if (ImGui::InputFloat3("Size", &size.x, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			UpdateCollider();
		break;
	case COLL_SPHERE:
		if (ImGui::InputFloat("Radius", &rad, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			UpdateCollider();
		break;
	case COLL_PLANE:
		if (ImGui::InputFloat3("Size", &size.x, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			UpdateCollider();
		break;
	case COLL_CAPSULE:
		if ((ImGui::InputFloat("Half height", &size.z, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			|| (ImGui::InputFloat("Radius", &rad, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue)))
			UpdateCollider();
		break;
	default:
		break;
	}

	if (ImGui::Button("Size From AABB", ImVec2(100, 20))) 
	{
		SetSizeFromBoundingBox();
	}

	// Material
	ImGui::Text("Friction: Static/Dynamic");
	if (ImGui::InputFloat2("", &material.x, 2, ImGuiInputTextFlags_EnterReturnsTrue) && body) 
	{
		body->SetMaterial(material.x, material.y, material.z);
	}
	ImGui::Text("Restitution");
	if (ImGui::InputFloat("[0,1]", &material.z, 0.1f, 1.f, 3, ImGuiInputTextFlags_EnterReturnsTrue) && body) 
	{
		body->SetMaterial(material.x, material.y, material.z);
	}

	ImGui::TreePop();
}

void CompCollider::CopyValues(const CompCollider * component)
{
	//more...
}

// -----------------------------------------------------------------
void CompCollider::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);

	//Collider type
	json_object_dotset_number_with_std(object, name + "ColliderType", collider_type);


	//Position
	json_object_dotset_number_with_std(object, name + "Postion_X", position.x);
	json_object_dotset_number_with_std(object, name + "Postion_Y", position.y);
	json_object_dotset_number_with_std(object, name + "Postion_Z", position.z);

	//Angle
	json_object_dotset_number_with_std(object, name + "Angle_X", angle.x);
	json_object_dotset_number_with_std(object, name + "Angle_Y", angle.y);
	json_object_dotset_number_with_std(object, name + "Angle_Z", angle.z);

	//Material
	json_object_dotset_number_with_std(object, name + "Material_X", material.x);
	json_object_dotset_number_with_std(object, name + "Material_Y", material.y);
	json_object_dotset_number_with_std(object, name + "Material_Z", material.z);

	//Size
	json_object_dotset_number_with_std(object, name + "Size_X", size.x);
	json_object_dotset_number_with_std(object, name + "Size_Y", size.y);
	json_object_dotset_number_with_std(object, name + "Size_Z", size.z);

	//Rad
	json_object_dotset_number_with_std(object, name + "Rad", rad);

}

void CompCollider::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");

	//Position
	position.x = json_object_dotget_number_with_std(object, name + "Postion_X");
	position.y = json_object_dotget_number_with_std(object, name + "Postion_Y");
	position.z = json_object_dotget_number_with_std(object, name + "Postion_Z");

	//Angle	
	angle.x = json_object_dotget_number_with_std(object, name + "Angle_X");
	angle.y = json_object_dotget_number_with_std(object, name + "Angle_Y");
	angle.z = json_object_dotget_number_with_std(object, name + "Angle_Z");

	//Material	
	angle.x = json_object_dotget_number_with_std(object, name + "Material_X");
	angle.y = json_object_dotget_number_with_std(object, name + "Material_Y");
	angle.z = json_object_dotget_number_with_std(object, name + "Material_Z");

	//Size	
	angle.x = json_object_dotget_number_with_std(object, name + "Size_X");
	angle.y = json_object_dotget_number_with_std(object, name + "Size_Y");
	angle.z = json_object_dotget_number_with_std(object, name + "Size_Z");

	//Rad	
	rad = json_object_dotget_number_with_std(object, name + "Rad");
}

// -----------------------------------------------------------------
void CompCollider::OnTriggerEnter(Component * actor)
{
	// Call Listner OnTriggerEnter(Component* actor);
	listener->csharp->DoMainFunction(FunctionBase::CS_OnTriggerEnter, (void**)&actor);
}

void CompCollider::OnTriggerLost(Component * actor)
{
	// Call Listner OnTriggerLost(Component* actor);
	listener->csharp->DoMainFunction(FunctionBase::CS_OnTriggerLost, (void**)&actor);
}

void CompCollider::ChangeCollider()
{
	if (body != nullptr)
	{
		body->SetGeometry(size, rad, curr_type);
	}
}

void CompCollider::UpdateCollider()
{
	if (body != nullptr)
	{
		body->SetShapeScale(size, rad, curr_type);
	}
}

void CompCollider::SetColliderPosition()
{
	Quat quat = transform->GetRotGlobal()*local_quat;
	float3 fpos = transform->GetPosGlobal() + quat * position;
	if (body != nullptr)
	{
		body->SetTransform(fpos, quat);
	}
	else if (rigid_body_comp != nullptr && rigid_body_comp->GetPhysicsBody() != nullptr)
	{
		rigid_body_comp->GetPhysicsBody()->SetTransform(fpos, quat);
	}
}

void CompCollider::SetSizeFromBoundingBox()
{
	AABB box = parent->box_fixed;
	if (box.IsFinite())
	{
		size = float3(box.MaxX() - box.MinX(), box.MaxY() - box.MinY(), box.MaxZ() - box.MinZ());
		rad = size.x*0.5f;
		UpdateCollider();
	}
	else 
	{
		size = float3(1.f, 1.f, 1.f);
		rad = 0.5;
	}
}

float3 CompCollider::GetPosition() const
{
	return position;
}

Quat CompCollider::GetLocalQuat() const
{
	return local_quat;
}

jpPhysicsRigidBody * CompCollider::GetPhysicsBody()
{
	return body;
}

