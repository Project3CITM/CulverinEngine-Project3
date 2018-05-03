#include "ClickAction.h"
#include "ModuleGUI.h"
#include "Application.h"
#include "Component.h"
#include "ModuleSceneIntro.h"
#include "Scene.h"
#include "CSharpScript.h"
#include "ResourceScript.h"
#include "GameObject.h"
#include "CompScript.h"
#include "ModuleFS.h"
#include "JSONSerialization.h"

ClickAction::ClickAction()
{
}


ClickAction::~ClickAction()
{
}
void ClickAction::ShowOnClickInfo()
{
	//ShowFSMInspectorInfo();

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
					if (actions[i].script->csharp != nullptr)
					{
					
					for (int id = 0; id < actions[i].script->csharp->methods.size(); id++)
					{
						if (ImGui::Selectable(actions[i].script->csharp->methods[id].name_method.c_str()) &&
							&actions[i].script->csharp->methods[id] != actions[i].method)
						{
							actions[i].current_function = actions[i].script->csharp->methods[id].name_method;
							actions[i].method = &actions[i].script->csharp->methods[id];
							RELEASE_ARRAY(actions[i].value);
							if (actions[i].method->type != VarType::Var_NONE)
								InitValueParamater(i);
						}
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
		ClickActionData new_action;
		actions.push_back(new_action);
	}
	ImGui::SameLine();
	if (ImGui::ImageButton((ImTextureID*)App->gui->icon_remove, ImVec2(8, 8), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ClickActionData new_action;
		actions.pop_back();
	}
	ImGui::PopStyleColor();
	ImGui::EndChild();
	ImGui::PopStyleVar();
	// Andre how to call functions :D
	// actions[i].script->csharp->DoPublicMethod(actions[i].method, &actions[i].value);


}
void ClickAction::ShowTypeMethod(int index)
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

void ClickAction::ShowVariablesInfo()
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
void ClickAction::ShowVarType(ScriptVariable* var)
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

void ClickAction::ShowVarValue(ScriptVariable* var, int pushi)
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
// (UI Button) -----------------------------
void ClickAction::InitValueParamater(int index)
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

void ClickAction::SaveClickAction(JSON_Object* object, std::string name)const
{
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
			json_object_dotset_number_with_std(object, action_temp + "Type Method", (int)actions[i].method->type);
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
				json_object_dotset_boolean_with_std(object, action_temp + "Value", *(bool*)actions[i].value);

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

void ClickAction::LoadClickAction(const JSON_Object * object, std::string name)
{
	// Load Actions (UI Button) ------------------------
	uint number_actions = json_object_dotget_number_with_std(object, name + "Actions.Number of Actions");
	for (int i = 0; i < number_actions; i++)
	{
		std::string action_temp = name + "Actions.Action " + std::to_string(i) + ".";
		ClickActionData temp;

		temp.selected_mode = json_object_dotget_number_with_std(object, action_temp + "Mode");
		temp.UID_attacked = json_object_dotget_number_with_std(object, action_temp + "GameObject");
		if (temp.UID_attacked == -1)
		{
			actions.push_back(temp);
			continue;
		}
		temp.current_script = json_object_dotget_string_with_std(object, action_temp + "Name Script");
		if (temp.current_script == "No Script")
		{
			actions.push_back(temp);
			continue;
		}
		temp.current_function = json_object_dotget_string_with_std(object, action_temp + "Name Method");
		if (temp.current_function == "No Function")
		{
			actions.push_back(temp);
			continue;
		}
		VarType type_function = (VarType)(int)json_object_dotget_number_with_std(object, action_temp + "Type Method");
		if (type_function != VarType::Var_NONE)
			//InitValueParamater();
			switch (type_function)
			{
			case VarType::Var_INT:
			{
				int value = json_object_dotget_number_with_std(object, action_temp + "Value");
				temp.value = new char[sizeof value];
				*(int*)temp.value = value;
				break;
			}
			case VarType::Var_FLOAT:
			{
				float value = json_object_dotget_number_with_std(object, action_temp + "Value");
				temp.value = new char[sizeof value];
				*(float*)temp.value = value;
				break;
			}
			case VarType::Var_BOOL:
			{
				bool value = json_object_dotget_boolean_with_std(object, action_temp + "Value");
				temp.value = new char[sizeof value];
				*(bool*)temp.value = value;
				break;
			}
			case VarType::Var_STRING:
			{
				break;
			}
			case VarType::Var_GAMEOBJECT:
			{
				// int value_go = json_object_dotget_number_with_std(object, action_temp + "Value");
				break;
			}
		}
		actions.push_back(temp);

	}


}

void ClickAction::GetOwnBufferSize(uint & buffer_size)
{
	// Save Actions (UI Button) ------------------------
	if (actions.size() > 0)
	{
		buffer_size += sizeof(int);										//actions.size()
		for (int i = 0; i < actions.size(); i++)
		{
			buffer_size += sizeof(int);									//actions[i].selected_mode
			if (actions[i].attacked == nullptr)
			{
				buffer_size += sizeof(int);								//actions[i].attacked.UID = -1
				continue;	
			}
			buffer_size += sizeof(int);									//actions[i].attacked->GetUUID()

			if (actions[i].script == nullptr)
			{
				buffer_size += sizeof(int);								//actions[i].script->GetScriptName() = "No Script"
				buffer_size += strlen("No Script");						//
				continue;
			}
			buffer_size += sizeof(int);									//actions[i].script->GetScriptName()
			buffer_size += strlen(actions[i].script->GetScriptName());	//

			if (actions[i].method == nullptr)
			{
				buffer_size += sizeof(int);								//actions[i].method->name_method.c_str() = "No Function"
				buffer_size += strlen("No Function");					//
				continue;
			}
			buffer_size += sizeof(int);									//actions[i].method->name_method.c_str()
			buffer_size += actions[i].method->name_method.size();		//

			buffer_size += sizeof(int);									//actions[i].method->type
			switch (actions[i].method->type)
			{
			case VarType::Var_INT:
			{
				buffer_size += sizeof(int);								//*(int*)actions[i].value
				break;
			}
			case VarType::Var_FLOAT:
			{
				buffer_size += sizeof(float);							//*(float*)actions[i].value
				break;
			}
			case VarType::Var_BOOL:
			{
				buffer_size += sizeof(bool);							//*(bool*)actions[i].value
				break;
			}
			case VarType::Var_STRING:
			{
				break;
			}
			case VarType::Var_GAMEOBJECT:
			{
				break;
			}
			}
		}
	}
}

void ClickAction::SaveBinary(char** cursor) const
{
	// Save Actions (UI Button) ------------------------
	if (actions.size() > 0)
	{
		App->json_seria->SaveIntBinary(cursor, actions.size());
		for (int i = 0; i < actions.size(); i++)
		{
			App->json_seria->SaveIntBinary(cursor, actions[i].selected_mode);
			if (actions[i].attacked == nullptr)
			{
				App->json_seria->SaveIntBinary(cursor, 0);
				continue;
			}
			App->json_seria->SaveIntBinary(cursor, actions[i].attacked->GetUUID());

			if (actions[i].script == nullptr)
			{
				App->json_seria->SaveStringBinary(cursor, "No Script");
				continue;
			}
			App->json_seria->SaveStringBinary(cursor, actions[i].script->GetScriptName());

			if (actions[i].method == nullptr)
			{
				App->json_seria->SaveStringBinary(cursor, "No Function");
				continue;
			}
			App->json_seria->SaveStringBinary(cursor, actions[i].method->name_method);

			App->json_seria->SaveIntBinary(cursor, (int)actions[i].method->type);
			switch (actions[i].method->type)
			{
			case VarType::Var_INT:
			{
				App->json_seria->SaveIntBinary(cursor, *(int*)actions[i].value);
				break;
			}
			case VarType::Var_FLOAT:
			{
				App->json_seria->SaveFloatBinary(cursor, *(float*)actions[i].value);
				break;
			}
			case VarType::Var_BOOL:
			{
				App->json_seria->SaveBooleanBinary(cursor, *(bool*)actions[i].value);
				break;
			}
			case VarType::Var_STRING:
			{
				break;
			}
			case VarType::Var_GAMEOBJECT:
			{
				break;
			}
			}
		}
	}
}

void ClickAction::LoadBinary(char ** cursor)
{
	uint number_actions = App->json_seria->LoadIntBinary(cursor);
	for (int i = 0; i < number_actions; i++)
	{
		ClickActionData temp;

		temp.selected_mode = App->json_seria->LoadIntBinary(cursor);
		temp.UID_attacked = App->json_seria->LoadIntBinary(cursor);
		if (temp.UID_attacked == -1)
		{
			actions.push_back(temp);
			continue;
		}
		temp.current_script = App->json_seria->LoadStringBinary(cursor);
		if (temp.current_script == "No Script")
		{
			actions.push_back(temp);
			continue;
		}
		temp.current_function = App->json_seria->LoadStringBinary(cursor);
		if (temp.current_function == "No Function")
		{
			actions.push_back(temp);
			continue;
		}
		VarType type_function = (VarType)App->json_seria->LoadIntBinary(cursor);
		if (type_function != VarType::Var_NONE)
		{
			switch (type_function)
			{
			case VarType::Var_INT:
			{
				int value = App->json_seria->LoadIntBinary(cursor);
				temp.value = new char[sizeof value];
				*(int*)temp.value = value;
				break;
			}
			case VarType::Var_FLOAT:
			{
				float value = App->json_seria->LoadFloatBinary(cursor);
				temp.value = new char[sizeof value];
				*(float*)temp.value = value;
				break;
			}
			case VarType::Var_BOOL:
			{
				bool value = App->json_seria->LoadBooleanBinary(cursor);
				temp.value = new char[sizeof value];
				*(bool*)temp.value = value;
				break;
			}
			case VarType::Var_STRING:
			{
				break;
			}
			case VarType::Var_GAMEOBJECT:
			{
				break;
			}
			}
		}
		actions.push_back(temp);
	}
}

void ClickAction::SyncClickAction()
{


	for (int i = 0; i < actions.size(); i++)
	{
		if (actions[i].UID_attacked != -1)
		{
			actions[i].attacked = App->scene->GetGameObjectbyuid(actions[i].UID_attacked);
		}
		if (actions[i].current_script != "No Script")
		{
			actions[i].script = (CompScript*)actions[i].attacked->GetComponentByName(actions[i].current_script.c_str());
		}
		if (actions[i].current_function != "No Function")
		{
			if(actions[i].script->csharp == nullptr)
			{
				continue;
			}
			for (int j = 0; j < actions[i].script->csharp->methods.size(); j++)
			{
				if (actions[i].script->csharp->methods[j].name_method == actions[i].current_function)
				{
					actions[i].current_function = actions[i].script->csharp->methods[j].name_method;
					actions[i].method = &actions[i].script->csharp->methods[j];
					RELEASE_ARRAY(actions[i].value);
					if (actions[i].method->type != VarType::Var_NONE)
						InitValueParamater(i);
				}
			}
		}
	}
}

void ClickAction::ClearLinkedScripts()
{
	actions.clear();
}

