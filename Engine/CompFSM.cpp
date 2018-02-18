#include "CompFSM.h"
#include "Application.h"
#include "WindowInspector.h"
#include "ModuleGUI.h"
#include "GameObject.h"
#include "Scene.h"

CompFiniteStateMachine::CompFiniteStateMachine(Comp_Type c_type, GameObject * parent) : Component(c_type, parent), initial_state(nullptr), current_state(nullptr), selected_state(nullptr), show_fsm(false)
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

			for (std::vector<FSM_State*>::const_iterator it = states.begin(); it != states.end(); it++)
			{
				ImGui::Text("State %s", (*it)->GetScriptName());

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
				{
					selected_state = (*it);
					ImGui::OpenPopup("New Transition Popup");
				}
				else if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
					selected_state = (*it);

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
			}
		}

		// --------------------------------  POPUPS  -------------------------------- //
		if (ImGui::BeginPopup("New State Popup"))
		{
			if (ImGui::Button("Create New State"))
			{
				CreateState();
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopup("New Transition Popup"))
		{
			if (ImGui::Button("Create New Transition"))
			{
				//TODO stateX->AddTransition(target_stateY);
			}
			if (ImGui::Button("Set as initial state"))
			{
				SetInitialState(selected_state);
			}
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

FSM_State * CompFiniteStateMachine::GetCurrentState() const
{
	return current_state;
}

FSM_State * CompFiniteStateMachine::GetInitialState() const
{
	return initial_state;
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

uint FSM_State::GetNumTransitions() const
{
	return transitions.size();
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
	delete target_state;
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
		break;
	}

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
		break;
	}

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
		break;
	}

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
//				return true;
			break;
		case FSM_COND_EQUAL_INT:
		case FSM_COND_GREATER_THAN_INT:
		case FSM_COND_GREATER_EQUAL_INT:
		case FSM_COND_LOWER_THAN_INT:
		case FSM_COND_LOWER_EQUAL_INT:
//				return true; 
			break;
		case FSM_COND_EQUAL_FLOAT:
		case FSM_COND_GREATER_THAN_FLOAT:
		case FSM_COND_GREATER_EQUAL_FLOAT:
		case FSM_COND_LOWER_THAN_FLOAT:
		case FSM_COND_LOWER_EQUAL_FLOAT:
//				return true; 
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

FSM_Condition::FSM_Condition(FSM_CONDITION_TYPE condition_type)
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