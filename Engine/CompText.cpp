#include "CompText.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "ResourceFont.h"
#include "SDL2_ttf/include/SDL_ttf.h"


CompText::CompText(Comp_Type t, GameObject * parent) :CompGraphic(t, parent)
{
	uid = App->random->Int();
	name_component = "Text";


}

CompText::CompText(const CompText & copy, GameObject * parent) : CompGraphic(Comp_Type::C_TEXT, parent)
{
}

CompText::~CompText()
{
}

void CompText::PreUpdate(float dt)
{
	// Manage Resource -------------------------------------------------
	// Before delete Resource Set this pointer to nullptr
	if (text != nullptr)
	{
		if (text->GetState() == Resource::State::WANTDELETE)
		{
			text = nullptr;
		}
		else if (text->GetState() == Resource::State::REIMPORTED)
		{
		//	uuid_resource_reimported = resource_font->GetUUID();
			text = nullptr;
		}
	}
	else
	{
		/*
		if (uuid_resource_reimported != 0)
		{
			resource_font = (ResourceMesh*)App->resource_manager->GetResource(uuid_resource_reimported);
			if (resource_font != nullptr)
			{
				resource_font->num_game_objects_use_me++;
				// Check if loaded!
				if (resource_font->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMesh->LoadResource(std::to_string(resource_font->GetUUID()).c_str(), resource_font);
				}
				uuid_resource_reimported = 0;
			}
		}
		*/
	}
	// -------------------------------------------------------------------
}
void CompText::ShowOptions()
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
			CopyValues(((CompText*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompText::ShowInspectorInfo()
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

	ImGui::TreePop();
}

void CompText::SetRect(float x, float y, float width, float height)
{
	text_rect.x = x;
	text_rect.y = y;
	text_rect.width = width;
	text_rect.height = height;
}

void CompText::SetString(std::string input)
{
	text_str = input;
	UpdateText();
}

void CompText::UpdateText()
{
	if (!text->font || text_str.empty())
		return;
	else if (s_font != NULL && text_str.empty())
	{
		FreeFont();
		return;
	}
	if (s_font != NULL)
	{
		FreeFont();
	}
	update_text = true;

	s_font = TTF_RenderText_Blended(text->font, text_str.c_str(), SDL_Color{ (Uint8)(color.x * 255), (Uint8)(color.y * 255),(Uint8)(color.z * 255), (Uint8)(color.w * 255) });

	GLuint texture;
	glGenTextures(1, &uid);
	glBindTexture(GL_TEXTURE_2D, uid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s_font->w, s_font->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, s_font->pixels);
	SetRect(0.0f, 0.0f, s_font->w, s_font->h);


	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(s_font);

}

void CompText::FreeFont()
{
	glDeleteTextures(1, &uid);
}

void CompText::CopyValues(const CompText * component)
{
}

void CompText::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
}

void CompText::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	//...
	Enable();
}
