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

void CSharpScript::ActivateRender(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		CompImage* image = (CompImage*)current_game_object->FindComponentByType(Comp_Type::C_IMAGE);
		if (image != nullptr)
		{
			image->ActivateRender();
		}
	}
}


void CSharpScript::DeactivateRender(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		CompImage* image = (CompImage*)current_game_object->FindComponentByType(Comp_Type::C_IMAGE);
		if (image != nullptr)
		{
			image->DeactivateRender();
		}
	}
}

void CSharpScript::SetAlpha(MonoObject * object, float alpha)
{
	if (current_game_object != nullptr)
	{
		CompText* text = (CompText*)current_game_object->FindComponentByType(Comp_Type::C_TEXT);
		if (text != nullptr)
		{
			text->SetAlpha(alpha);
		}
	}
}
