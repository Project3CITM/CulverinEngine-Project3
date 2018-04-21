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
#include "CompText.h"
#include "CompCanvas.h"

void CSharpScript::Activate(MonoObject* object, int uid)
{
	if (current_game_object != nullptr)
	{

		CompInteractive* interactive = (CompInteractive*)current_game_object->FindComponentByType(Comp_Type::C_BUTTON);
		if (interactive != nullptr)
		{
			interactive->Activate();
			return;
		}
		interactive = (CompInteractive*)current_game_object->FindComponentByType(Comp_Type::C_CHECK_BOX);
		if (interactive != nullptr)
		{
			interactive->Activate();
			return;

		}
	}
}

void CSharpScript::Deactivate(MonoObject * object, int uid)
{
	if (current_game_object != nullptr)
	{
		CompInteractive* interactive = (CompInteractive*)current_game_object->FindComponentByType(Comp_Type::C_BUTTON);
		if (interactive != nullptr)
		{
			interactive->Deactive();
			return;
		}
		interactive = (CompInteractive*)current_game_object->FindComponentByType(Comp_Type::C_CHECK_BOX);
		if (interactive != nullptr)
		{
			interactive->Deactive();
			return;

		}
	}
}

void CSharpScript::Clicked(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		CompButton* interactive = (CompButton*)current_game_object->FindComponentByType(Comp_Type::C_BUTTON);
		if (interactive != nullptr)
		{
			interactive->OnClick();
		}
	}
}

void CSharpScript::SetInteractivity(MonoObject * object, mono_bool enable)
{
	if (current_game_object != nullptr)
	{
		CompInteractive* item = (CompInteractive*)App->importer->iScript->GetComponentMono(object);
		if (item != nullptr)
		{
			item->SetInteractivity(enable);
		}
	}
}

void CSharpScript::SetRaycastTarget(MonoObject * object, mono_bool flag)
{
	if (current_game_object != nullptr)
	{
		CompImage* graphic = (CompImage*)current_game_object->FindComponentByType(Comp_Type::C_IMAGE);
		if (graphic != nullptr)
		{
			graphic->SetRaycastTarget(flag);
		}
	}
}

void CSharpScript::FillAmount(MonoObject * object, float value)
{
	if (current_game_object != nullptr)
	{
		CompImage* image = (CompImage*)current_game_object->FindComponentByType(Comp_Type::C_IMAGE);
		if (image != nullptr)
		{
			image->FillAmount(value);
		}
	}
}

void CSharpScript::SetRender(MonoObject * object, mono_bool flag)
{
	if (current_game_object != nullptr)
	{
		CompGraphic* item = (CompGraphic*)App->importer->iScript->GetComponentMono(object);
		if (item != nullptr)
		{
			item->SetCanDraw(flag);
		}
	}
}

void CSharpScript::ActivateRender(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		CompGraphic* item = (CompGraphic*)App->importer->iScript->GetComponentMono(object);
		if (item != nullptr)
		{
			item->ActivateRender();
		}
	}
}


void CSharpScript::DeactivateRender(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		CompGraphic* item = (CompGraphic*)App->importer->iScript->GetComponentMono(object);
		if (item != nullptr)
		{
			item->DeactivateRender();
		}
		
	}
}

void CSharpScript::SetAlpha(MonoObject * object, float alpha)
{
	if (current_game_object != nullptr)
	{
		CompGraphic* item = (CompGraphic*)App->importer->iScript->GetComponentMono(object);
		if (item != nullptr)
		{
			item->SetAlpha(alpha);
		}
	}
}

void CSharpScript::SetText(MonoObject * object, MonoString * string)
{
	if (current_game_object != nullptr)
	{
		CompText* text = (CompText*)current_game_object->FindComponentByType(Comp_Type::C_TEXT);
		if (text != nullptr)
		{
			text->SetString(mono_string_to_utf8(string));
		}
	}
}

void CSharpScript::SetColor(MonoObject * object, MonoObject * color, float alpha)
{
	if (current_game_object != nullptr)
	{
		CompImage* image = (CompImage*)current_game_object->FindComponentByType(Comp_Type::C_IMAGE);
		if (image != nullptr)
		{
			MonoClass* classT = mono_object_get_class(color);
			MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
			MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
			MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

			float3 new_vec;

			if (x_field) mono_field_get_value(color, x_field, &new_vec.x);
			if (y_field) mono_field_get_value(color, y_field, &new_vec.y);
			if (z_field) mono_field_get_value(color, z_field, &new_vec.z);

			image->SetColor(new_vec.x, new_vec.y, new_vec.z, alpha);
		}
	}
}

void CSharpScript::SetCanvasAlpha(MonoObject * object, float alpha)
{
	if (current_game_object != nullptr)
	{
		CompCanvas* item = (CompCanvas*)App->importer->iScript->GetComponentMono(object);
		if (item != nullptr)
		{
			item->SetNewCanvasAlphaValue(alpha); 
		}
	}
}
