#include "CSharpScript.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ModuleInput.h"
#include "ModuleImporter.h"
#include "ModulePhysics.h"
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
#include "CompLight.h"
#include "CompParticleSystem.h"
#include "CompMaterial.h"
#include "Materials.h"
#include "ShadersLib.h"
#include "ImportMaterial.h"


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
	return false;
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
	return false;
}

mono_bool CSharpScript::IsAnimOverXTime(MonoObject * object, float number_between_0_1)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			if (animation->GetBlendingClip() == nullptr)
			{
				return animation->GetCurrentClip()->IsAnimOverXTime(number_between_0_1);
			}
			else
			{
				return false;
			}
		}
	}
	return false;
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

float CSharpScript::GetClipDuration(MonoObject * object, MonoString * name)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			AnimationClip* clip = animation->GetClipFromName(mono_string_to_utf8(name));
			if (clip != nullptr)
			{
				return (clip->end_frame_time - clip->start_frame_time) * clip->speed_factor;
			}
			else
			{
				return 0.0f;
			}
		}
	}
	return 0.0f;
}

void CSharpScript::SetClipDuration(MonoObject * object, MonoString * name, float duration)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			AnimationClip* clip = animation->GetClipFromName(mono_string_to_utf8(name));
			if (clip != nullptr)
			{
				clip->speed_factor = (clip->end_frame_time - clip->start_frame_time) / duration;
			}
		}
	}
}

void CSharpScript::SetActiveBlendingClip(MonoObject * object, MonoString * name)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			AnimationNode* active_node = animation->GetActiveNode();
			if (active_node != nullptr)
			{
				active_node->SetActiveBlendingClip(mono_string_to_utf8(name));
			}
		}
	}
}

void CSharpScript::SetActiveBlendingClipWeight(MonoObject * object, float weight)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			AnimationNode* active_node = animation->GetActiveNode();
			if (active_node != nullptr)
			{
				active_node->SetActiveBlendingClipWeight(weight);
			}
		}
	}
}

void CSharpScript::SetBlendInTime(MonoObject * object, MonoString * name, float time)
{
	if (current_game_object != nullptr)
	{
		CompAnimation* animation = (CompAnimation*)current_game_object->FindComponentByType(Comp_Type::C_ANIMATION);
		if (animation != nullptr)
		{
			AnimationClip* clip = animation->GetClipFromName(mono_string_to_utf8(name));
			if (clip != nullptr)
			{
				clip->total_blending_time = time;
			}
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

void CSharpScript::CallOnContact(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		CollisionData data;
		((CompCollider*)current_game_object->FindComponentByType(C_COLLIDER))->OnContact(data);
	}
}

void CSharpScript::CallOnTriggerEnter(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		((CompCollider*)current_game_object->FindComponentByType(C_COLLIDER))->OnTriggerEnter(nullptr);
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

void CSharpScript::ApplyForce(MonoObject * object, MonoObject * force)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classP = mono_object_get_class(force);
		MonoClassField* x_field = mono_class_get_field_from_name(classP, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classP, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classP, "z");

		float3 new_force;

		if (x_field) mono_field_get_value(force, x_field, &new_force.x);
		if (y_field) mono_field_get_value(force, y_field, &new_force.y);
		if (z_field) mono_field_get_value(force, z_field, &new_force.z);

		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->ApplyForce(new_force);
	}
}

void CSharpScript::ApplyImpulse(MonoObject * object, MonoObject * impulse)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classP = mono_object_get_class(impulse);
		MonoClassField* x_field = mono_class_get_field_from_name(classP, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classP, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classP, "z");

		float3 new_impuse;

		if (x_field) mono_field_get_value(impulse, x_field, &new_impuse.x);
		if (y_field) mono_field_get_value(impulse, y_field, &new_impuse.y);
		if (z_field) mono_field_get_value(impulse, z_field, &new_impuse.z);

		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->ApplyImpulse(new_impuse);
	}
}

void CSharpScript::ApplyTorqueForce(MonoObject * object, MonoObject * force)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classP = mono_object_get_class(force);
		MonoClassField* x_field = mono_class_get_field_from_name(classP, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classP, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classP, "z");

		float3 new_force;

		if (x_field) mono_field_get_value(force, x_field, &new_force.x);
		if (y_field) mono_field_get_value(force, y_field, &new_force.y);
		if (z_field) mono_field_get_value(force, z_field, &new_force.z);

		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->ApplyTorqueForce(new_force);
	}
}

void CSharpScript::ApplyTorqueImpulse(MonoObject * object, MonoObject * impulse)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classP = mono_object_get_class(impulse);
		MonoClassField* x_field = mono_class_get_field_from_name(classP, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classP, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classP, "z");

		float3 new_impuse;

		if (x_field) mono_field_get_value(impulse, x_field, &new_impuse.x);
		if (y_field) mono_field_get_value(impulse, y_field, &new_impuse.y);
		if (z_field) mono_field_get_value(impulse, z_field, &new_impuse.z);

		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->ApplyTorqueImpulse(new_impuse);
	}
}

void CSharpScript::LockTransform(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->LockTransform();
	}
}

void CSharpScript::UnLockTransform(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->UnLockTransform();
	}
}

void CSharpScript::ResetForce(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->SetMomentumToZero();
	}
}

void CSharpScript::WakeUp(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->WakeUp();
	}
}

void CSharpScript::SetAtMaxJointPose(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		((CompRigidBody*)current_game_object->FindComponentByType(C_RIGIDBODY))->SetMaxJointPose();
	}
}

void CSharpScript::DestroyJoint(MonoObject* object)
{
	if (current_game_object != nullptr)
	{
		((CompJoint*)current_game_object->FindComponentByType(C_JOINT))->RemoveActors();
	}
}

MonoObject * CSharpScript::RayCast(MonoObject * origin, MonoObject * direction, float distance)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classP = mono_object_get_class(origin);
		MonoClassField* x_field = mono_class_get_field_from_name(classP, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classP, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classP, "z");

		float3 ray_origin;

		if (x_field) mono_field_get_value(origin, x_field, &ray_origin.x);
		if (y_field) mono_field_get_value(origin, y_field, &ray_origin.y);
		if (z_field) mono_field_get_value(origin, z_field, &ray_origin.z);

		MonoClass* classR = mono_object_get_class(direction);

		x_field = mono_class_get_field_from_name(classR, "x");
		y_field = mono_class_get_field_from_name(classR, "y");
		z_field = mono_class_get_field_from_name(classR, "z");

		float3 ray_direction;

		if (x_field) mono_field_get_value(direction, x_field, &ray_direction.x);
		if (y_field) mono_field_get_value(direction, y_field, &ray_direction.y);
		if (z_field) mono_field_get_value(direction, z_field, &ray_direction.z);

		GameObject* target = App->physics->RayCast(ray_origin, ray_direction, distance);
		
		if (target == nullptr)return nullptr;

		return App->importer->iScript->GetMonoObject(target);
	}
	return nullptr;
}

void CSharpScript::SetAlbedo(MonoObject * object, MonoString * string)
{
	const char* c_string = mono_string_to_utf8(string);
	
	CompMaterial* c_material = current_game_object->GetComponentMaterial();
	if (c_material == nullptr)
	{
		LOG("ERROR %s", c_string);
		return;
	}

	ResourceMaterial* texture_resource = (ResourceMaterial*)App->resource_manager->GetResource((char*)c_string);
	if (texture_resource == nullptr)
	{
		LOG("ERROR %s", c_string);
		return;
	}

	texture_resource->num_game_objects_use_me++;
	if (texture_resource->IsLoadedToMemory() == Resource::State::UNLOADED)
	{
		std::string temp = std::to_string(texture_resource->GetUUID());
		App->importer->iMaterial->LoadResource(temp.c_str(), texture_resource);
	}
		
	c_material->material->textures[0].value = (ResourceMaterial*)texture_resource;
}

void CSharpScript::SetNormals(MonoObject * object, MonoString * string)
{
	const char* c_string = mono_string_to_utf8(string);

	CompMaterial* c_material = current_game_object->GetComponentMaterial();
	if (c_material == nullptr)
	{
		LOG("ERROR %s", c_string);
		return;
	}

	ResourceMaterial* texture_resource = (ResourceMaterial*)App->resource_manager->GetResource((char*)c_string);
	if (texture_resource == nullptr)
	{
		LOG("ERROR %s", c_string);
		return;
	}

	texture_resource->num_game_objects_use_me++;
	if (texture_resource->IsLoadedToMemory() == Resource::State::UNLOADED)
	{
		std::string temp = std::to_string(texture_resource->GetUUID());
		App->importer->iMaterial->LoadResource(temp.c_str(), texture_resource);
	}

	c_material->material->textures[1].value = (ResourceMaterial*)texture_resource;
}

void CSharpScript::SetAmbientOcclusion(MonoObject * object, MonoString * string)
{
	const char* c_string = mono_string_to_utf8(string);

	CompMaterial* c_material = current_game_object->GetComponentMaterial();
	if (c_material == nullptr)
	{
		LOG("ERROR %s", c_string);
		return;
	}

	ResourceMaterial* texture_resource = (ResourceMaterial*)App->resource_manager->GetResource((char*)c_string);
	if (texture_resource == nullptr)
	{
		LOG("ERROR %s", c_string);
		return;
	}

	texture_resource->num_game_objects_use_me++;
	if (texture_resource->IsLoadedToMemory() == Resource::State::UNLOADED)
	{
		std::string temp = std::to_string(texture_resource->GetUUID());
		App->importer->iMaterial->LoadResource(temp.c_str(), texture_resource);
	}

	c_material->material->textures[2].value = (ResourceMaterial*)texture_resource;
}

float CSharpScript::GetIntensity(MonoObject * object)
{
	CompLight* c_light = current_game_object->GetComponentLight();

	if (c_light == nullptr)
		return 0;
	return c_light->properties[0];
}

void CSharpScript::SetIntensity(MonoObject * object, float value)
{
	CompLight* c_light = current_game_object->GetComponentLight();

	if (c_light == nullptr)
		return ;
	c_light->properties[0] = value;
}

float CSharpScript::GetConstant(MonoObject * object)
{
	CompLight* c_light = current_game_object->GetComponentLight();

	if (c_light == nullptr)
		return 0;
	return c_light->properties[1];
}

void CSharpScript::SetConstant(MonoObject * object, float value)
{
	CompLight* c_light = current_game_object->GetComponentLight();

	if (c_light == nullptr)
		return;
	c_light->properties[1] = value;
}

float CSharpScript::GetLinear(MonoObject * object)
{
	CompLight* c_light = current_game_object->GetComponentLight();

	if (c_light == nullptr)
		return 0;
	return c_light->properties[2];
}

void CSharpScript::SetLinear(MonoObject * object, float value)
{
	CompLight* c_light = current_game_object->GetComponentLight();

	if (c_light == nullptr)
		return;
	c_light->properties[2] = value;
}

float CSharpScript::GetQuadratic(MonoObject * object)
{
	CompLight* c_light = current_game_object->GetComponentLight();

	if (c_light == nullptr)
		return 0;
	return c_light->properties[3];
}

void CSharpScript::SetQuadratic(MonoObject * object, float value)
{
	CompLight* c_light = current_game_object->GetComponentLight();

	if (c_light == nullptr)
		return;
	c_light->properties[3] = value;
}