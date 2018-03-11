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
#include "CompGraphic.h"
#include "CompImage.h"



MonoObject* CSharpScript::GetForwardVector(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			Component* obj = App->importer->iScript->GetComponentMono(object);
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)obj;
				float3 new_vec;
				new_vec = transform->GetForwardVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject* CSharpScript::GetBackwardVector(MonoObject * object)
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

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetBackwardVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject* CSharpScript::GetUpVector(MonoObject* object)
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

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetUpVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetDownVector(MonoObject* object)
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

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetDownVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject* CSharpScript::GetRightVector(MonoObject* object)
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

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetRightVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject* CSharpScript::GetLeftVector(MonoObject* object)
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

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetLeftVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

void CSharpScript::SetForwardVector(MonoObject* object, MonoObject* vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetForwardVector(new_vec);
	}
}

void CSharpScript::SetBackwardVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetBackwardVector(new_vec);
	}
}

void CSharpScript::SetUpVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetUpVector(new_vec);
	}
}

void CSharpScript::SetDownVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetDownVector(new_vec);
	}
}

void CSharpScript::SetRightVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetRightVector(new_vec);
	}
}

void CSharpScript::SetLeftVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetLeftVector(new_vec);
	}
}

MonoObject* CSharpScript::GetPosition(MonoObject* object)
{
	Component* obj = App->importer->iScript->GetComponentMono(object);
	if (current_game_object != nullptr && obj != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		float3* new_pos = ((CompTransform*)obj)->GetPosPointer();
		MonoObject* new_obj = App->importer->iScript->GetMonoObject(((CompTransform*)obj)->GetPosPointer());
		if (new_obj != nullptr)
		{
			if (x_field) mono_field_set_value(new_obj, x_field, &new_pos->x);
			if (y_field) mono_field_set_value(new_obj, y_field, &new_pos->y);
			if (z_field) mono_field_set_value(new_obj, z_field, &new_pos->z);

			return new_obj;
		}
		else
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				if (x_field) mono_field_set_value(new_object, x_field, &new_pos->x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_pos->y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_pos->z);

				// Put in map the new MonoObject
				App->importer->iScript->UpdateMonoPos(new_pos, new_object);

				return new_object;
			}
		}
	}
	return nullptr;
}

MonoObject* CSharpScript::GetGlobalPosition(MonoObject * object)
{
	CheckMonoObject(object);
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

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_pos;
				new_pos = transform->GetPosGlobal();

				if (x_field) mono_field_set_value(new_object, x_field, &new_pos.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_pos.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_pos.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

// We need to pass the MonoObject* to get a reference on act
void CSharpScript::SetPosition(MonoObject* object, MonoObject* vector3)
{
	Component* obj = App->importer->iScript->GetComponentMono(object);
	if (current_game_object != nullptr || obj != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

		CompTransform* transform = (CompTransform*)obj;
		transform->SetPos(new_pos);
		int hjer = 0;
	}
}

void CSharpScript::SetGlobalPosition(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetPosGlobal(new_pos);
	}
}

void CSharpScript::Translate(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 added_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &added_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &added_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &added_pos.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->Translate(added_pos);
	}
}

MonoObject* CSharpScript::GetRotation(MonoObject* object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			Component* obj = App->importer->iScript->GetComponentMono(object);
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object && obj)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)obj;
				float3 new_pos;
				new_pos = transform->GetRotEuler();

				if (x_field) mono_field_set_value(new_object, x_field, &new_pos.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_pos.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_pos.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetGlobalRotation(MonoObject * object)
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

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_pos;
				new_pos = transform->GetGlobalRotEuler();

				if (x_field) mono_field_set_value(new_object, x_field, &new_pos.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_pos.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_pos.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

void CSharpScript::SetRotation(MonoObject* object, MonoObject* vector3)
{
	Component* obj = App->importer->iScript->GetComponentMono(object);
	if (current_game_object != nullptr && obj)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		CompTransform* transform = (CompTransform*)obj;
		transform->SetRot(new_rot);
	}
}

void CSharpScript::SetGlobalRotation(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetRotGlobal(new_rot);
	}
}

void CSharpScript::IncrementRotation(MonoObject* object, MonoObject* vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->IncrementRot(new_rot);
	}
}

void CSharpScript::RotateAroundAxis(MonoObject * object, MonoObject * vector3, float value)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->RotateAroundAxis(new_rot, value);
	}
}

void CSharpScript::SetScale(MonoObject* object, MonoObject* vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_scale;

		if (x_field) mono_field_get_value(vector3, x_field, &new_scale.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_scale.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_scale.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetScale(new_scale);
	}
}

MonoObject * CSharpScript::GetScale(MonoObject * object)
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

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 get_scale;
				get_scale = transform->GetScale();

				if (x_field) mono_field_set_value(new_object, x_field, &get_scale.x);
				if (y_field) mono_field_set_value(new_object, y_field, &get_scale.y);
				if (z_field) mono_field_set_value(new_object, z_field, &get_scale.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

void CSharpScript::LookAt(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 target_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &target_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &target_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &target_pos.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->LookAt(target_pos);
	}
}

void CSharpScript::LookAtTrans(MonoObject * object, MonoObject * transform)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(transform);
		MonoClassField* field = mono_class_get_field_from_name(classT, "position");

		float3 target_pos;

		if (field) mono_field_get_value(transform, field, &target_pos);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->LookAt(target_pos);
	}
}

MonoObject* CSharpScript::GetLocalTransform(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "float4x4");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* a0_field = mono_class_get_field_from_name(classT, "a0");
				MonoClassField* a1_field = mono_class_get_field_from_name(classT, "a1");
				MonoClassField* a2_field = mono_class_get_field_from_name(classT, "a2");
				MonoClassField* a3_field = mono_class_get_field_from_name(classT, "a3");

				MonoClassField* b0_field = mono_class_get_field_from_name(classT, "b0");
				MonoClassField* b1_field = mono_class_get_field_from_name(classT, "b1");
				MonoClassField* b2_field = mono_class_get_field_from_name(classT, "b2");
				MonoClassField* b3_field = mono_class_get_field_from_name(classT, "b3");

				MonoClassField* c0_field = mono_class_get_field_from_name(classT, "c0");
				MonoClassField* c1_field = mono_class_get_field_from_name(classT, "c1");
				MonoClassField* c2_field = mono_class_get_field_from_name(classT, "c2");
				MonoClassField* c3_field = mono_class_get_field_from_name(classT, "c3");

				MonoClassField* d0_field = mono_class_get_field_from_name(classT, "d0");
				MonoClassField* d1_field = mono_class_get_field_from_name(classT, "d1");
				MonoClassField* d2_field = mono_class_get_field_from_name(classT, "d2");
				MonoClassField* d3_field = mono_class_get_field_from_name(classT, "d3");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float4x4 local_transform = transform->GetLocalTransform();

				if (a0_field) mono_field_set_value(new_object, a0_field, &local_transform.At(0, 0));
				if (a1_field) mono_field_set_value(new_object, a1_field, &local_transform.At(0, 1));
				if (a2_field) mono_field_set_value(new_object, a2_field, &local_transform.At(0, 2));
				if (a3_field) mono_field_set_value(new_object, a3_field, &local_transform.At(0, 3));

				if (b0_field) mono_field_set_value(new_object, b0_field, &local_transform.At(1, 0));
				if (b1_field) mono_field_set_value(new_object, b1_field, &local_transform.At(1, 1));
				if (b2_field) mono_field_set_value(new_object, b2_field, &local_transform.At(1, 2));
				if (b3_field) mono_field_set_value(new_object, b3_field, &local_transform.At(1, 3));

				if (c0_field) mono_field_set_value(new_object, c0_field, &local_transform.At(2, 0));
				if (c1_field) mono_field_set_value(new_object, c1_field, &local_transform.At(2, 1));
				if (c2_field) mono_field_set_value(new_object, c2_field, &local_transform.At(2, 2));
				if (c3_field) mono_field_set_value(new_object, c3_field, &local_transform.At(2, 3));

				if (d0_field) mono_field_set_value(new_object, d0_field, &local_transform.At(3, 0));
				if (d1_field) mono_field_set_value(new_object, d1_field, &local_transform.At(3, 1));
				if (d2_field) mono_field_set_value(new_object, d2_field, &local_transform.At(3, 2));
				if (d3_field) mono_field_set_value(new_object, d3_field, &local_transform.At(3, 3));

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject* CSharpScript::GetGlobalTransform(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "float4x4");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* a0_field = mono_class_get_field_from_name(classT, "a0");
				MonoClassField* a1_field = mono_class_get_field_from_name(classT, "a1");
				MonoClassField* a2_field = mono_class_get_field_from_name(classT, "a2");
				MonoClassField* a3_field = mono_class_get_field_from_name(classT, "a3");

				MonoClassField* b0_field = mono_class_get_field_from_name(classT, "b0");
				MonoClassField* b1_field = mono_class_get_field_from_name(classT, "b1");
				MonoClassField* b2_field = mono_class_get_field_from_name(classT, "b2");
				MonoClassField* b3_field = mono_class_get_field_from_name(classT, "b3");

				MonoClassField* c0_field = mono_class_get_field_from_name(classT, "c0");
				MonoClassField* c1_field = mono_class_get_field_from_name(classT, "c1");
				MonoClassField* c2_field = mono_class_get_field_from_name(classT, "c2");
				MonoClassField* c3_field = mono_class_get_field_from_name(classT, "c3");

				MonoClassField* d0_field = mono_class_get_field_from_name(classT, "d0");
				MonoClassField* d1_field = mono_class_get_field_from_name(classT, "d1");
				MonoClassField* d2_field = mono_class_get_field_from_name(classT, "d2");
				MonoClassField* d3_field = mono_class_get_field_from_name(classT, "d3");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float4x4 global_transform = transform->GetGlobalTransform();

				if (a0_field) mono_field_set_value(new_object, a0_field, &global_transform.At(0, 0));
				if (a1_field) mono_field_set_value(new_object, a1_field, &global_transform.At(0, 1));
				if (a2_field) mono_field_set_value(new_object, a2_field, &global_transform.At(0, 2));
				if (a3_field) mono_field_set_value(new_object, a3_field, &global_transform.At(0, 3));

				if (b0_field) mono_field_set_value(new_object, b0_field, &global_transform.At(1, 0));
				if (b1_field) mono_field_set_value(new_object, b1_field, &global_transform.At(1, 1));
				if (b2_field) mono_field_set_value(new_object, b2_field, &global_transform.At(1, 2));
				if (b3_field) mono_field_set_value(new_object, b3_field, &global_transform.At(1, 3));
				
				if (c0_field) mono_field_set_value(new_object, c0_field, &global_transform.At(2, 0));
				if (c1_field) mono_field_set_value(new_object, c1_field, &global_transform.At(2, 1));
				if (c2_field) mono_field_set_value(new_object, c2_field, &global_transform.At(2, 2));
				if (c3_field) mono_field_set_value(new_object, c3_field, &global_transform.At(2, 3));
				
				if (d0_field) mono_field_set_value(new_object, d0_field, &global_transform.At(3, 0));
				if (d1_field) mono_field_set_value(new_object, d1_field, &global_transform.At(3, 1));
				if (d2_field) mono_field_set_value(new_object, d2_field, &global_transform.At(3, 2));
				if (d3_field) mono_field_set_value(new_object, d3_field, &global_transform.At(3, 3));

				return new_object;
			}
		}
		return nullptr;
	}
}
