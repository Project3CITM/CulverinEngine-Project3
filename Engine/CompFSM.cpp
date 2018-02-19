#include "CompFSM.h"
#include "Application.h"
#include "WindowInspector.h"
#include "ModuleGUI.h"
#include "GameObject.h"
#include "Scene.h"

CompFiniteStateMachine::CompFiniteStateMachine(Comp_Type c_type, GameObject * parent) : Component(c_type, parent), initial_state(nullptr), current_state(nullptr), selected_state(nullptr), target_state(nullptr), new_transition(nullptr), candidate_state_to_delete(nullptr), candidate_transition_to_delete(nullptr), show_fsm(false), show_create_transition_window(false), show_create_conditions_window(false), show_select_script_window(false)
{
	name_component = "Finite State Machine";
}

CompFiniteStateMachine::CompFiniteStateMachine(const CompFiniteStateMachine & copy, GameObject * parent) : Component(Comp_Type::C_FSM, parent)
{
	name_component = copy.name_component;
}

CompFiniteStateMachine::~CompFiniteStateMachine()
{
}

void CompFiniteStateMachine::Start()
{
	for (std::vector<FSM_State*>::const_iterator it = states.begin(); it != states.end(); it++)
		(*it)->StartScripts();
}

void CompFiniteStateMachine::Update(float dt)
{
	if (current_state != nullptr)
	{
		FSM_Transition* triggered_transition = nullptr;

		current_state->CheckTriggeredTransition(triggered_transition);

		if (triggered_transition != nullptr)
		{
			FSM_State* target_state = triggered_transition->GetTargetState();
			current_state->DoExitAction();
			target_state->DoEntryAction();
			current_state = target_state;
		}

		current_state->DoAction(dt);
	}

	// --------------------------------  VISUALS  -------------------------------- //
	if (show_fsm)
	{
		if (ImGui::Begin("Finite State Machine", &show_fsm))
		{
			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
				ImGui::OpenPopup("New State Popup");

			// Shows basic information of all States
			for (std::vector<FSM_State*>::const_iterator it = states.begin(); it != states.end(); it++)
			{
				ImGui::Separator();

				ImGui::Text("%s:\n", (*it)->GetStateName());					CheckOpenStateOptions(*it);
				//DEBUG for now START
				if ((*it) == initial_state)
				{
					ImGui::SameLine();
					ImGui::Text(" <-- Starting State");
				}
				if ((*it) == selected_state)
				{
					ImGui::SameLine();
					ImGui::Text(" <-- Selected");
				}
				//DEBUG for now END
				ImGui::Text("Script: %s", (*it)->GetScriptName());				CheckOpenStateOptions(*it);
				ImGui::Text("Transitions (%i):", (*it)->GetNumTransitions());	CheckOpenStateOptions(*it);
				if (candidate_transition_to_delete != nullptr)
					(*it)->DisplayTransitionsInfo();				
				else
				{
					candidate_state_to_delete = (*it); // Just using the pointer temporarly
					candidate_transition_to_delete = (*it)->DisplayTransitionsInfo();
				}
				ImGui::Separator();
			}

			if (ImGui::BeginPopup("Erase Transition Popup"))
			{
				if (ImGui::Button("Delete") && candidate_state_to_delete != nullptr)
				{
					candidate_state_to_delete->DeleteTransition(candidate_transition_to_delete);
					candidate_state_to_delete = nullptr;
					candidate_transition_to_delete = nullptr;
				}

				ImGui::EndPopup();
			}
			//-------------------------------------
		}

		if (show_create_transition_window)
		{
			if (ImGui::Begin("Create Transition", &show_create_transition_window))
			{
				// --------------  Transitions  -------------- //
				std::vector<const char*> listbox_states;
				static int selected = 0;
				for (std::vector<FSM_State*>::const_iterator it = states.begin(); it != states.end(); it++)
					if (*it != selected_state)
						listbox_states.push_back((*it)->GetStateName());
				
				ImGui::Text("Choose the target State");
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.35f, 0.1f, 0.4f, 1.00f));
				ImGui::ListBox("", &selected, &listbox_states[0], (listbox_states.size()));
				ImGui::PopStyleColor();

				for (std::vector<FSM_State*>::const_iterator it_states = states.begin(); it_states != states.end(); it_states++)
					if ((*it_states)->GetStateName() == listbox_states[selected])
						target_state = *it_states;
				// --------------  Transitions  -------------- //

				// --------------  Conditions  -------------- //
				if (ImGui::Button("+"))
					new_conditions.push_back(0);
				
				ImGui::SameLine(); App->ShowHelpMarker("Click to add a new Condition");

				int i = 0;
				for (std::vector<int>::iterator it = new_conditions.begin(); it != new_conditions.end(); it++)
				{
					i++;
					ImGui::Text("Condition %i:", i); ImGui::SameLine();

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
					{
						condition_to_erase = it;
						ImGui::OpenPopup("Delete Condition Popup");
					}

					ImGui::PushID(i);
					ImGui::Combo("", &(*it), "Bool\0Equal Int\0Greater Int\0Greater or Equal Int\0Lower Int\0Lower or Equal Int\0Equal Float\0Greater Float\0Greater or Equal Float\0Lower Float\0Lower or Equal Float\0\0");
					ImGui::PopID();
				}
				// --------------  Conditions  -------------- //

				// --------------------------------  POPUPS  -------------------------------- //
				if (ImGui::BeginPopup("Delete Condition Popup"))
				{
					if (ImGui::Button("Delete"))
						new_conditions.erase(condition_to_erase);

					ImGui::EndPopup();
				}
				// --------------------------------  POPUPS  -------------------------------- //

				if (ImGui::Button("Confirm Transition"))
				{
					// --- Create new Transition --- //
					new_transition = selected_state->AddTransition(target_state);

					// --- Add Conditions to the new Transition --- //
					for (std::vector<int>::iterator it = new_conditions.begin(); it != new_conditions.end(); it++)
					{
						switch (*it)
						{
							// --- Bools --- //
						case 0:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_BOOL, true);
							break;
							// --- Bools --- //
							
							// --- Ints --- //
						case 1:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_EQUAL_INT, 0);
							break;
						case 2:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_GREATER_THAN_INT, 0);
							break;
						case 3:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_GREATER_EQUAL_INT, 0);
							break;
						case 4:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_LOWER_THAN_INT, 0);
							break;
						case 5:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_LOWER_EQUAL_INT, 0);
							break;
							// --- Ints --- //
							
							// --- Floats --- //
						case 6:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_EQUAL_FLOAT, 0.0f);
							break;
						case 7:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_GREATER_THAN_FLOAT, 0.0f);
							break;
						case 8:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_GREATER_EQUAL_FLOAT, 0.0f);
							break;
						case 9:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_LOWER_THAN_FLOAT, 0.0f);
							break;
						case 10:
							new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_LOWER_EQUAL_FLOAT, 0.0f);
							break;
							// --- Floats --- //

						default:
							break;
						}
					}
					// --- Add Conditions to the new Transition --- //

					show_create_conditions_window = true;
					show_create_transition_window = false;
				}
			}
			ImGui::End();
		}

		if (show_create_conditions_window)
		{
			if (ImGui::Begin("Create Conditions", &show_create_conditions_window))
			{
				// --------------  Conditions  -------------- //
				new_transition->CreateConditionsModifyingOptions();
				
				//TODO: Some way to save the values from the cript we will compare the conditions to
				// --------------  Conditions  -------------- //

				// --------------  Creates Transition with Conditions  -------------- //
				if (ImGui::Button("Confirm Conditions"))
				{
					new_conditions.clear();
					show_create_conditions_window = false;
				}
				// --------------  Creates Transition with Conditions  -------------- //
			}
			ImGui::End();
		}

		if (show_select_script_window)
		{
			SelectScript(selected_state);
		}

		// --------------------------------  POPUPS  -------------------------------- //
		if (ImGui::BeginPopup("New State Popup"))
		{
			if (ImGui::Button("Create New State"))
				CreateState();

			ImGui::EndPopup();
		}
		if (ImGui::BeginPopup("State Options Popup"))
		{
			if (ImGui::Button("Create New Transition"))
				show_create_transition_window = true;

			if (ImGui::Button("Set as initial state"))
				SetInitialState(selected_state);

			if (ImGui::Button("Add Script"))
				show_select_script_window = true;

			ImGui::EndPopup();
		}
		// --------------------------------  POPUPS  -------------------------------- //

		ImGui::End();
	}
	// --------------------------------  VISUALS  -------------------------------- //
}

void CompFiniteStateMachine::Clear()
{
}

void CompFiniteStateMachine::ShowOptions()
{
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Reset"))
	{
		ImGui::CloseCurrentPopup();
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
	if (ImGui::MenuItem("Copy Component", NULL, false))
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
			CopyValues(((CompFiniteStateMachine*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompFiniteStateMachine::ShowInspectorInfo()
{
	// Reset Values Button -------------------------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
		ImGui::OpenPopup("OptionsFSM");

	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsFSM"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	if (ImGui::Button("Show FSM"))
		show_fsm = true;

	ImGui::TreePop();
}

void CompFiniteStateMachine::CopyValues(const CompFiniteStateMachine * component)
{
	//more...
}

void CompFiniteStateMachine::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", C_FSM);
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", 0);
}

void CompFiniteStateMachine::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	Enable();
}

FSM_State * CompFiniteStateMachine::CreateState()
{
	LOG("Creating FSM State.");
	FSM_State* new_state = new FSM_State();

	if (states.empty())
		SetInitialState(new_state);

	states.push_back(new_state);
	std::string state_name = "State " + std::to_string(states.size());
	new_state->SetStateName(state_name.c_str());

	return new_state;
}

bool CompFiniteStateMachine::SetCurrentState(FSM_State * new_current_state)
{
	if (new_current_state == nullptr)
		return false;

	current_state = new_current_state;
	return true;
}

bool CompFiniteStateMachine::SetInitialState(FSM_State * new_initial_state)
{
	if (new_initial_state == nullptr)
		return false;

	initial_state = new_initial_state;
	return true;
}

uint CompFiniteStateMachine::GetNumStates() const
{
	return states.size();
}

FSM_State * CompFiniteStateMachine::GetCurrentState() const
{
	return current_state;
}

FSM_State * CompFiniteStateMachine::GetInitialState() const
{
	return initial_state;
}

CompScript * CompFiniteStateMachine::SelectScript(FSM_State* state_to_add_script)
{
	CompScript* new_script = new CompScript()

	ResourceScript* temp = (ResourceScript*)App->resource_manager->ShowResources(select_script, Resource::Type::SCRIPT);
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
			csharp = App->importer->iScript->LoadScript_CSharp(resource_script->GetPathdll());
			SetOwnGameObject(parent);
		}
		Enable();
		return this;
	}

	return nullptr;
}

void CompFiniteStateMachine::CheckOpenStateOptions(FSM_State* state)
{
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
	{
		selected_state = state;
		ImGui::OpenPopup("State Options Popup");
	}
	else if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
		selected_state = state;
}

FSM_State::FSM_State() : script(nullptr)
{
}

FSM_State::FSM_State(const FSM_State & copy) : script(copy.script)	//doesn't copy the transitions
{	
}

FSM_State::~FSM_State()
{
	delete script;
	for (std::vector<FSM_Transition*>::iterator it = transitions.begin(); it != transitions.end(); it++)
		delete *it;
}

bool FSM_State::StartScripts()
{
	if (script == nullptr)
		return false;
	
	script->Start();
	return true;
}

void FSM_State::DoEntryAction()
{
}

void FSM_State::DoAction(float dt)
{
	if (script != nullptr)
		script->UpdateScript(dt);
}

void FSM_State::DoExitAction()
{
}

bool FSM_State::AddScript(CompScript * script_to_add)
{
	if (script != nullptr || script_to_add == nullptr)	//Only support 1 script per node yet
		return false;

	script = script_to_add;
	return true;
}

FSM_Transition * FSM_State::AddTransition(FSM_State * target_state)
{
	LOG("Creating FSM Transition.");
	FSM_Transition* new_transition = new FSM_Transition(target_state);
	transitions.push_back(new_transition);

	return new_transition;
}

bool FSM_State::DeleteTransition(FSM_Transition * transition_to_delete)
{
	if (transition_to_delete == nullptr)
		return false;

	for (std::vector<FSM_Transition*>::const_iterator it = transitions.begin(); it != transitions.end(); it++)
	{
		if ((*it) == transition_to_delete)
		{
			transitions.erase(it);
			delete transition_to_delete;
			return true;
		}
	}

	return false;
}

bool FSM_State::CheckTriggeredTransition(FSM_Transition * transition) const
{
	for (std::vector<FSM_Transition*>::const_iterator it = transitions.begin(); it != transitions.end(); it++)
		if ((*it)->IsTriggered())
		{
			transition = *it;
			return true;
		}
	return false;
}

FSM_Transition* FSM_State::DisplayTransitionsInfo()
{
	FSM_Transition* candidate_transition_to_delete = nullptr;
	for (std::vector<FSM_Transition*>::const_iterator it_transitions = transitions.begin(); it_transitions != transitions.end(); it_transitions++)
	{
		ImGui::Text("     - Target State: %s", (*it_transitions)->GetTargetState()->GetStateName());	//TODO Open Popup to delete a transition
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
			candidate_transition_to_delete = (*it_transitions);

		ImGui::Text("          - Conditions (%i):", (*it_transitions)->GetNumConditions());
		(*it_transitions)->DisplayConditionsInfo();
	}

	if (candidate_transition_to_delete != nullptr)
		ImGui::OpenPopup("Erase Transition Popup");

	return candidate_transition_to_delete;
}

void FSM_State::SetStateName(const char * new_name)
{
	state_name = new_name;
}

uint FSM_State::GetNumTransitions() const
{
	return transitions.size();
}

const char * FSM_State::GetStateName() const
{
	return state_name.c_str();
}

const char * FSM_State::GetScriptName() const
{
	if (script != nullptr)
		return script->GetScriptName();
	return nullptr;
}

FSM_Transition::FSM_Transition(FSM_State * target_state_) : target_state(target_state_)
{
}

FSM_Transition::~FSM_Transition()
{
	target_state = nullptr;
	for (std::vector<FSM_Condition*>::iterator it = conditions.begin(); it != conditions.end(); it++)
		delete *it;
}

FSM_Condition * FSM_Transition::AddCondition(FSM_CONDITION_TYPE condition_type, bool condition)
{
	LOG("Creating FSM Condition.");
	FSM_Condition* new_condition = nullptr;

	switch (condition_type)
	{
	case FSM_COND_NONE:
		LOG("Incorrect FSM Condition Type.");
		return nullptr;
		break;
	case FSM_COND_BOOL:
		new_condition = new FSM_ConditionBool(condition);
		break;
	case FSM_COND_EQUAL_INT:
	case FSM_COND_GREATER_THAN_INT:
	case FSM_COND_GREATER_EQUAL_INT:
	case FSM_COND_LOWER_THAN_INT:
	case FSM_COND_LOWER_EQUAL_INT:
	case FSM_COND_EQUAL_FLOAT:
	case FSM_COND_GREATER_THAN_FLOAT:
	case FSM_COND_GREATER_EQUAL_FLOAT:
	case FSM_COND_LOWER_THAN_FLOAT:
	case FSM_COND_LOWER_EQUAL_FLOAT:
	case FSM_COND_MAX:
	default:
		LOG("Incorrect FSM Condition Type.");
		return nullptr;
		break;
	}

	conditions.push_back(new_condition);

	return new_condition;
}

FSM_Condition * FSM_Transition::AddCondition(FSM_CONDITION_TYPE condition_type, int condition)
{
	LOG("Creating FSM Condition.");
	FSM_Condition* new_condition = nullptr;

	switch (condition_type)
	{
	case FSM_COND_NONE:
	case FSM_COND_BOOL:
		LOG("Incorrect FSM Condition Type.");
		return nullptr;
		break;
	case FSM_COND_EQUAL_INT:
		new_condition = new FSM_ConditionEqualInt(condition);
		break;
	case FSM_COND_GREATER_THAN_INT:
		new_condition = new FSM_ConditionGreaterThanInt(condition);
		break;
	case FSM_COND_GREATER_EQUAL_INT:
		new_condition = new FSM_ConditionGreaterEqualInt(condition);
		break;
	case FSM_COND_LOWER_THAN_INT:
		new_condition = new FSM_ConditionLowerThanInt(condition);
		break;
	case FSM_COND_LOWER_EQUAL_INT:
		new_condition = new FSM_ConditionLowerEqualInt(condition);
		break;
	case FSM_COND_EQUAL_FLOAT:
	case FSM_COND_GREATER_THAN_FLOAT:
	case FSM_COND_GREATER_EQUAL_FLOAT:
	case FSM_COND_LOWER_THAN_FLOAT:
	case FSM_COND_LOWER_EQUAL_FLOAT:
	case FSM_COND_MAX:
	default:
		LOG("Incorrect FSM Condition Type.");
		return nullptr;
		break;
	}

	conditions.push_back(new_condition);

	return new_condition;
}

FSM_Condition * FSM_Transition::AddCondition(FSM_CONDITION_TYPE condition_type, float condition)
{
	LOG("Creating FSM Condition.");
	FSM_Condition* new_condition = nullptr;

	switch (condition_type)
	{
	case FSM_COND_NONE:
	case FSM_COND_BOOL:
	case FSM_COND_EQUAL_INT:
	case FSM_COND_GREATER_THAN_INT:
	case FSM_COND_GREATER_EQUAL_INT:
	case FSM_COND_LOWER_THAN_INT:
	case FSM_COND_LOWER_EQUAL_INT:
		LOG("Incorrect FSM Condition Type.");
		return nullptr;
		break;
	case FSM_COND_EQUAL_FLOAT:
		new_condition = new FSM_ConditionEqualFloat(condition);
		break; 
	case FSM_COND_GREATER_THAN_FLOAT:
		new_condition = new FSM_ConditionGreaterThanFloat(condition);
		break;
	case FSM_COND_GREATER_EQUAL_FLOAT:
		new_condition = new FSM_ConditionGreaterEqualFloat(condition);
		break;
	case FSM_COND_LOWER_THAN_FLOAT:
		new_condition = new FSM_ConditionLowerThanFloat(condition);
		break;
	case FSM_COND_LOWER_EQUAL_FLOAT:
		new_condition = new FSM_ConditionLowerEqualFloat(condition);
		break;
	case FSM_COND_MAX:
	default:
		LOG("Incorrect FSM Condition Type.");
		return nullptr;
		break;
	}

	conditions.push_back(new_condition);

	return new_condition;
}

bool FSM_Transition::IsTriggered()const
{
	//TODO: Check all conditions
	for (std::vector<FSM_Condition*>::const_iterator it = conditions.begin(); it != conditions.end(); it++)
	{
		switch ((*it)->GetConditionType())
		{
		case FSM_COND_NONE:
			break;
		case FSM_COND_BOOL:
// TODO				return true;
			break;
		case FSM_COND_EQUAL_INT:
		case FSM_COND_GREATER_THAN_INT:
		case FSM_COND_GREATER_EQUAL_INT:
		case FSM_COND_LOWER_THAN_INT:
		case FSM_COND_LOWER_EQUAL_INT:
// TODO				return true; 
			break;
		case FSM_COND_EQUAL_FLOAT:
		case FSM_COND_GREATER_THAN_FLOAT:
		case FSM_COND_GREATER_EQUAL_FLOAT:
		case FSM_COND_LOWER_THAN_FLOAT:
		case FSM_COND_LOWER_EQUAL_FLOAT:
// TODO				return true; 
			break;
		case FSM_COND_MAX:
			break;
		default:
			break;
		}
	}

	return false;
}

FSM_State * FSM_Transition::GetTargetState()
{
	return target_state;
}

void FSM_Transition::CreateConditionsModifyingOptions()
{
	int id = 0;
	for (std::vector<FSM_Condition*>::const_iterator it = conditions.begin(); it != conditions.end(); it++, id++)
	{
		bool b = ((FSM_ConditionBool*)*it)->GetCondition();
		int i = 0;
		float f = 0.0f;

		ImGui::PushID(id);
		switch ((*it)->GetConditionType())
		{
		case FSM_COND_NONE:
			break;

		case FSM_COND_BOOL:
			if (ImGui::Checkbox("true", &b))
				((FSM_ConditionBool*)*it)->SetCondition(b);
			break;

		case FSM_COND_EQUAL_INT:
			i = ((FSM_ConditionEqualInt*)*it)->GetCondition();
			ImGui::Text("Equal to: ");
			if (ImGui::InputInt("", &i))
				((FSM_ConditionEqualInt*)*it)->SetCondition(i);
			break;

		case FSM_COND_GREATER_THAN_INT:
			i = ((FSM_ConditionGreaterThanInt*)*it)->GetCondition();
			ImGui::Text("Greater than: ");
			if (ImGui::InputInt("", &i))
				((FSM_ConditionGreaterThanInt*)*it)->SetCondition(i);
			break;

		case FSM_COND_GREATER_EQUAL_INT:
			i = ((FSM_ConditionGreaterEqualInt*)*it)->GetCondition();
			ImGui::Text("Greater or Equal than: ");
			if (ImGui::InputInt("", &i))
				((FSM_ConditionGreaterEqualInt*)*it)->SetCondition(i);
			break;

		case FSM_COND_LOWER_THAN_INT:
			i = ((FSM_ConditionLowerThanInt*)*it)->GetCondition();
			ImGui::Text("Lower than: ");
			if (ImGui::InputInt("", &i))
				((FSM_ConditionLowerThanInt*)*it)->SetCondition(i);
			break;

		case FSM_COND_LOWER_EQUAL_INT:
			i = ((FSM_ConditionLowerEqualInt*)*it)->GetCondition();
			ImGui::Text("Lower or Equal than: ");
			if (ImGui::InputInt("", &i))
				((FSM_ConditionLowerEqualInt*)*it)->SetCondition(i);
			break;

		case FSM_COND_EQUAL_FLOAT:
			f = ((FSM_ConditionEqualFloat*)*it)->GetCondition();
			ImGui::Text("Equal to: ");
			if (ImGui::InputFloat("", &f))
				((FSM_ConditionEqualFloat*)*it)->SetCondition(f);
			break;

		case FSM_COND_GREATER_THAN_FLOAT:
			f = ((FSM_ConditionGreaterThanFloat*)*it)->GetCondition();
			ImGui::Text("Greater than: ");
			if (ImGui::InputFloat("", &f))
				((FSM_ConditionGreaterThanFloat*)*it)->SetCondition(f);
			break;

		case FSM_COND_GREATER_EQUAL_FLOAT:
			f = ((FSM_ConditionGreaterEqualFloat*)*it)->GetCondition();
			ImGui::Text("Greater or Equal than: ");
			if (ImGui::InputFloat("", &f))
				((FSM_ConditionGreaterEqualFloat*)*it)->SetCondition(f);
			break;

		case FSM_COND_LOWER_THAN_FLOAT:
			f = ((FSM_ConditionLowerThanFloat*)*it)->GetCondition();
			ImGui::Text("Lower than: ");
			if (ImGui::InputFloat("", &f))
				((FSM_ConditionLowerThanFloat*)*it)->SetCondition(f);
			break;

		case FSM_COND_LOWER_EQUAL_FLOAT:
			f = ((FSM_ConditionLowerEqualFloat*)*it)->GetCondition();
			ImGui::Text("Lower or Equal than: ");
			if (ImGui::InputFloat("", &f))
				((FSM_ConditionLowerEqualFloat*)*it)->SetCondition(f);
			break;

		case FSM_COND_MAX:
			break;

		default:
			break;
		}
		ImGui::PopID();
	}
}

void FSM_Transition::DisplayConditionsInfo()
{
	for (std::vector<FSM_Condition*>::const_iterator it_conditions = conditions.begin(); it_conditions != conditions.end(); it_conditions++)
		ImGui::Text("               - Condition %i", (*it_conditions)->GetConditionType());	//TODO Open Popup to delete a condition
}

uint FSM_Transition::GetNumConditions() const
{
	return conditions.size();
}

FSM_Condition::FSM_Condition(FSM_CONDITION_TYPE condition_type_) : condition_type(condition_type_)
{
}

FSM_Condition::~FSM_Condition()
{
}

FSM_CONDITION_TYPE FSM_Condition::GetConditionType() const
{
	return condition_type;
}

FSM_ConditionBool::FSM_ConditionBool(bool condition_) : FSM_Condition(FSM_COND_BOOL), condition(condition_)
{
}

FSM_ConditionBool::~FSM_ConditionBool()
{
}

bool FSM_ConditionBool::Test(bool b)
{
	return condition == b;
}

bool FSM_ConditionBool::SetCondition(bool condition_)
{
	condition = condition_;
	return true;
}

bool FSM_ConditionBool::GetCondition() const
{
	return condition;
}

FSM_ConditionEqualInt::FSM_ConditionEqualInt(int condition_) : FSM_Condition(FSM_COND_EQUAL_INT), condition(condition_)
{
}

FSM_ConditionEqualInt::~FSM_ConditionEqualInt()
{
}

bool FSM_ConditionEqualInt::Test(int i)
{
	return i == condition;
}

bool FSM_ConditionEqualInt::SetCondition(int condition_)
{
	condition = condition_;
	return true;
}

int FSM_ConditionEqualInt::GetCondition() const
{
	return condition;
}

FSM_ConditionGreaterThanInt::FSM_ConditionGreaterThanInt(int condition_) : FSM_Condition(FSM_COND_GREATER_THAN_INT), condition(condition_)
{
}

FSM_ConditionGreaterThanInt::~FSM_ConditionGreaterThanInt()
{
}

bool FSM_ConditionGreaterThanInt::Test(int i)
{
	return i > condition;
}

bool FSM_ConditionGreaterThanInt::SetCondition(int condition_)
{
	condition = condition_;
	return true;
}

int FSM_ConditionGreaterThanInt::GetCondition() const
{
	return condition;
}

FSM_ConditionGreaterEqualInt::FSM_ConditionGreaterEqualInt(int condition_) : FSM_Condition(FSM_COND_GREATER_EQUAL_INT), condition(condition_)
{
}

FSM_ConditionGreaterEqualInt::~FSM_ConditionGreaterEqualInt()
{
}

bool FSM_ConditionGreaterEqualInt::Test(int i)
{
	return i >= condition;
}

bool FSM_ConditionGreaterEqualInt::SetCondition(int condition_)
{
	condition = condition_;
	return true;
}

int FSM_ConditionGreaterEqualInt::GetCondition() const
{
	return condition;
}

FSM_ConditionLowerThanInt::FSM_ConditionLowerThanInt(int condition_) : FSM_Condition(FSM_COND_LOWER_THAN_INT), condition(condition_)
{
}

FSM_ConditionLowerThanInt::~FSM_ConditionLowerThanInt()
{
}

bool FSM_ConditionLowerThanInt::Test(int i)
{
	return i < condition;
}

bool FSM_ConditionLowerThanInt::SetCondition(int condition_)
{
	condition = condition_;
	return true;
}

int FSM_ConditionLowerThanInt::GetCondition() const
{
	return condition;
}

FSM_ConditionLowerEqualInt::FSM_ConditionLowerEqualInt(int condition_) : FSM_Condition(FSM_COND_LOWER_EQUAL_INT), condition(condition_)
{
}

FSM_ConditionLowerEqualInt::~FSM_ConditionLowerEqualInt()
{
}

bool FSM_ConditionLowerEqualInt::Test(int i)
{
	return i <= condition;
}

bool FSM_ConditionLowerEqualInt::SetCondition(int condition_)
{
	condition = condition_;
	return true;
}

int FSM_ConditionLowerEqualInt::GetCondition() const
{
	return condition;
}

FSM_ConditionEqualFloat::FSM_ConditionEqualFloat(float condition_) : FSM_Condition(FSM_COND_EQUAL_FLOAT), condition(condition_)
{
}

FSM_ConditionEqualFloat::~FSM_ConditionEqualFloat()
{
}

bool FSM_ConditionEqualFloat::Test(float i)
{
	return i == condition;
}

bool FSM_ConditionEqualFloat::SetCondition(float condition_)
{
	condition = condition_;
	return true;
}

float FSM_ConditionEqualFloat::GetCondition() const
{
	return condition;
}

FSM_ConditionGreaterThanFloat::FSM_ConditionGreaterThanFloat(float condition_) : FSM_Condition(FSM_COND_GREATER_THAN_FLOAT), condition(condition_)
{
}

FSM_ConditionGreaterThanFloat::~FSM_ConditionGreaterThanFloat()
{
}

bool FSM_ConditionGreaterThanFloat::Test(float i)
{
	return i > condition;
}

bool FSM_ConditionGreaterThanFloat::SetCondition(float condition_)
{
	condition = condition_;
	return true;
}

float FSM_ConditionGreaterThanFloat::GetCondition() const
{
	return condition;
}

FSM_ConditionGreaterEqualFloat::FSM_ConditionGreaterEqualFloat(float condition_) : FSM_Condition(FSM_COND_GREATER_EQUAL_FLOAT), condition(condition_)
{
}

FSM_ConditionGreaterEqualFloat::~FSM_ConditionGreaterEqualFloat()
{
}

bool FSM_ConditionGreaterEqualFloat::Test(float i)
{
	return i >= condition;
}

bool FSM_ConditionGreaterEqualFloat::SetCondition(float condition_)
{
	condition = condition_;
	return true;
}

float FSM_ConditionGreaterEqualFloat::GetCondition() const
{
	return condition;
}

FSM_ConditionLowerThanFloat::FSM_ConditionLowerThanFloat(float condition_) : FSM_Condition(FSM_COND_LOWER_THAN_FLOAT), condition(condition_)
{
}

FSM_ConditionLowerThanFloat::~FSM_ConditionLowerThanFloat()
{
}

bool FSM_ConditionLowerThanFloat::Test(float i)
{
	return i < condition;
}

bool FSM_ConditionLowerThanFloat::SetCondition(float condition_)
{
	condition = condition_;
	return true;
}

float FSM_ConditionLowerThanFloat::GetCondition() const
{
	return condition;
}

FSM_ConditionLowerEqualFloat::FSM_ConditionLowerEqualFloat(float condition_) : FSM_Condition(FSM_COND_LOWER_EQUAL_FLOAT), condition(condition_)
{
}

FSM_ConditionLowerEqualFloat::~FSM_ConditionLowerEqualFloat()
{
}

bool FSM_ConditionLowerEqualFloat::Test(float i)
{
	return i <= condition;
}

bool FSM_ConditionLowerEqualFloat::SetCondition(float condition_)
{
	condition = condition_;
	return true;
}

float FSM_ConditionLowerEqualFloat::GetCondition() const
{
	return condition;
}
