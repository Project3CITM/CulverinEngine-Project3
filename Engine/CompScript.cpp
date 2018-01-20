#include "CompScript.h"
#include "Application.h"
#include "ResourceScript.h"
#include "ModuleResourceManager.h"
#include "ModuleImporter.h"
#include "ImportScript.h"
#include "CSharpScript.h"
#include "Scene.h"
#include "ModuleFS.h"

CompScript::CompScript(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	uid = App->random->Int();
	nameComponent = "Script";
}

CompScript::CompScript(const CompScript & copy, GameObject * parent) : Component(Comp_Type::C_SCRIPT, parent)
{
	nameComponent = copy.nameScript.c_str();
}

CompScript::~CompScript()
{
	if (resourcescript != nullptr)
	{
		if (resourcescript->NumGameObjectsUseMe > 0)
		{
			resourcescript->NumGameObjectsUseMe--;
		}
	}
	resourcescript = nullptr;
}

void CompScript::Init()
{
	//nameComponent = "New Sript (Script)";
	//nameScript = "New Sript";
	//editor = new Script_editor();
	//editor->Start(nameScript.c_str());
	//editor->SaveScript();
}

void CompScript::preUpdate(float dt)
{
	//Check if have public in script
	//std::string allscript = editor->editor.GetText();
	//size_t firstPublic = allscript.find_first_of("public");
	// Before delete Resource, Set this pointer to nullptr
	if (resourcescript != nullptr)
	{
		if (resourcescript->GetState() == Resource::State::WANTDELETE)
		{
			resourcescript = nullptr;
		}
		else if (resourcescript->GetState() == Resource::State::REIMPORTED)
		{
			uuidResourceReimported = resourcescript->GetUUID();
			resourcescript = nullptr;
		}
	}
	else
	{
		if (uuidResourceReimported != 0)
		{
			resourcescript = (ResourceScript*)App->resource_manager->GetResource(uuidResourceReimported);
			if (resourcescript != nullptr)
			{
				resourcescript->NumGameObjectsUseMe++;

				// Check if loaded
				if (resourcescript->IsCompiled() == Resource::State::UNLOADED)
				{
					if (App->importer->iScript->LoadResource(resourcescript->GetPathAssets().c_str(), resourcescript))
					{
						resourcescript->SetState(Resource::State::LOADED);
					}
					else
					{
						resourcescript->SetState(Resource::State::FAILED);
					}
				}
				uuidResourceReimported = 0;
				if (resourcescript->GetState() != Resource::State::FAILED)
				{
					resourcescript->SetOwnGameObject(parent);
				}
			}
		}
	}
}

void CompScript::Start()
{
	if (resourcescript != nullptr && (App->engineState == EngineState::PLAY || App->engineState == EngineState::PLAYFRAME))
	{
		App->importer->iScript->SetCurrentScript(resourcescript->GetCSharpScript());
		resourcescript->SetCurrentGameObject(parent);
		resourcescript->Start();
	}
}

void CompScript::Update(float dt)
{
	if (resourcescript != nullptr && resourcescript->GetState() == Resource::State::REIMPORTEDSCRIPT)
	{
		resourcescript->LoadValuesGameObject();
		resourcescript->SetOwnGameObject(parent);
	}
	if (resourcescript != nullptr && (App->engineState == EngineState::PLAY || App->engineState == EngineState::PLAYFRAME))
	{
		App->importer->iScript->SetCurrentScript(resourcescript->GetCSharpScript());
		resourcescript->SetCurrentGameObject(parent);
		resourcescript->Update(dt);
	}
}

bool CompScript::CheckAllVariables()
{
	//Access chsharp script, it contains a vector of all variables with their respective info
	for (uint i = 0; i < resourcescript->GetCSharpScript()->variables.size(); i++)
	{
		if (resourcescript->GetCSharpScript()->variables[i]->type == VarType::Var_GAMEOBJECT)
		{
			if (resourcescript->GetCSharpScript()->variables[i]->gameObject == nullptr)
			{
				return false;
			}
		}
	}
	return true;
}

void CompScript::RemoveReferences(GameObject* go)
{
	resourcescript->GetCSharpScript()->RemoveReferences(go);
}

void CompScript::ClearVariables()
{
	if (resourcescript != nullptr)
	{
		resourcescript->GetCSharpScript()->Clear();
	}
}

bool CompScript::CheckScript()
{
	if (resourcescript != nullptr)
	{
		if (resourcescript->IsCompiled() != Resource::State::FAILED &&
			resourcescript->IsCompiled() != Resource::State::UNLOADED)
		{
			if (CheckAllVariables() == false)
			{
				LOG("[error]You have to associate all GameObject of the Script before Run the Game!");
				return false;
			}
			return true;
		}
	}
	return true;
}

void CompScript::ShowOptions()
{
	if (ImGui::MenuItem("Reset", NULL, false, false))
	{
		// Not implmented yet.
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Remove Component"))
	{
		toDelete = true;
	}
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
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
}

void CompScript::ShowInspectorInfo()
{
	// Reset Values Button -------------------------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsScript");
	}

	// Button Options --------------------------------------
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 1.00f));
	if (ImGui::BeginPopup("OptionsScript"))
	{
		/* Reset Material */
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 2));
		if (ImGui::Button("Reset Script"))
		{
			if (resourcescript != nullptr)
			{
				if (resourcescript->NumGameObjectsUseMe > 0)
				{
					resourcescript->NumGameObjectsUseMe--;
				}
			}
			resourcescript = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}

	//ImGui::Text("Script"); ImGui::SameLine();
	ImGui::Selectable("< Edit Script >", false);
	static bool activeScript = false;
	if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
	{
		//LOG("%.2f - %.2f  / /  %.2f - %.2f", ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
		if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringWindow())
		{
			if (resourcescript != nullptr)
			{
				activeScript = !activeScript;
			}
		}
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	if (resourcescript != nullptr)
	{
		static bool active = isActive();
		if (ImGui::Checkbox("Active", &active))
		{
			SetActive(active);
		}

		/* Name of the Script */
		ImGui::Text("NAME:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%s", resourcescript->name);

		ImGui::Spacing();

		ImGui::Text("VARIABLES:");

		//Show info of each variable of the script
		ShowVariablesInfo();

		ImGui::Spacing();

		//Info about ACTIVE/INACTIVE ---------------------------------
		if (isActive())
		{
			ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "SCRIPT ACTIVE");
		}
		else
		{
			ImGui::TextColored(ImVec4(0.0f, 0.837f, 0.453f, 1.0f), "SCRIPT INACTIVE");
		}

	}

	if (resourcescript == nullptr || selectScript)
	{
		if (resourcescript == nullptr)
		{
			if (ImGui::Button("Select Script..."))
			{
				selectScript = true;
			}
		}
		if (selectScript)
		{
			ResourceScript* temp = (ResourceScript*)App->resource_manager->ShowResources(selectScript, Resource::Type::SCRIPT);
			if (temp != nullptr)
			{
				if (resourcescript != nullptr)
				{
					if (resourcescript->NumGameObjectsUseMe > 0)
					{
						resourcescript->NumGameObjectsUseMe--;
					}
				}
				resourcescript = temp;
				resourcescript->NumGameObjectsUseMe++;
				if (resourcescript->IsCompiled() == Resource::State::UNLOADED)
				{
					if (App->importer->iScript->LoadResource(resourcescript->GetPathAssets().c_str(), resourcescript))
					{
						resourcescript->SetState(Resource::State::LOADED);
					}
					else
					{
						resourcescript->SetState(Resource::State::FAILED);
					}
				}
				if (resourcescript->GetState() != Resource::State::FAILED)
				{
					resourcescript->SetOwnGameObject(parent);
				}
				Enable();
			}
		}
	}
	if (activeScript)
	{
		resourcescript->ShowEditor(activeScript);
	}

	ImGui::TreePop();
}

void CompScript::ShowVariablesInfo()
{
	if (resourcescript->GetState() == Resource::State::LOADED)
	{
		//Access chsharp script, it contains a vector of all variables with their respective info
		for (uint i = 0; i < resourcescript->GetCSharpScript()->variables.size(); i++)
		{
			ImGui::PushID(i);

			//Show variable TYPE --------------------------
			ShowVarType(resourcescript->GetCSharpScript()->variables[i]); ImGui::SameLine();

			//Show variable NAME -------------------------
			ImGui::Text(" %s", resourcescript->GetCSharpScript()->variables[i]->name); ImGui::SameLine();

			//Show variable VALUE -------------------------
			ShowVarValue(resourcescript->GetCSharpScript()->variables[i], i);

			ImGui::PopID();
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f,0.223f,0.233f,1.0f),"SCRIPT NOT COMPILED CORRECTLY");
	}
}

void CompScript::ShowVarType(ScriptVariable* var)
{
	if (var->type == VarType::Var_INT)
	{
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f),"INT");
	}
	else if (var->type == VarType::Var_FLOAT)
	{
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "FLOAT");
	}
	else if (var->type == VarType::Var_BOOL)
	{
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "BOOL");
	}
	else if (var->type == VarType::Var_STRING)
	{	
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "STRING");
	}
	else if (var->type == VarType::Var_GAMEOBJECT)
	{
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "GO");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "UNKNOWN TYPE");
	}
}

void CompScript::ShowVarValue(ScriptVariable* var, int pushi)
{
	if (var->type == VarType::Var_INT)
	{
		if (ImGui::InputInt("##iVal", &*(int*)var->value))
		{
			int val = *(int*)var->value;
			var->SetMonoValue((int*)var->value);
		}
	}
	else if (var->type == VarType::Var_FLOAT)
	{
		if (ImGui::InputFloat("##fVal", &*(float*)var->value, 0, 0, 4))
		{
			float val = *(float*)var->value;
			var->SetMonoValue((float*)var->value);
		}
	}
	else if (var->type == VarType::Var_BOOL)
	{
		if (ImGui::Checkbox("##bVal", &*(bool*)var->value))
		{
			bool val = *(bool*)var->value;
			var->SetMonoValue((bool*)var->value);
		}
	}
	else if (var->type == VarType::Var_STRING)
	{
		//std::string strval = (const char*)var->value;
		//if (ImGui::InputText("##strVal", &*(char*)var->value, 50))
		//{
		//	var->SetMonoValue((char*)var->value);
		//}
		ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "%s", var->str_value.c_str());
	}
	else if (var->type == VarType::Var_GAMEOBJECT)
	{
		if (var->gameObject == nullptr)
		{
			ImGui::PushID(pushi);
			if (ImGui::Button("Select GO..."))
			{
				var->selectGameObject = true;
			}

			if (var->selectGameObject)
			{
				GameObject* temp = App->scene->GetGameObjectfromScene(var->selectGameObject);
				if (temp != nullptr)
				{
					var->gameObject = temp;
					var->SetMonoValue((GameObject*)var->gameObject);
				}
			}
			ImGui::PopID();
		}
		else
		{
			if (!var->gameObject->WanttoDelete())
			{
				ImGui::Text("%s", var->gameObject->GetName()); ImGui::SameLine();
				if (App->engineState != EngineState::PLAY)
				{
					if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
					{
						var->EreaseMonoValue(var->gameObject);
						var->selectGameObject = true;
					}
				}
			}
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "UNKNOWN VALUE");
	}
}

void CompScript::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_number_with_std(object, name + "Type", Comp_Type::C_SCRIPT);
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	if (resourcescript != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Resource Script UUID", resourcescript->GetUUID());
		// Now Save Info in CSharp
		resourcescript->Save(object, name);
	}
	json_object_dotset_string_with_std(object, name + "Name Script", nameScript.c_str());
}

void CompScript::Load(const JSON_Object* object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	nameScript = json_object_dotget_string_with_std(object, name + "Name Script");
	uint resourceID = uid = json_object_dotget_number_with_std(object, name + "Resource Script UUID");
	//std::string temp = nameScript + " (Script)";
	//nameComponent = "Script";
	//editor = new Script_editor();
	//editor->Start(nameScript.c_str(), false);
	if (resourceID > 0)
	{
		resourcescript = (ResourceScript*)App->resource_manager->GetResource(resourceID);
		if (resourcescript != nullptr)
		{
			resourcescript->NumGameObjectsUseMe++;

			// LOAD SCRIPT -------------------------
			if (resourcescript->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iScript->LoadResource(resourcescript->GetPathAssets().c_str(), resourcescript);
			}
			resourcescript->Load(object, name);
			if (resourcescript->GetState() != Resource::State::FAILED)
			{
				resourcescript->SetOwnGameObject(parent);
			}

		}
	}
	Enable();
}