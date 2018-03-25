#include "CompScript.h"
#include "Application.h"
#include "ResourceScript.h"
#include "ModuleResourceManager.h"
#include "ModuleImporter.h"
#include "ImportScript.h"
#include "CSharpScript.h"
#include "Scene.h"
#include "ModuleFS.h"
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "WindowInspector.h"

CompScript::CompScript(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Script";
}

CompScript::CompScript(const CompScript & copy, GameObject * parent) : Component(Comp_Type::C_SCRIPT, parent)
{
	name_component = copy.name_script.c_str();
}

CompScript::~CompScript()
{
	if (resource_script != nullptr)
	{
		if (resource_script->num_game_objects_use_me > 0)
		{
			resource_script->num_game_objects_use_me--;
		}
		resource_script = nullptr;
	}
}

void CompScript::Init()
{
	//nameComponent = "New Sript (Script)";
	//nameScript = "New Script";
	//editor = new Script_editor();
	//editor->Start(nameScript.c_str());
	//editor->SaveScript();
}

void CompScript::PreUpdate(float dt)
{
	//Check if have public in script
	//std::string allscript = editor->editor.GetText();
	//size_t firstPublic = allscript.find_first_of("public");
	// Before delete Resource, Set this pointer to nullptr
	if (p_active)
	{
		if (csharp != nullptr)
		{
			if (resource_script != nullptr && (App->engine_state == EngineState::PLAY || App->engine_state == EngineState::PLAYFRAME))
			{
				csharp->DoMainFunction(FunctionBase::CS_OnEnable);
			}
		}
		p_active = false;
	}
	if (do_start)
	{
		if (csharp != nullptr)
		{
			if (resource_script != nullptr && (App->engine_state == EngineState::PLAY || App->engine_state == EngineState::PLAYFRAME))
			{
				csharp->DoMainFunction(FunctionBase::CS_Start);
			}
		}
		do_start = false;
	}
	if (resource_script != nullptr)
	{
		if (resource_script->GetState() == Resource::State::WANTDELETE)
		{
			resource_script = nullptr;
		}
		else if (resource_script->GetState() == Resource::State::REIMPORTED)
		{
			uuid_resource_reimported = resource_script->GetUUID();
			resource_script = nullptr;
		}
	}
	else
	{
		if (uuid_resource_reimported != 0)
		{
			resource_script = (ResourceScript*)App->resource_manager->GetResource(uuid_resource_reimported);
			if (resource_script != nullptr)
			{
				resource_script->num_game_objects_use_me++;

				// Check if loaded
				if (resource_script->IsCompiled() == Resource::State::UNLOADED)
				{
					if (App->importer->iScript->LoadResource(resource_script->GetPathAssets().c_str(), resource_script))
					{
						resource_script->SetState(Resource::State::LOADED);
					}
					else
					{
						resource_script->SetState(Resource::State::FAILED);
					}
				}
				uuid_resource_reimported = 0;
				if (resource_script->GetState() != Resource::State::FAILED)
				{
					csharp = App->importer->iScript->LoadScript_CSharp(resource_script->GetPathdll(), resource_script->name);
					SetOwnGameObject(parent);
				}
			}
		}
	}
}

void CompScript::Start()
{
	if (resource_script != nullptr && (App->engine_state == EngineState::PLAY || App->engine_state == EngineState::PLAYFRAME))
	{
		if (csharp != nullptr)
		{
			LoadValuesGameObjectScript();
		}
		App->importer->iScript->SetCurrentScript(csharp);
		SetCurrentGameObject(parent);
		StartScript();
	}
}

void CompScript::Update(float dt)
{
	if (p_active == false && do_start == false)
	{
		// Link GameObject* variables of the script
		if (resource_script != nullptr && resource_script->GetState() == Resource::State::REIMPORTEDSCRIPT)
		{
			SetOwnGameObject(parent);
		}
		if (resource_script != nullptr && (App->engine_state == EngineState::PLAY || App->engine_state == EngineState::PLAYFRAME))
		{
			App->importer->iScript->SetCurrentScript(csharp);
			//SetCurrentGameObject(parent);
			UpdateScript(dt);
		}
	}
}

void CompScript::postUpdate()
{
	if (csharp != nullptr)
	{
		if (resource_script != nullptr && (App->engine_state == EngineState::PLAY || App->engine_state == EngineState::PLAYFRAME))
		{
			csharp->DoMainFunction(FunctionBase::CS_OnDisable);
		}
	}
}

void CompScript::Clear()
{
	if (resource_script != nullptr && csharp != nullptr)
	{
		if (resource_script->num_game_objects_use_me > 0)
		{
			resource_script->num_game_objects_use_me--;
		}
		resource_script = nullptr;
		csharp->Clear();
	}
	// Clear all Actions (UI Button) -----------------
}

bool CompScript::CheckAllVariables()
{
	//Access chsharp script, it contains a vector of all variables with their respective info
	if (csharp != nullptr)
	{
		for (uint i = 0; i < csharp->variables.size(); i++)
		{
			if (csharp->variables[i]->type == VarType::Var_GAMEOBJECT)
			{
				if (csharp->variables[i]->game_object == nullptr)
				{
					return false;
				}
			}
		}
	}
	return true;
}

void CompScript::RemoveReferences(GameObject* go)
{
	if (csharp != nullptr)
	{
		csharp->RemoveReferences(go);
	}
}

const char * CompScript::GetScriptName() const
{
	return name_script.c_str();
}

void CompScript::SetCurrentGameObject(GameObject* current)
{
	if (csharp != nullptr)
	{
		csharp->SetCurrentGameObject(current);
	}
}

void CompScript::SetOwnGameObject(GameObject* owenerofScript)
{
	if (csharp != nullptr)
	{
		csharp->SetOwnGameObject(owenerofScript);
	}
}

void CompScript::SetCSharp(CSharpScript* csharp_)
{
	csharp = csharp_;
}

bool CompScript::StartScript()
{
	if (csharp != nullptr)
	{
		csharp->DoMainFunction(FunctionBase::CS_Start);
		return true;
	}
	return false;
}

bool CompScript::UpdateScript(float dt)
{
	if (csharp != nullptr)
	{
		csharp->DoMainFunction(FunctionBase::CS_Update);
		return true;
	}
	return false;
}

bool CompScript::CheckScript()
{
	if (resource_script != nullptr)
	{
		if (resource_script->IsCompiled() != Resource::State::FAILED &&
			resource_script->IsCompiled() != Resource::State::UNLOADED)
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
		to_delete = true;
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
			CopyValues(((CompScript*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Reset Script"))
	{
		if (resource_script != nullptr)
		{
			if (resource_script->num_game_objects_use_me > 0)
			{
				resource_script->num_game_objects_use_me--;
			}
		}
		resource_script = nullptr;
		ImGui::CloseCurrentPopup();
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
	if (ImGui::BeginPopup("OptionsScript"))
	{
		ShowOptions();
		ImGui::EndPopup();

	}

	ImGui::PopStyleVar();
	ShowFSMInspectorInfo();

	// Show Editor Actions (UI Button) ----------------------------
	ImGui::Separator();
	ImGui::Text("");
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	int space_with_no_actions = 0;
	if (actions.size() == 0)
	{
		space_with_no_actions = 96;
	}
	if (ImGui::BeginChild("Inspector", ImVec2(ImGui::GetWindowWidth() - 40, 96 * actions.size() + space_with_no_actions + 19), true, ImGuiWindowFlags_NoScrollWithMouse))
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.481f, 0.481f, 0.481f, 1.00f));
		if (ImGui::BeginChild(ImGui::GetID("Inspector"), ImVec2(ImGui::GetWindowWidth(), 19), false, ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::Text("  On Click ()");
			ImGui::Separator();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		if (actions.size() == 0)
		{
			ImGui::Text("  List is Empty ");
		}
		static std::string mode[] = { "Off", "Runtime Only" };
		for (int i = 0; i < actions.size(); i++)
		{
			ImGui::PushID(i * actions.size());
			if (i > 0)
				ImGui::Separator();
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.311f, 0.311f, 0.311f, 1.00f));
			ImGui::BeginChild("Inspector", ImVec2(ImGui::GetWindowWidth(), 90), false, ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushItemWidth(130);
			if (ImGui::BeginCombo("##Mode", actions[i].current_mode.c_str()))
			{
				for (int n = 0; n < IM_ARRAYSIZE(mode); n++)
				{
					if (ImGui::Selectable(mode[n].c_str()))
					{
						actions[i].selected_mode = n;
						actions[i].current_mode = mode[n];
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (actions[i].attacked == nullptr)
			{
				ImGui::PushItemWidth(130);
				if (ImGui::BeginCombo("##Function", "No Script"))
				{
					ImGui::EndCombo();
				}
				ImGui::PopItemWidth();
			}
			else
			{
				ImGui::PushItemWidth(130);
				if (ImGui::BeginCombo("##SelectScript", actions[i].current_script.c_str()))
				{
					for (int id = -1; id < actions[i].attacked->GetNumComponents(); id++)
					{
						if (id == -1)
						{
							if (ImGui::Selectable("No Script"))
							{
								actions[i].script = nullptr;
								actions[i].current_script = "No Script";
								actions[i].current_function = "No Function";
								actions[i].method = nullptr;
								actions[i].value_go = nullptr;
								RELEASE_ARRAY(actions[i].value);
								ImGui::SetItemDefaultFocus();
							}
							//ImGui::Separator();
						}
						else if (actions[i].attacked->GetComponentbyIndex(id)->GetType() == Comp_Type::C_SCRIPT)
						{
							if (ImGui::Selectable(((CompScript*)actions[i].attacked->GetComponentbyIndex(id))->GetScriptName()))
							{
								actions[i].script = (CompScript*)actions[i].attacked->GetComponentbyIndex(id);
								actions[i].current_script = ((CompScript*)actions[i].attacked->GetComponentbyIndex(id))->GetScriptName();
								ImGui::SetItemDefaultFocus();
							}
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopItemWidth();
			}
			ImGui::Text("   GameObject: "); ImGui::SameLine();
			if (actions[i].attacked == nullptr)
			{
				if (ImGui::Button("Select GO..."))
				{
					actions[i].select_game_object = true;
				}

				if (actions[i].select_game_object)
				{
					GameObject* temp = App->scene->GetGameObjectfromScene(actions[i].select_game_object);
					if (temp != nullptr)
					{
						actions[i].attacked = temp;
						actions[i].select_game_object = false;
					}
				}
			}
			else
			{
				ImGui::Text("   %s", actions[i].attacked->GetName()); ImGui::SameLine();
				if (App->engine_state != EngineState::PLAY)
				{
					if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
					{
						actions[i].select_game_object = true;
						actions[i].attacked = nullptr;
						actions[i].method = nullptr;
						actions[i].script = nullptr;
						actions[i].current_function = "No Function";
						actions[i].current_script = "No Script";
						actions[i].value_go = nullptr;
						RELEASE_ARRAY(actions[i].value);
					}
				}
				ImGui::PushItemWidth(130);
				if (actions[i].script == nullptr)
				{
					ImGui::PopItemWidth();
					ImGui::EndChild();
					ImGui::PopStyleColor();
					ImGui::PopID();
					continue;
				}
				if (ImGui::BeginCombo("##SelectFunction", actions[i].current_function.c_str()))
				{
					for (int id = 0; id < actions[i].script->csharp->methods.size(); id++)
					{
						if (ImGui::Selectable(actions[i].script->csharp->methods[id].name_method.c_str()) &&
							&actions[i].script->csharp->methods[id] != actions[i].method)
						{
							actions[i].current_function = actions[i].script->csharp->methods[id].name_method;
							actions[i].method = &actions[i].script->csharp->methods[id];
							RELEASE_ARRAY(actions[i].value);
							if(actions[i].method->type != VarType::Var_NONE)
								InitValueParamater(i);
						}
					}
					
					ImGui::EndCombo();
				}
				ImGui::PopItemWidth();
				if (actions[i].method == nullptr)
				{
					ImGui::EndChild();
					ImGui::PopStyleColor();
					ImGui::PopID();
					continue;
				}
				ImGui::SameLine();
				// Show Type by Method (int, float, string, bool, gameobject)
				ShowTypeMethod(i);
				ImGui::Text("%s", actions[i].method->name_param.c_str()); ImGui::SameLine();
				// Now Set Value with type param.
				switch (actions[i].method->type)
				{
				case VarType::Var_INT:
				{
					if (ImGui::InputInt("##iVal", &*(int*)actions[i].value))
					{
						int val = *(int*)actions[i].value;
					}
					break;
				}
				case VarType::Var_FLOAT:
				{
					if (ImGui::InputFloat("##fVal", &*(float*)actions[i].value, 0, 0, 4))
					{
						float val = *(float*)actions[i].value;
					}
					break;
				}
				case VarType::Var_BOOL:
				{
					if (ImGui::Checkbox("##bVal", &*(bool*)actions[i].value))
					{
						bool val = *(bool*)actions[i].value;
					}
					break;
				}
				case VarType::Var_STRING:
				{
					ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "...");
					break;
				}
				case VarType::Var_GAMEOBJECT:
				{

					break;
				}
				}

			}

			ImGui::EndChild();
			ImGui::PopStyleColor();
			ImGui::PopID();
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Text(""); ImGui::SameLine(ImGui::GetWindowWidth() - 70);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
	ImGui::BeginChild(ImGui::GetID("Inspector"), ImVec2(60, 20), true, ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.612f, 0.612f, 0.612f, 1.0f));
	if (ImGui::ImageButton((ImTextureID*)App->gui->icon_plus, ImVec2(8, 8), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		Actions new_action;
		actions.push_back(new_action);
	}
	ImGui::SameLine();
	if (ImGui::ImageButton((ImTextureID*)App->gui->icon_remove, ImVec2(8, 8), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		Actions new_action;
		actions.pop_back();
	}
	ImGui::PopStyleColor();
	ImGui::EndChild();
	ImGui::PopStyleVar();
	// Andre how to call functions :D
	// actions[i].script->csharp->DoPublicMethod(actions[i].method, &actions[i].value);
	ImGui::TreePop();
	
}

// (UI Button) -----------------------------
void CompScript::InitValueParamater(int index)
{
	// Now Init Value with type param.
	switch (actions[index].method->type)
	{
	case VarType::Var_INT:
	{
		int value = 0;
		actions[index].value = new char[sizeof value];
		*(int*)actions[index].value = value;
		break;
	}
	case VarType::Var_FLOAT:
	{
		float value = 0.0f;
		actions[index].value = new char[sizeof value];
		*(float*)actions[index].value = value;
		break;
	}
	case VarType::Var_BOOL:
	{
		bool value = false;
		actions[index].value = new char[sizeof value];
		*(bool*)actions[index].value = value;
		break;
	}
	case VarType::Var_STRING:
	{
		ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "Value");
		break;
	}
	case VarType::Var_GAMEOBJECT:
	{

		break;
	}
	}
}

// (UI Button) -----------------------------
void CompScript::ShowTypeMethod(int index)
{
	switch (actions[index].method->type)
	{
	case VarType::Var_INT:
	{
		ImGui::Text("Type: Int()");
		break;
	}
	case VarType::Var_FLOAT:
	{
		ImGui::Text("Type: Float()");
		break;
	}
	case VarType::Var_BOOL:
	{
		ImGui::Text("Type: Bool()");
		break;
	}
	case VarType::Var_STRING:
	{
		ImGui::Text("Type: String()");
		break;
	}
	case VarType::Var_GAMEOBJECT:
	{
		ImGui::Text("Type: GameObject()");
		break;
	}
	}
}

void CompScript::ShowVariablesInfo()
{
	if (resource_script->GetState() == Resource::State::LOADED && csharp != nullptr)
	{
		//Access chsharp script, it contains a vector of all variables with their respective info
		for (uint i = 0; i < csharp->variables.size(); i++)
		{
			ImGui::PushID(i);
			if (csharp->variables[i]->access == VarAccess::Var_PUBLIC)
			{
				//Show variable TYPE --------------------------
				ShowVarType(csharp->variables[i]); ImGui::SameLine();

				//Show variable NAME -------------------------
				ImGui::Text(" %s", csharp->variables[i]->name); ImGui::SameLine();

				//Show variable VALUE -------------------------
				ShowVarValue(csharp->variables[i], i);
			}

			ImGui::PopID();
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.223f, 0.233f, 1.0f), "SCRIPT NOT COMPILED CORRECTLY");
	}
}

void CompScript::ShowFSMInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 1.00f));
	if (resource_script != nullptr)
	{
		char buffer[50];
		sprintf(buffer, "< Edit Script >##%i", uid);
		ImGui::Selectable(buffer, false);
		if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
		{
			//LOG("%.2f - %.2f  / /  %.2f - %.2f", ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsMouseHoveringWindow())
			{
				if (resource_script != nullptr)
				{
					active_script = !active_script;
				}
			}
		}
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	if (resource_script == nullptr)
	{
		ImGui::Text("NAME:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "None (Script)");
		char creat_string[50];
		sprintf(creat_string, "Create New Script##%i", uid);
		if (ImGui::Button(creat_string))
		{
			App->gui->ShowCreateNewScriptWindow();
		}
	}

	if (resource_script != nullptr)
	{
		if (ImGui::Checkbox("Active", &this->active))
		{
			SetActive(active);
		}

		/* Name of the Script */
		ImGui::Text("NAME:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%s", resource_script->name.c_str());

		ImGui::Spacing();

		ImGui::Text("VARIABLES:");

		//Show info of each variable of the script
		ShowVariablesInfo();

		ImGui::Spacing();

		//Info about ACTIVE/INACTIVE ---------------------------------
		if (IsActive())
		{
			ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "SCRIPT ACTIVE");
		}
		else
		{
			ImGui::TextColored(ImVec4(0.0f, 0.837f, 0.453f, 1.0f), "SCRIPT INACTIVE");
		}

	}

	if (resource_script == nullptr || select_script)
	{
		if (resource_script == nullptr)
		{
			char select_str[50];
			sprintf(select_str, "Select Script...##%i", uid);
			if (ImGui::Button(select_str))
			{
				select_script = true;
			}
		}
		if (select_script)
			SelectScript(select_script);
	}
	if (active_script && resource_script != nullptr)
	{
		resource_script->ShowEditor(active_script);
	}
}

void CompScript::ShowVarType(ScriptVariable* var)
{
	if (var->type == VarType::Var_INT)
	{
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "INT");
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
		if (var->game_object == nullptr)
		{
			ImGui::PushID(pushi);
			if (ImGui::Button("Select GO..."))
			{
				var->select_game_object = true;
			}

			if (var->select_game_object)
			{
				GameObject* temp = App->scene->GetGameObjectfromScene(var->select_game_object);
				if (temp != nullptr)
				{
					var->game_object = temp;
					var->SetMonoValue((GameObject*)var->game_object);
				}
			}
			ImGui::PopID();
		}
		else
		{
			if (!var->game_object->WanttoDelete())
			{
				ImGui::Text("%s", var->game_object->GetName()); ImGui::SameLine();
				if (App->engine_state != EngineState::PLAY)
				{
					if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
					{
						var->EreaseMonoValue(var->game_object);
						var->select_game_object = true;
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

void CompScript::AddScriptbyName(const char* name_script)
{
	if (name_script != nullptr)
	{
		resource_script = (ResourceScript*)App->resource_manager->GetResource(name_script);
		if (resource_script != nullptr)
		{
			resource_script->num_game_objects_use_me++;

			// LOAD SCRIPT -------------------------
			if (resource_script->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iScript->LoadResource(resource_script->GetPathAssets().c_str(), resource_script);
			}

			if (resource_script->GetState() != Resource::State::FAILED)
			{
				csharp = App->importer->iScript->LoadScript_CSharp(resource_script->GetPathdll(), resource_script->name);
				SetOwnGameObject(parent);
			}
		}
	}
}

void CompScript::CopyValues(const CompScript* component)
{
	//more...
}

bool CompScript::SelectScript(bool& selecting)
{
	ResourceScript* temp = (ResourceScript*)App->resource_manager->ShowResources(selecting, Resource::Type::SCRIPT);
	if (temp != nullptr)
	{
		if (resource_script != nullptr)
		{
			if (resource_script->num_game_objects_use_me > 0)
			{
				resource_script->num_game_objects_use_me--;
			}
		}

		//Link the Resource to the Component
		resource_script = temp;
		resource_script->num_game_objects_use_me++;
		name_script = resource_script->name;

		if (resource_script->IsCompiled() == Resource::State::UNLOADED)
		{
			if (App->importer->iScript->LoadResource(resource_script->GetPathAssets().c_str(), resource_script))
			{
				resource_script->SetState(Resource::State::LOADED);
			}
			else
			{
				resource_script->SetState(Resource::State::FAILED);
			}
		}
		if (resource_script->GetState() != Resource::State::FAILED)
		{
			csharp = App->importer->iScript->LoadScript_CSharp(resource_script->GetPathdll(), resource_script->name);
			SetOwnGameObject(parent);
		}
		Enable();
		return true;
	}
	return false;
}

void CompScript::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", Comp_Type::C_SCRIPT);
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	json_object_dotset_string_with_std(object, name + "Name Script", strcmp(name_script.c_str(), "") == 0 ? "Empty Script" : name_script.c_str());
	if (resource_script != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Resource Script UUID", resource_script->GetUUID());
		// Now Save Info in CSharp
		App->fs->NormalitzatePath(name);
		SaveScript(object, name);
	}

	// Save Actions (UI Button) ------------------------
	if (actions.size() > 0)
	{
		json_object_dotset_number_with_std(object, name + "Actions.Number of Actions", actions.size());
		for (int i = 0; i < actions.size(); i++)
		{
			std::string action_temp = name + "Actions.Action " + std::to_string(i) + ".";
			json_object_dotset_number_with_std(object, action_temp + "Mode", actions[i].selected_mode);
			if (actions[i].attacked == nullptr)
			{
				json_object_dotset_number_with_std(object, action_temp + "GameObject", -1);
				continue;
			}
			json_object_dotset_number_with_std(object, action_temp + "GameObject", actions[i].attacked->GetUUID());
			if (actions[i].script == nullptr)
			{
				json_object_dotset_string_with_std(object, action_temp + "Name Script", "No Script");
				continue;
			}
			json_object_dotset_string_with_std(object, action_temp + "Name Script", actions[i].script->GetScriptName());
			if (actions[i].method == nullptr)
			{
				json_object_dotset_string_with_std(object, action_temp + "Name Method", "No Function");
				continue;
			}
			json_object_dotset_string_with_std(object, action_temp + "Name Method", actions[i].method->name_method.c_str());
			switch (actions[i].method->type)
			{
			case VarType::Var_INT:
			{
				json_object_dotset_number_with_std(object, action_temp + "Value", *(int*)actions[i].value);
				break;
			}
			case VarType::Var_FLOAT:
			{
				json_object_dotset_number_with_std(object, action_temp + "Value", *(float*)actions[i].value);
				break;
			}
			case VarType::Var_BOOL:
			{
				json_object_dotset_number_with_std(object, action_temp + "Value", *(bool*)actions[i].value);
				break;
			}
			case VarType::Var_STRING:
			{
				json_object_dotset_string_with_std(object, action_temp + "Value", actions[i].value_string.c_str());
				break;
			}
			case VarType::Var_GAMEOBJECT:
			{
				json_object_dotset_number_with_std(object, action_temp + "Value", actions[i].value_go->GetUUID());
				break;
			}
			}
		}
	}
}

void CompScript::Load(const JSON_Object* object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	name_script = json_object_dotget_string_with_std(object, name + "Name Script");
	uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Script UUID");

	if (resourceID > 0)
	{
		resource_script = (ResourceScript*)App->resource_manager->GetResource(resourceID);
		if (resource_script != nullptr)
		{
			resource_script->num_game_objects_use_me++;

			// LOAD SCRIPT -------------------------
			if (resource_script->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				resource_script->SetState(Resource::State::LOADED);
				//App->importer->iScript->LoadResource(resource_script->GetPathAssets().c_str(), resource_script);
			}

			if (resource_script->GetState() != Resource::State::FAILED)
			{
				csharp = App->importer->iScript->LoadScript_CSharp(resource_script->GetPathdll(), resource_script->name);
				LoadScript(object, name);
				SetOwnGameObject(parent);
			}
		}
	}
	Enable();
}

void CompScript::SyncComponent(GameObject * sync_parent)
{
	if (csharp != nullptr && csharp->NeedToLinkGO())
	{
		csharp->LoadValuesGO(sync_parent);
	}
}

void CompScript::LoadValuesGameObjectScript()
{
	if (csharp != nullptr && csharp->NeedToLinkGO())
	{
		csharp->LoadValuesGO(nullptr);
	}
}

void CompScript::SaveScript(JSON_Object * object, std::string name) const
{
	//Save Values
	if (csharp != nullptr)
	{
		csharp->Save(object, name);
	}
}

void CompScript::LoadScript(const JSON_Object* object, std::string name)
{
	//Load Values
	if (csharp != nullptr)
	{
		csharp->Load(object, name);
	}
}

