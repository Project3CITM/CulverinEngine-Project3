#include "CSharpScript.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ModuleInput.h"
#include "ModuleImporter.h"
#include "ImportScript.h"
#include "ModuleMap.h"
#include "CompTransform.h"
#include "CompScript.h"
#include "CompInteractive.h"
#include "ResourceScript.h"
#include "ModuleResourceManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompAudio.h"
#include "CompAnimation.h"
#include "CompButton.h"
#include "CompCollider.h"
#include "CompRigidBody.h"
#include "CompJoint.h"
#include "CompGraphic.h"
#include "CompImage.h"
#include "CompParticleSystem.h"


void CSharpScript::ActivateEmission(MonoObject* obj, bool a)
{
	if (current_game_object != nullptr)
	{
		CompParticleSystem* part = (CompParticleSystem*)current_game_object->FindComponentByType(Comp_Type::C_PARTICLE_SYSTEM);
		if (part)
		{
			part->ActivateEmitter(a);
		}
	}
}

bool CSharpScript::IsEmitterActive(MonoObject* obj)
{
	if (current_game_object != nullptr)
	{
		CompParticleSystem* part = (CompParticleSystem*)current_game_object->FindComponentByType(Comp_Type::C_PARTICLE_SYSTEM);
		if (part)
		{
			return part->IsActive();
		}
	}

	return false;
}


void CSharpScript::PlayAudioEvent(MonoObject* object, MonoString* event_name)
{
	if (current_game_object != nullptr)
	{
		CompAudio* audio = (CompAudio*)current_game_object->FindComponentByType(Comp_Type::C_AUDIO);
		if (audio != nullptr)
		{
			audio->PlayAudioEvent(mono_string_to_utf8(event_name));
		}
	}
}

void CSharpScript::StopAudioEvent(MonoObject* object, MonoString* event_name)
{
	if (current_game_object != nullptr)
	{
		CompAudio* audio = (CompAudio*)current_game_object->FindComponentByType(Comp_Type::C_AUDIO);
		if (audio != nullptr)
		{
			audio->StopAudioEvent(mono_string_to_utf8(event_name));
		}
	}
}

void CSharpScript::SetAuxiliarySends(MonoObject* object, MonoString* bus, float value)
{
	if (current_game_object != nullptr)
	{
		CompAudio* audio = (CompAudio*)current_game_object->FindComponentByType(Comp_Type::C_AUDIO);
		if (audio != nullptr)
		{
			audio->SetAuxiliarySend(mono_string_to_utf8(bus), value);
		}
	}
}

void CSharpScript::PlayAnimation(MonoObject* object, MonoString* name, mono_bool blending)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			animation->PlayClip(mono_string_to_utf8(name), blending);
		}
	}
}

void CSharpScript::SetTransition(MonoObject* object, MonoString* name, mono_bool condition)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			animation->SetTransition(mono_string_to_utf8(name), condition);
		}
	}
}

mono_bool CSharpScript::IsAnimationStopped(MonoObject * object, MonoString * name)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			return animation->IsAnimationStopped(mono_string_to_utf8(name));
		}
	}
}

mono_bool CSharpScript::IsAnimationRunning(MonoObject * object, MonoString * name)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			return animation->IsAnimationRunning(mono_string_to_utf8(name));
		}
	}
}

mono_bool CSharpScript::IsAnimOverXTime(MonoObject * object, float number_between_0_1)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			return animation->GetCurrentClip()->IsAnimOverXTime(number_between_0_1);
		}
	}
}

void CSharpScript::SetClipsSpeed(MonoObject * object, float speed_value)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			animation->SetClipsSpeed(speed_value);
		}
	}
}

// CompCollider -----------------------------------------------------------
MonoObject* CSharpScript::GetCollidedObject(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		GameObject* target = ((CompCollider*)current_game_object->FindComponentByType(Comp_Type::C_COLLIDER))->GetCollidedObject();
		if (target == nullptr)return nullptr;

		return App->importer->iScript->GetMonoObject(target);
	}
	return nullptr;
}

void CSharpScript::MoveStaticColliderTo(MonoObject * object, MonoObject * position)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classP = mono_object_get_class(position);
		MonoClassField* x_field = mono_class_get_field_from_name(classP, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classP, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classP, "z");

		float3 new_pos;

		if (x_field) mono_field_get_value(position, x_field, &new_pos.x);
		if (y_field) mono_field_get_value(position, y_field, &new_pos.y);
		if (z_field) mono_field_get_value(position, z_field, &new_pos.z);

		((CompCollider*)current_game_object->FindComponentByType(C_COLLIDER))->MoveStaticTo(new_pos);
	}
}

// CompRigidBody ----------------------------------------------------------
MonoObject * CSharpScript::GetColliderPosition(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				float3 pos = ((CompCollider*)current_game_object->FindComponentByType(C_COLLIDER))->GetGlobalPosition();

				if (x_field) mono_field_set_value(new_object, x_field, &pos.x);
				if (y_field) mono_field_set_value(new_object, y_field, &pos.y);
				if (z_field) mono_field_set_value(new_object, z_field, &pos.z);

				return new_object;
			}
		}
	}

	return nullptr;
}

MonoObject * CSharpScript::GetColliderQuaternion(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Quaternion");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");
				MonoClassField* w_field = mono_class_get_field_from_name(classT, "w");


				Quat rot = ((CompCollider*)current_game_object->FindComponentByType(C_COLLIDER))->GetGlobalQuat();

				if (x_field) mono_field_set_value(new_object, x_field, &rot.x);
				if (y_field) mono_field_set_value(new_object, y_field, &rot.y);
				if (z_field) mono_field_set_value(new_object, z_field, &rot.z);
				if (w_field) mono_field_set_value(new_object, w_field, &rot.w);

				return new_object;
			}
		}
	}

	return nullptr;
}

void CSharpScript::RemoveJoint(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		CompRigidBody* body = (CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY);
		if (body != nullptr)
		{
			body->RemoveJoint();
		}
	}
}



void CSharpScript::MoveKinematic(MonoObject * object, MonoObject * position, MonoObject * rotation)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classP = mono_object_get_class(position);
		MonoClassField* x_field = mono_class_get_field_from_name(classP, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classP, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classP, "z");

		float3 new_pos;

		if (x_field) mono_field_get_value(position, x_field, &new_pos.x);
		if (y_field) mono_field_get_value(position, y_field, &new_pos.y);
		if (z_field) mono_field_get_value(position, z_field, &new_pos.z);

		MonoClass* classR = mono_object_get_class(rotation);
		x_field = mono_class_get_field_from_name(classR, "x");
		y_field = mono_class_get_field_from_name(classR, "y");
		z_field = mono_class_get_field_from_name(classR, "z");
		MonoClassField* w_field = mono_class_get_field_from_name(classR, "w");

		Quat new_rot;

		if (x_field) mono_field_get_value(rotation, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(rotation, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(rotation, z_field, &new_rot.z);
		if (w_field) mono_field_get_value(rotation, w_field, &new_rot.w);

		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->MoveKinematic(new_pos, new_rot);
	}
}

void CSharpScript::DestroyJoint(MonoObject* object)
{
	if (current_game_object != nullptr)
	{
		((CompJoint*)current_game_object->FindComponentByType(C_JOINT))->RemoveActors();
	}
}