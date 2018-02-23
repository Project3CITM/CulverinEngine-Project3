#include "CompFSM.h"
#include "Application.h"
#include "WindowInspector.h"
#include "ModuleGUI.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompScript.h"
#include "CSharpScript.h"

CompFiniteStateMachine::CompFiniteStateMachine(Comp_Type c_type, GameObject * parent) : Component(c_type, parent), initial_state(nullptr),
current_state(nullptr), selected_state(nullptr), selected_transition(nullptr), selected_condition(nullptr), target_state(nullptr), new_transition(nullptr),
show_fsm(false), show_create_transition_window(false), show_create_conditions_window(false), show_selecting_script_window(false), states_id(0)
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

void CompFiniteStateMachine::Init()
{
	for (std::vector<FSM_State*>::const_iterator it = states.begin(); it != states.end(); it++)
		(*it)->Init();
}

void CompFiniteStateMachine::PreUpdate(float dt)
{
	for (std::vector<FSM_State*>::const_iterator it = states.begin(); it != states.end(); it++)
		(*it)->PreUpdate(dt);
}

void CompFiniteStateMachine::Start()
{
	for (std::vector<FSM_State*>::const_iterator it = states.begin(); it != states.end(); it++)
		(*it)->StartScripts();
}

void CompFiniteStateMachine::Update(float dt)
{
	if (App->engine_state == EngineState::STOP)
		SetCurrentState(initial_state); 
	
	if (current_state != nullptr && (App->engine_state == EngineState::PLAY || App->engine_state == EngineState::PLAYFRAME))
	{
		FSM_Transition* triggered_transition = nullptr;
		
		if (current_state->CheckTriggeredTransition(&triggered_transition))
		{
			FSM_State* target_state = triggered_transition->GetTargetState();
			current_state->DoExitAction();
			target_state->DoEntryAction();
			current_state = target_state;
		}

		current_state->DoAction(dt);
	}

	else
	{
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
					if ((*it) == current_state)
					{
						ImGui::SameLine();
						ImGui::Text(" <-- Current State");
					}
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
					if(!show_selecting_script_window)
						(*it)->ShowScriptInfo();
					ImGui::Text("Transitions (%i):", (*it)->GetNumTransitions());	CheckOpenStateOptions(*it);
					// --- Show Transitions + Manages the selected state/transition/condition --- //
					if ((*it)->DisplayTransitionsInfo(&selected_transition, &selected_condition))
						selected_state = *it;

					ImGui::Separator();
				}

				// --------------------------------  POPUPS  -------------------------------- //
				if (ImGui::BeginPopup("Create New Condition"))
				{
					if (ImGui::Button("New Condition (Not working yet)") && selected_transition != nullptr)
					{
						//TODO: Create new condition here
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				if (ImGui::BeginPopup("Erase Transition Popup"))
				{
					if (ImGui::Button("Delete") && selected_transition != nullptr)
					{
						selected_state->DeleteTransition(selected_transition);
						selected_transition = nullptr;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				if (ImGui::BeginPopup("Erase Condition Popup"))
				{
					if (ImGui::Button("Delete"))
					{
						selected_transition->DeleteCondition(selected_condition);
						selected_condition = nullptr;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
				// --------------------------------  POPUPS  -------------------------------- //
				//-------------------------------------
			}

			if (show_create_transition_window)
			{
				if (states.size() <= 1)
				{
					show_create_transition_window = false;
					LOG("Can't create a transition between states if you dont have 2 States");
				}
				else
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
							ImGui::Text("Only Bools working atm");
							ImGui::Combo("", &(*it), "Bool\0Equal Int\0Greater Int\0Greater or Equal Int\0Lower Int\0Lower or Equal Int\0Equal Float\0Greater Float\0Greater or Equal Float\0Lower Float\0Lower or Equal Float\0\0");
							*it = 0;// <-- TODO: Only Bools working atm
							ImGui::PopID();
						}
						// --------------  Conditions  -------------- //

						// --------------------------------  POPUPS  -------------------------------- //
						if (ImGui::BeginPopup("Delete Condition Popup"))
						{
							if (ImGui::Button("Delete"))
							{
								new_conditions.erase(condition_to_erase);
								ImGui::CloseCurrentPopup();
							}
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
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_EQUAL_INT, 0, 0);
									break;
								case 2:
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_GREATER_THAN_INT, 0, 0);
									break;
								case 3:
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_GREATER_EQUAL_INT, 0, 0);
									break;
								case 4:
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_LOWER_THAN_INT, 0, 0);
									break;
								case 5:
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_LOWER_EQUAL_INT, 0, 0);
									break;
									// --- Ints --- //

									// --- Floats --- //
								case 6:
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_EQUAL_FLOAT, 0.0f, 0.0f);
									break;
								case 7:
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_GREATER_THAN_FLOAT, 0.0f, 0.0f);
									break;
								case 8:
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_GREATER_EQUAL_FLOAT, 0.0f, 0.0f);
									break;
								case 9:
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_LOWER_THAN_FLOAT, 0.0f, 0.0f);
									break;
								case 10:
									new_transition->AddCondition(FSM_CONDITION_TYPE::FSM_COND_LOWER_EQUAL_FLOAT, 0.0f, 0.0f);
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
			}

			if (show_create_conditions_window)
			{
				if (ImGui::Begin("Create Conditions", &show_create_conditions_window))
				{
					// --------------  Conditions  -------------- //
					new_transition->CreateConditionsModifyingOptions(selected_state);
					// --------------  Conditions  -------------- //

					// --------------  Creates Transition with Conditions  -------------- //
					ImGui::Separator();
					if (ImGui::Button("Confirm Conditions"))
					{
						new_conditions.clear();
						show_create_conditions_window = false;
					}
					// --------------  Creates Transition with Conditions  -------------- //
				}
				ImGui::End();
			}

			if (show_selecting_script_window)
				if (!selected_state->SelectScript(show_selecting_script_window) && !show_selecting_script_window)
					selected_state->RemoveScript();

			// --------------------------------  POPUPS  -------------------------------- //
			if (ImGui::BeginPopup("New State Popup"))
			{
				if (ImGui::Button("Create New State"))
				{
					CreateState();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
			if (ImGui::BeginPopup("State Options Popup"))
			{
				if (ImGui::Button("Add Script"))
				{
					selected_state->AddScript(new CompScript(Comp_Type::C_SCRIPT, parent));
					show_selecting_script_window = true;
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Remove Script"))
				{
					selected_state->RemoveScript();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Create New Transition"))
				{
					show_create_transition_window = true;
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Set as initial state"))
				{
					SetInitialState(selected_state);
					SetCurrentState(selected_state);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Delete State") && selected_state != nullptr)
				{
					DeleteState(selected_state);
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
			// --------------------------------  POPUPS  -------------------------------- //

			ImGui::End();
		}
		// --------------------------------  VISUALS  -------------------------------- //
	}
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

	json_object_dotset_number_with_std(object, name + "States ID", states_id);
	int num_states = 0;
	for (std::vector<FSM_State*>::const_iterator it_states = states.begin(); it_states != states.end(); it_states++)
	{
		num_states++;
		if(initial_state == (*it_states))
			json_object_dotset_number_with_std(object, name + "Initial State Num", num_states);

		(*it_states)->SaveState(object, name, saveScene, countResources, num_states);
	}
	json_object_dotset_number_with_std(object, name + "Number States", num_states);
}

void CompFiniteStateMachine::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	
	int num_states = json_object_dotget_number_with_std(object, name + "Number States");
	int initial_state_num = initial_state_num = json_object_dotget_number_with_std(object, name + "Initial State Num");
	for (int i = 1; i <= num_states; i++)
	{
		FSM_State* new_state = new FSM_State(-1);
		new_state->LoadState(object, name, i, parent);

		if (i == initial_state_num)
			initial_state = current_state = new_state;
	
		states.push_back(new_state);
	}

	// Must Load the Transitions state targets
	for (std::vector<FSM_State*>::const_iterator it_states = states.begin(); it_states != states.end(); it_states++)
		(*it_states)->LoadTransitionsTargetStates(states);

	states_id = json_object_dotget_number_with_std(object, name + "States ID");

	Enable();
}

FSM_State * CompFiniteStateMachine::CreateState()
{
	LOG("Creating FSM State.");
	states_id++;
	FSM_State* new_state = new FSM_State(states_id);
	
	if (states.empty())
	{
		SetInitialState(new_state);
		SetCurrentState(new_state);
	}

	states.push_back(new_state);
	std::string state_name = "State " + std::to_string(states.size());
	new_state->SetStateName(state_name.c_str());

	return new_state;
}

bool CompFiniteStateMachine::DeleteState(FSM_State * state_to_delete)
{
	if (state_to_delete == nullptr)
		return false;

	if (state_to_delete == initial_state)
	{
		if (states.size() > 1)
		{
			if (state_to_delete == states[0])
				initial_state = current_state = states[1];
			else
				initial_state = current_state = states[0];
		}
		else
			initial_state = current_state = nullptr;		
	}

	for (std::vector<FSM_State*>::const_iterator it_states = states.begin(); it_states != states.end(); it_states++)
	{
		if ((*it_states) == state_to_delete)
		{
			state_to_delete->DeleteAllTransitions();
			states.erase(it_states);
			delete state_to_delete;
			return true;
		}
	}

	return false;
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

FSM_State::FSM_State(int state_id_) : script(nullptr), state_id(state_id_)
{
}

FSM_State::FSM_State(const FSM_State & copy, int state_id_) : script(copy.script), state_id(state_id_)	//doesn't copy the transitions
{	
}

FSM_State::~FSM_State()
{
	delete script;
	for (std::vector<FSM_Transition*>::iterator it = transitions.begin(); it != transitions.end(); it++)
		delete *it;
}

bool FSM_State::Init()
{
	if (script == nullptr)
		return false;

	script->Init();
	return true;
}

bool FSM_State::PreUpdate(float dt)
{
	if (script == nullptr)
		return false;

	script->PreUpdate(dt);
	return true;
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
		script->Update(dt);
}

void FSM_State::DoExitAction()
{
}

void FSM_State::SaveState(JSON_Object * object, std::string name, bool saveScene, uint & countResources, int state_num)
{
	json_object_dotset_number_with_std(object, name + "State ID" + std::to_string(state_num), state_id);
	json_object_dotset_string_with_std(object, name + "State Name" + std::to_string(state_num), state_name.c_str());
	
	if (script == nullptr)
		json_object_dotset_boolean_with_std(object, name + "Has Script" + std::to_string(state_num), false);
	else
	{
		json_object_dotset_boolean_with_std(object, name + "Has Script" + std::to_string(state_num), true);
		json_object_dotset_string_with_std(object, name + "Script Name" + std::to_string(state_num), script->GetScriptName());
	}

	int num_transitions = 0;
	for (std::vector<FSM_Transition*>::const_iterator it_transitions = transitions.begin(); it_transitions != transitions.end(); it_transitions++)
	{
		num_transitions++;
		(*it_transitions)->SaveTransition(object, name, saveScene, countResources, state_num, num_transitions);
	}
	json_object_dotset_number_with_std(object, name + "Number Transitions State" + std::to_string(state_num), num_transitions);
}

void FSM_State::LoadState(const JSON_Object * object, std::string name, int num_state, GameObject* parent)
{
	state_id = json_object_dotget_number_with_std(object, name + "State ID" + std::to_string(num_state));
	state_name = json_object_dotget_string_with_std(object, name + "State Name" + std::to_string(num_state));

	if (json_object_dotget_boolean_with_std(object, name + "Has Script" + std::to_string(num_state)))
	{
		script = new CompScript(Comp_Type::C_SCRIPT, parent);
		script->name_script = json_object_dotget_string_with_std(object, name + "Script Name" + std::to_string(num_state));
		script->AddScriptbyName(script->name_script.c_str()); // Doesn't load the variables :(
	}

	int num_transitions = json_object_dotget_number_with_std(object, name + "Number Transitions State" + std::to_string(num_state));
	for (int i = 1; i <= num_transitions; i++)
	{
		FSM_Transition* new_transition = new FSM_Transition();
		new_transition->LoadTransition(object, name, num_state, i, this);
		transitions.push_back(new_transition);
	}
}

void FSM_State::LoadTransitionsTargetStates(const std::vector<FSM_State*>& states)
{
	for (std::vector<FSM_Transition*>::const_iterator it_transitions = transitions.begin(); it_transitions != transitions.end(); it_transitions++)
		for (std::vector<FSM_State*>::const_iterator it_states = states.begin(); it_states != states.end(); it_states++)
			if ((*it_transitions)->GetTargetStateID() == (*it_states)->GetStateID())
				(*it_transitions)->SetTargetState(*it_states);
}

bool FSM_State::AddScript(CompScript * script_to_add)
{
	if (script != nullptr || script_to_add == nullptr)	//Only support 1 script per node yet
		return false;

	script = script_to_add;
	return true;
}

bool FSM_State::SelectScript(bool& selecting)
{
	return script->SelectScript(selecting);
}

bool FSM_State::RemoveScript()
{
	if (script == nullptr)
		return false;

	delete script;
	script = nullptr;
	return true;
}

FSM_Transition * FSM_State::AddTransition(FSM_State * target_state)
{
	LOG("Creating FSM Transition.");
	FSM_Transition* new_transition = new FSM_Transition(target_state);
	transitions.push_back(new_transition);

	return new_transition;
}

bool FSM_State::DeleteAllTransitions()
{
	if (transitions.size() == 0)
		return false;

	for (std::vector<FSM_Transition*>::const_iterator it_trans = transitions.begin(); it_trans != transitions.end(); it_trans++)
	{
		(*it_trans)->DeleteAllConditions();
		delete (*it_trans);
	}

	transitions.clear();

	return true;
}

bool FSM_State::DeleteTransition(FSM_Transition * transition_to_delete)
{
	if (transition_to_delete == nullptr)
		return false;

	for (std::vector<FSM_Transition*>::const_iterator it_trans = transitions.begin(); it_trans != transitions.end(); it_trans++)
	{
		if ((*it_trans) == transition_to_delete)
		{
			transition_to_delete->DeleteAllConditions();
			transitions.erase(it_trans);
			delete transition_to_delete;
			return true;
		}
	}

	return false;
}

bool FSM_State::CheckTriggeredTransition(FSM_Transition** transition) const
{
	for (std::vector<FSM_Transition*>::const_iterator it = transitions.begin(); it != transitions.end(); it++)
		if ((*it)->IsTriggered(this))
		{
			*transition = *it;
			return true;
		}
	return false;
}

bool FSM_State::DisplayTransitionsInfo(FSM_Transition** selected_transition, FSM_Condition** selected_condition)
{
	bool candidate_to_delete = false;
	bool transition_selected = false;
	bool open_new_condition_popup = false;
	int i = 0;

	for (std::vector<FSM_Transition*>::const_iterator it_transitions = transitions.begin(); it_transitions != transitions.end(); it_transitions++, i++)
	{
		ImGui::Text("     - Transition %i:", i);
		if (ImGui::IsItemHovered() && (ImGui::IsMouseClicked(1) || ImGui::IsMouseClicked(0)))
		{
			*selected_transition = *it_transitions;
			candidate_to_delete = true;
			transition_selected = true;
		}
		ImGui::Text("     - Target State: %s", (*it_transitions)->GetTargetState()->GetStateName());
		if (ImGui::IsItemHovered() && (ImGui::IsMouseClicked(1) || ImGui::IsMouseClicked(0)))
		{
			*selected_transition = *it_transitions;
			candidate_to_delete = true;
			transition_selected = true;
		}
		ImGui::Text("          - Conditions (%i):", (*it_transitions)->GetNumConditions());
		if ((*it_transitions)->DisplayConditionsInfo(selected_condition))
		{
			*selected_transition = *it_transitions;
			transition_selected = true;
		}
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
		{
			*selected_transition = *it_transitions;
			transition_selected = true;
			open_new_condition_popup = true;
		}
	}

	if (open_new_condition_popup)
		ImGui::OpenPopup("Create New Condition");

	if (candidate_to_delete)
		ImGui::OpenPopup("Erase Transition Popup");

	return transition_selected;
}

bool FSM_State::ShowScriptInfo()
{
	if (script == nullptr)
		return false;

	if (ImGui::TreeNodeEx("Script Info:", ImGuiTreeNodeFlags_DefaultOpen))
	{
		script->ShowFSMInspectorInfo();
		ImGui::TreePop();
	}

	return true;
}

void FSM_State::SetStateName(const char * new_name)
{
	state_name = new_name;
}

int FSM_State::GetStateID() const
{
	return state_id;
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

CompScript * FSM_State::GetScript() const
{
	if (script != nullptr)
		return script;

	return nullptr;
}

FSM_Transition::FSM_Transition() : target_state(nullptr), target_state_id(-1)
{
}

FSM_Transition::FSM_Transition(FSM_State * target_state_) : target_state(target_state_), target_state_id(target_state_->GetStateID())
{
}

FSM_Transition::~FSM_Transition()
{
	target_state = nullptr;
	for (std::vector<FSM_Condition*>::iterator it = conditions.begin(); it != conditions.end(); it++)
		delete *it;
}

void FSM_Transition::SaveTransition(JSON_Object * object, std::string name, bool saveScene, uint & countResources, int state_num, int transition_num)
{
	json_object_dotset_number_with_std(object, name + "Target State ID", target_state->GetStateID());

	int num_conditions = 0;
	for (std::vector<FSM_Condition*>::const_iterator it_conditions = conditions.begin(); it_conditions != conditions.end(); it_conditions++)
	{
		num_conditions++;
		json_object_dotset_number_with_std(object, name + "Condition Type Condition" + std::to_string(num_conditions), (*it_conditions)->GetConditionType());
		(*it_conditions)->SaveCondition(object, name, saveScene, countResources);
	}
	json_object_dotset_number_with_std(object, name + "Number Conditions State" + std::to_string(state_num) + "Transition" + std::to_string(transition_num), num_conditions);
}

void FSM_Transition::LoadTransition(const JSON_Object * object, std::string name, int num_state, int num_transition, FSM_State* state_loading)
{
	// The target state must be applied later because the state may not be created yet
	target_state_id = json_object_dotget_number_with_std(object, name + "Target State ID");

	int num_conditions = json_object_dotget_number_with_std(object, name + "Number Conditions State" + std::to_string(num_state) + "Transition" + std::to_string(num_transition));
	for (int i = 1; i <= num_conditions; i++)
	{
		FSM_CONDITION_TYPE condition_type = (FSM_CONDITION_TYPE)(int)json_object_dotget_number_with_std(object, name + "Condition Type Condition" + std::to_string(num_conditions));
		FSM_ConditionBool * new_condition = new FSM_ConditionBool(true);
		switch (condition_type)
		{
		case FSM_COND_NONE:
			break;
		case FSM_COND_BOOL:
			new_condition->LoadCondition(object, name, state_loading);
			conditions.push_back(new_condition);
			break;
		case FSM_COND_EQUAL_INT:
			//TODO: Rest of cases:
			break;
		case FSM_COND_GREATER_THAN_INT:
			break;
		case FSM_COND_GREATER_EQUAL_INT:
			break;
		case FSM_COND_LOWER_THAN_INT:
			break;
		case FSM_COND_LOWER_EQUAL_INT:
			break;
		case FSM_COND_EQUAL_FLOAT:
			break;
		case FSM_COND_GREATER_THAN_FLOAT:
			break;
		case FSM_COND_GREATER_EQUAL_FLOAT:
			break;
		case FSM_COND_LOWER_THAN_FLOAT:
			break;
		case FSM_COND_LOWER_EQUAL_FLOAT:
			break;
		case FSM_COND_MAX:
			break;
		default:
			break;
		}
	}
}

FSM_Condition * FSM_Transition::AddCondition(FSM_CONDITION_TYPE condition_type, bool condition_b)
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
		new_condition = new FSM_ConditionBool(condition_b);
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

FSM_Condition * FSM_Transition::AddCondition(FSM_CONDITION_TYPE condition_type, int condition_a, int condition_b)
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
		new_condition = new FSM_ConditionEqualInt(condition_a, condition_b);
		break;
	case FSM_COND_GREATER_THAN_INT:
		new_condition = new FSM_ConditionGreaterThanInt(condition_a, condition_b);
		break;
	case FSM_COND_GREATER_EQUAL_INT:
		new_condition = new FSM_ConditionGreaterEqualInt(condition_a, condition_b);
		break;
	case FSM_COND_LOWER_THAN_INT:
		new_condition = new FSM_ConditionLowerThanInt(condition_a, condition_b);
		break;
	case FSM_COND_LOWER_EQUAL_INT:
		new_condition = new FSM_ConditionLowerEqualInt(condition_a, condition_b);
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

FSM_Condition * FSM_Transition::AddCondition(FSM_CONDITION_TYPE condition_type, float condition_a, float condition_b)
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
		new_condition = new FSM_ConditionEqualFloat(condition_a, condition_b);
		break; 
	case FSM_COND_GREATER_THAN_FLOAT:
		new_condition = new FSM_ConditionGreaterThanFloat(condition_a, condition_b);
		break;
	case FSM_COND_GREATER_EQUAL_FLOAT:
		new_condition = new FSM_ConditionGreaterEqualFloat(condition_a, condition_b);
		break;
	case FSM_COND_LOWER_THAN_FLOAT:
		new_condition = new FSM_ConditionLowerThanFloat(condition_a, condition_b);
		break;
	case FSM_COND_LOWER_EQUAL_FLOAT:
		new_condition = new FSM_ConditionLowerEqualFloat(condition_a, condition_b);
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

bool FSM_Transition::IsTriggered(const FSM_State* current_state)const
{
	bool ret = true;
	for (std::vector<FSM_Condition*>::const_iterator it = conditions.begin(); it != conditions.end(); it++)
		if (!(*it)->Test(current_state))
		{
			ret = false;
			break;
		}
	return ret;
}

void FSM_Transition::CreateConditionsModifyingOptions(FSM_State* selected_state)
{
	int id = 0;
	int id2 = 0;
	for (std::vector<FSM_Condition*>::const_iterator it = conditions.begin(); it != conditions.end(); it++, id++,id2++)
	{
		CompScript* state_script = selected_state->GetScript();

		if (state_script == nullptr)
		{
			LOG("Need a script to get its variables");
			return;
		}

		ScriptVariable* condition_a = nullptr;
		bool bool_b = false;
		int i = 0;
		float f = 0.0f;

		ImGui::PushID(id);		
		switch ((*it)->GetConditionType())
		{
		case FSM_COND_NONE:
			break;

		case FSM_COND_BOOL:
			condition_a = ((FSM_ConditionBool*)(*it))->GetConditionA();
			bool_b = ((FSM_ConditionBool*)(*it))->GetConditionB();
			
			ImGui::PushID(id2);
			if (ImGui::BeginCombo("##SelectVariable", (*it)->variable_a_name))
			{
				for (std::vector<ScriptVariable*>::iterator it_variables = state_script->csharp->variables.begin(); it_variables != state_script->csharp->variables.end(); it_variables++)
				{
					if ((*it_variables)->type == VarType::Var_BOOL)
					{
						bool is_selected = ((*it)->variable_a_name == (*it_variables)->name);
						if (ImGui::Selectable((*it_variables)->name, is_selected))
						{
							(*it)->variable_a_name = (*it_variables)->name;
							condition_a = (*it_variables);
							((FSM_ConditionBool*)(*it))->SetScriptVariable(&(*it_variables));
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
				}				
				ImGui::EndCombo();
			}
			ImGui::PopID();

			if (!add_second_variable_from_script)
			{
				ImGui::SameLine();
				ImGui::PushID(id2);
				if (ImGui::Checkbox("True", &bool_b))
					(*it)->SetConditionB(bool_b);

				ImGui::PopID();
			}
			ImGui::PushID(id2);
			if (ImGui::Button("Save Condition"))
				((FSM_ConditionBool*)*it)->SetConditionB(bool_b);
			ImGui::PopID();

			ImGui::SameLine();
			App->ShowHelpMarker("Conditions need to be saved before Confirming them");
			break;
/*
// TODO
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
*/
		case FSM_COND_MAX:
			break;

		default:
			break;
		}		
		ImGui::PopID();
	}
}

bool FSM_Transition::DisplayConditionsInfo(FSM_Condition** selected_condition)
{
	bool candidate_to_delete = false;
	int i = 0;
	for (std::vector<FSM_Condition*>::const_iterator it_conditions = conditions.begin(); it_conditions != conditions.end(); it_conditions++, i++)
	{
		ImGui::Text("               - Condition %i:", i);
		if (ImGui::IsItemHovered() && (ImGui::IsMouseClicked(1) || ImGui::IsMouseClicked(0)))
		{
			*selected_condition = *it_conditions;
			candidate_to_delete = true;
		}
		ImGui::Text("                    - %s", (*it_conditions)->GetConditionTypeStr());
		if (ImGui::IsItemHovered() && (ImGui::IsMouseClicked(1) || ImGui::IsMouseClicked(0)))
		{
			*selected_condition = *it_conditions;
			candidate_to_delete = true;
		}
	}

	if (candidate_to_delete)
	{
		ImGui::OpenPopup("Erase Condition Popup");
		return true;
	}
	return false;
}

bool FSM_Transition::DeleteAllConditions()
{
	if (conditions.size() == 0)
		return false;

	for (std::vector<FSM_Condition*>::const_iterator it_cond = conditions.begin(); it_cond != conditions.end(); it_cond++)
		delete (*it_cond);

	conditions.clear();

	return true;
}

bool FSM_Transition::DeleteCondition(FSM_Condition* condition_to_delete)
{
	if (condition_to_delete == nullptr)
		return false;

	for (std::vector<FSM_Condition*>::const_iterator it_cond = conditions.begin(); it_cond != conditions.end(); it_cond++)
	{
		if ((*it_cond) == condition_to_delete)
		{
			conditions.erase(it_cond);
			delete condition_to_delete;
			return true;
		}
	}

	return false;
}

void FSM_Transition::SetTargetState(FSM_State* new_target_state)
{
	target_state = new_target_state;
	target_state_id = new_target_state->GetStateID();
}

int FSM_Transition::GetTargetStateID()
{
	return target_state_id;
}

FSM_State * FSM_Transition::GetTargetState()
{
	return target_state;
}

uint FSM_Transition::GetNumConditions() const
{
	return conditions.size();
}

FSM_Condition::FSM_Condition() : condition_type(FSM_CONDITION_TYPE::FSM_COND_NONE)
{
}

FSM_Condition::FSM_Condition(FSM_CONDITION_TYPE condition_type_) : condition_type(condition_type_)
{
}

FSM_Condition::~FSM_Condition()
{
}

void FSM_Condition::SetConditionType(FSM_CONDITION_TYPE condition_type_)
{
	condition_type = condition_type_;
}

FSM_CONDITION_TYPE FSM_Condition::GetConditionType() const
{
	return condition_type;
}

const char * FSM_Condition::GetConditionTypeStr() const
{
	switch (condition_type)
	{
	case FSM_COND_NONE:
		return "Condition type ERROR";
		break;
	case FSM_COND_BOOL:
		return "Bool";
		break;
	case FSM_COND_EQUAL_INT:
		return "Equal Int";
		break;
	case FSM_COND_GREATER_THAN_INT:
		return "Greater Int";
		break;
	case FSM_COND_GREATER_EQUAL_INT:
		return "Greater or Equal Int";
		break;
	case FSM_COND_LOWER_THAN_INT:
		return "Lower Int";
		break;
	case FSM_COND_LOWER_EQUAL_INT:
		return "Lower or Equal Int";
		break;
	case FSM_COND_EQUAL_FLOAT:
		return "Equal Float";
		break;
	case FSM_COND_GREATER_THAN_FLOAT:
		return "Greater Float";
		break;
	case FSM_COND_GREATER_EQUAL_FLOAT:
		return "Greater or Equal Float";
		break;
	case FSM_COND_LOWER_THAN_FLOAT:
		return "Lower Float";
		break;
	case FSM_COND_LOWER_EQUAL_FLOAT:
		return "Lower or Equal Float";
		break;
	case FSM_COND_MAX:
	default:
		return "Condition type ERROR";
		break;
	}

	return "Condition type ERROR";
}

FSM_ConditionBool::FSM_ConditionBool(bool condition_b_) : FSM_Condition(FSM_COND_BOOL), condition_b(condition_b_), condition_script_a(nullptr)
{
}

FSM_ConditionBool::~FSM_ConditionBool()
{
}

void FSM_ConditionBool::SaveCondition(JSON_Object * object, std::string name, bool saveScene, uint & countResources)
{
	json_object_dotset_boolean_with_std(object, name + "Condition B", condition_b);
}

void FSM_ConditionBool::LoadCondition(const JSON_Object * object, std::string name, FSM_State* loading_state)
{
	condition_b = json_object_dotget_boolean_with_std(object, name + "Condition B");

	for (std::vector<ScriptVariable*>::iterator it_variables = loading_state->GetScript()->csharp->variables.begin(); it_variables != loading_state->GetScript()->csharp->variables.end(); it_variables++)
		if ((*it_variables)->type == VarType::Var_BOOL)
			SetScriptVariable(&(*it_variables));
}

bool FSM_ConditionBool::Test(const FSM_State* current_state)
{
	return *(bool*)condition_script_a->value == condition_b;
}

bool FSM_ConditionBool::SetScriptVariable(ScriptVariable ** script_variable)
{
	if (script_variable == nullptr)
		return false;

	condition_script_a = *script_variable;
	return true;
}

void FSM_ConditionBool::SetConditionB(bool condition_b_)
{
	condition_b = condition_b_;
}

ScriptVariable* FSM_ConditionBool::GetConditionA() const
{
	return condition_script_a;
}

bool FSM_ConditionBool::GetConditionB() const
{
	return condition_b;
}

FSM_ConditionEqualInt::FSM_ConditionEqualInt(int condition_a_, int condition_b_) : FSM_Condition(FSM_COND_EQUAL_INT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionEqualInt::~FSM_ConditionEqualInt()
{
}

bool FSM_ConditionEqualInt::Test()
{
	return condition_a == condition_b;
}

bool FSM_ConditionEqualInt::SetCondition(int condition_a_, int condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;	
	return true;
}

int FSM_ConditionEqualInt::GetConditionA() const
{
	return condition_a;
}

FSM_ConditionGreaterThanInt::FSM_ConditionGreaterThanInt(int condition_a_, int condition_b_) : FSM_Condition(FSM_COND_GREATER_THAN_INT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionGreaterThanInt::~FSM_ConditionGreaterThanInt()
{
}

bool FSM_ConditionGreaterThanInt::Test()
{
	return condition_a > condition_b;
}

bool FSM_ConditionGreaterThanInt::SetCondition(int condition_a_, int condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;
	return true;
}

int FSM_ConditionGreaterThanInt::GetConditionA() const
{
	return condition_a;
}

FSM_ConditionGreaterEqualInt::FSM_ConditionGreaterEqualInt(int condition_a_, int condition_b_) : FSM_Condition(FSM_COND_GREATER_EQUAL_INT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionGreaterEqualInt::~FSM_ConditionGreaterEqualInt()
{
}

bool FSM_ConditionGreaterEqualInt::Test()
{
	return condition_a >= condition_b;
}

bool FSM_ConditionGreaterEqualInt::SetCondition(int condition_a_, int condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;
	return true;
}

int FSM_ConditionGreaterEqualInt::GetConditionA() const
{
	return condition_a;
}

FSM_ConditionLowerThanInt::FSM_ConditionLowerThanInt(int condition_a_, int condition_b_) : FSM_Condition(FSM_COND_LOWER_THAN_INT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionLowerThanInt::~FSM_ConditionLowerThanInt()
{
}

bool FSM_ConditionLowerThanInt::Test()
{
	return condition_a < condition_b;
}

bool FSM_ConditionLowerThanInt::SetCondition(int condition_a_, int condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;
	return true;
}

int FSM_ConditionLowerThanInt::GetConditionA() const
{
	return condition_a;
}

FSM_ConditionLowerEqualInt::FSM_ConditionLowerEqualInt(int condition_a_, int condition_b_) : FSM_Condition(FSM_COND_LOWER_EQUAL_INT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionLowerEqualInt::~FSM_ConditionLowerEqualInt()
{
}

bool FSM_ConditionLowerEqualInt::Test()
{
	return condition_a <= condition_b;
}

bool FSM_ConditionLowerEqualInt::SetCondition(int condition_a_, int condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;
	return true;
}

int FSM_ConditionLowerEqualInt::GetConditionA() const
{
	return condition_a;
}

FSM_ConditionEqualFloat::FSM_ConditionEqualFloat(float condition_a_, float condition_b_) : FSM_Condition(FSM_COND_EQUAL_FLOAT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionEqualFloat::~FSM_ConditionEqualFloat()
{
}

bool FSM_ConditionEqualFloat::Test()
{
	return condition_a == condition_b;
}

bool FSM_ConditionEqualFloat::SetCondition(float condition_a_, float condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;
	return true;
}

float FSM_ConditionEqualFloat::GetConditionA() const
{
	return condition_a;
}

FSM_ConditionGreaterThanFloat::FSM_ConditionGreaterThanFloat(float condition_a_, float condition_b_) : FSM_Condition(FSM_COND_GREATER_THAN_FLOAT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionGreaterThanFloat::~FSM_ConditionGreaterThanFloat()
{
}

bool FSM_ConditionGreaterThanFloat::Test()
{
	return condition_a > condition_b;
}

bool FSM_ConditionGreaterThanFloat::SetCondition(float condition_a_, float condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;
	return true;
}

float FSM_ConditionGreaterThanFloat::GetConditionA() const
{
	return condition_a;
}

FSM_ConditionGreaterEqualFloat::FSM_ConditionGreaterEqualFloat(float condition_a_, float condition_b_) : FSM_Condition(FSM_COND_GREATER_EQUAL_FLOAT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionGreaterEqualFloat::~FSM_ConditionGreaterEqualFloat()
{
}

bool FSM_ConditionGreaterEqualFloat::Test()
{
	return condition_a >= condition_b;
}

bool FSM_ConditionGreaterEqualFloat::SetCondition(float condition_a_, float condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;
	return true;
}

float FSM_ConditionGreaterEqualFloat::GetConditionA() const
{
	return condition_a;
}

FSM_ConditionLowerThanFloat::FSM_ConditionLowerThanFloat(float condition_a_, float condition_b_) : FSM_Condition(FSM_COND_LOWER_THAN_FLOAT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionLowerThanFloat::~FSM_ConditionLowerThanFloat()
{
}

bool FSM_ConditionLowerThanFloat::Test()
{
	return condition_a < condition_b;
}

bool FSM_ConditionLowerThanFloat::SetCondition(float condition_a_, float condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;
	return true;
}

float FSM_ConditionLowerThanFloat::GetConditionA() const
{
	return condition_a;
}

FSM_ConditionLowerEqualFloat::FSM_ConditionLowerEqualFloat(float condition_a_, float condition_b_) : FSM_Condition(FSM_COND_LOWER_EQUAL_FLOAT), condition_a(condition_a_), condition_b(condition_b_)
{
}

FSM_ConditionLowerEqualFloat::~FSM_ConditionLowerEqualFloat()
{
}

bool FSM_ConditionLowerEqualFloat::Test()
{
	return condition_a <= condition_b;
}

bool FSM_ConditionLowerEqualFloat::SetCondition(float condition_a_, float condition_b_)
{
	condition_a = condition_a_;
	condition_b = condition_b_;
	return true;
}

float FSM_ConditionLowerEqualFloat::GetConditionA() const
{
	return condition_a;
}
