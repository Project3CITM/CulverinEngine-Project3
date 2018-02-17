#include "CompFSM.h"
#include "Application.h"
#include "WindowInspector.h"
#include "ModuleGUI.h"
#include "GameObject.h"
#include "Scene.h"

CompFiniteStateMachine::CompFiniteStateMachine(Comp_Type c_type, GameObject * parent) : Component(c_type, parent)
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
	{
		ImGui::OpenPopup("OptionsFSM");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsFSM"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}
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

FSM_State::FSM_State()
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

FSM_Transition * FSM_State::GetTransitions()
{
	return transitions[0];
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

bool FSM_Transition::IsTriggered()
{
	//TODO: Check all conditions
	for (std::vector<FSM_Condition*>::iterator it = conditions.begin(); it != conditions.end(); it++)
		return false;
	return false;
}

FSM_State * FSM_Transition::GetTargetState()
{
	return target_state;
}

FSM_Condition::FSM_Condition()
{
}

FSM_Condition::~FSM_Condition()
{
}

FSM_ConditionBool::FSM_ConditionBool(bool condition_) : condition(condition_)
{
}

FSM_ConditionBool::~FSM_ConditionBool()
{
}

bool FSM_ConditionBool::Test()
{
	return condition;
}

FSM_ConditionEqualInt::FSM_ConditionEqualInt(int condition_) : condition(condition_)
{
}

FSM_ConditionEqualInt::~FSM_ConditionEqualInt()
{
}

bool FSM_ConditionEqualInt::Test(int i)
{
	return i == condition;
}

FSM_ConditionGreaterThanInt::FSM_ConditionGreaterThanInt(int condition_) :condition(condition_)
{
}

FSM_ConditionGreaterThanInt::~FSM_ConditionGreaterThanInt()
{
}

bool FSM_ConditionGreaterThanInt::Test(int i)
{
	return i > condition;
}
