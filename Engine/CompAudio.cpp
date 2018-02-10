#include "CompAudio.h"
#include "Application.h"
#include "WindowInspector.h"
#include "ModuleGUI.h"
#include "GameObject.h"
#include "Scene.h"

#include "CompTransform.h"
#include "ModuleAudio.h"

CompAudio::CompAudio(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	name_component = "Audio";
	CreateEmitter();
}

CompAudio::CompAudio(const CompAudio & copy, GameObject * parent) : Component(Comp_Type::C_AUDIO, parent)
{
}

CompAudio::~CompAudio()
{
	delete emitter;

	if (audio_type == AUDIO_TYPE::LISTENER)
		App->audio->SetListener(nullptr);
}

void CompAudio::ShowOptions()
{
	//ImGui::MenuItem("CREATE", NULL, false, false);
	if (ImGui::MenuItem("Reset"))
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
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompAudio*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompAudio::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("Audio Options");
	}
	ImGui::PopStyleVar();


	// Button Options --------------------------------------
	if (ImGui::BeginPopup("Audio Options"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}


	//Audio component options -------------------
	int selected_opt = audio_type;
	if (ImGui::Combo("Audio type", &selected_opt, "FX\0Listener\0", 2))
	{
		if (selected_opt == 1 && audio_type != LISTENER)
		{
			audio_type = LISTENER;
			App->audio->SetListener(this);
		}
		else if (selected_opt == 0)
		{
			if(audio_type==LISTENER)
				App->audio->SetListener(nullptr);			
			audio_type = FX;
		}		
	}


	ImGui::TreePop();
}

void CompAudio::CopyValues(const CompAudio * component)
{
}

void CompAudio::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	
}

void CompAudio::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	//...
	Enable();
}

uint CompAudio::GetEmitterID() const
{
	return emitter ? emitter->GetID() : 0;
}

void CompAudio::ResetAudio()
{
	audio_type = AUDIO_TYPE::FX;
}

void CompAudio::CreateEmitter()
{
	unsigned long emitter_id = parent->GetUUID();
	CompTransform* transf = parent->GetComponentTransform();
	float3 pos = transf->GetPosGlobal();
	emitter = Wwished::Utility::CreateEmitter(emitter_id, parent->GetName(), pos.x, pos.y, pos.z);
}
