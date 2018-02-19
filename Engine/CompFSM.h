#ifndef COMPONENT_FINITE_STATE_MACHINE_H
#define COMPONENT_FINITE_STATE_MACHINE_H

#include "Component.h"
#include "CompScript.h"

class FSM_State;
class FSM_Transition;
class FSM_Condition;
enum FSM_CONDITION_TYPE;

class CompFiniteStateMachine : public Component
{
public:
	CompFiniteStateMachine(Comp_Type c_type, GameObject* parent);
	CompFiniteStateMachine(const CompFiniteStateMachine& copy,GameObject* parent);
	~CompFiniteStateMachine();

	//Game loop ---------
	void Start();
	void Update(float dt);
	void Clear();
	// ------------------------

	// EDITOR METHODS ---------
	void ShowOptions();
	void ShowInspectorInfo();
	// ------------------------

	void CopyValues(const CompFiniteStateMachine * component);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------

	FSM_State* CreateState();

	// SETTERS ----------------
	bool SetCurrentState(FSM_State* new_current_state);
	bool SetInitialState(FSM_State* new_initial_state);
	// ------------------------

	// GETTERS ----------------
	uint GetNumStates()const;
	FSM_State* GetCurrentState()const;
	FSM_State* GetInitialState()const;
	// ------------------------

private:
	CompScript* SelectScript(FSM_State* state_to_add_script);
	void CheckOpenStateOptions(FSM_State* state);

	// ----- Needed Updating States -----//
	std::vector<FSM_State*> states;
	FSM_State* initial_state;
	FSM_State* current_state;
	// ----- Needed Updating States -----//

	// ----- Needed Creating States ----- //
	FSM_State* selected_state;
	FSM_State* target_state;		//for creating transitions
	FSM_Transition* new_transition;	//When creating a new transition, in order to fill all the conditions
	// ----- Needed Creating states ----- //

	// ----- Visual Scripting ----- //
	bool show_fsm;
	bool show_create_transition_window;
	bool show_select_script_window;
	std::vector<int> new_conditions;	//Saves the type and number of the new condition that are being created
	std::vector<int>::iterator condition_to_erase;
	bool show_create_conditions_window;
	FSM_State* candidate_state_to_delete;
	FSM_Transition* candidate_transition_to_delete;
	// ----- Visual Scripting ----- //
};

// --------------------------------  STATES  -------------------------------- //

class FSM_State
{
public:
	FSM_State();
	FSM_State(const FSM_State& copy);	//doesn't copy the transitions, makes no sense
	~FSM_State();

	bool StartScripts();
	//TODO
	void DoEntryAction();
	void DoAction(float dt);
	void DoExitAction();

	bool AddScript(CompScript* script_to_add);	//Only support 1 script per node yet
	FSM_Transition* AddTransition(FSM_State* target_state);
	bool DeleteTransition(FSM_Transition* transition_to_delete);
	bool CheckTriggeredTransition(FSM_Transition* transition)const;

	// ----- Visual Scripting ----- //
	FSM_Transition* DisplayTransitionsInfo();	//returns a transition if it must be deleted
	// ----- Visual Scripting ----- //

	// SETTERS ----------------
	void SetStateName(const char* new_name);
	// ------------------------

	// GETTERS ----------------
	uint GetNumTransitions()const;
	const char* GetStateName()const;
	const char* GetScriptName()const;
	// ------------------------

private:	
	std::string state_name;
	CompScript* script;
	std::vector<FSM_Transition*> transitions;
};

// --------------------------------  STATES  -------------------------------- //

// --------------------------------  TRANSITIONS  -------------------------------- //

class FSM_Transition
{
public:
	FSM_Transition(FSM_State* target_state_);
	~FSM_Transition();

	// TODO: Should Ints/Floats transform in each other in case of using wrong function to avoid errors ???
	FSM_Condition* AddCondition(FSM_CONDITION_TYPE condition_type, bool condition);
	FSM_Condition* AddCondition(FSM_CONDITION_TYPE condition_type, int condition);
	FSM_Condition* AddCondition(FSM_CONDITION_TYPE condition_type, float condition);

	bool IsTriggered()const;
	FSM_State* GetTargetState();

	// --- Visual Scripting --- //
	void CreateConditionsModifyingOptions();
	void DisplayConditionsInfo();
	// --- Visual Scripting --- //

	// GETTERS ----------------
	uint GetNumConditions()const;
	// ------------------------

private:
	FSM_State* target_state;
	std::vector<FSM_Condition*> conditions;
};

// --------------------------------  TRANSITIONS  -------------------------------- //

// --------------------------------  CONDITIONS  -------------------------------- //

enum FSM_CONDITION_TYPE
{
	FSM_COND_NONE = -1,
	FSM_COND_BOOL = 0,
	FSM_COND_EQUAL_INT,
	FSM_COND_GREATER_THAN_INT,
	FSM_COND_GREATER_EQUAL_INT,
	FSM_COND_LOWER_THAN_INT,
	FSM_COND_LOWER_EQUAL_INT,
	FSM_COND_EQUAL_FLOAT,
	FSM_COND_GREATER_THAN_FLOAT,
	FSM_COND_GREATER_EQUAL_FLOAT,
	FSM_COND_LOWER_THAN_FLOAT,
	FSM_COND_LOWER_EQUAL_FLOAT,
	FSM_COND_MAX
};

class FSM_Condition
{
public:
	FSM_Condition(FSM_CONDITION_TYPE condition_type_);
	~FSM_Condition();

	virtual bool Test(bool b)	{ return false; };
	virtual bool Test(int i)	{ return false; };
	virtual bool Test(float f)	 { return false; };

	// SETTERS ----------------
	virtual bool SetCondition(bool condition_)	{ return false; }
	virtual bool SetCondition(int condition_)	{ return false; }
	virtual bool SetCondition(float condition_)	{ return false; }
	// ------------------------
	
	// GETTERS ----------------
	FSM_CONDITION_TYPE GetConditionType()const;
	// ------------------------

private:
	FSM_CONDITION_TYPE condition_type;
};

	// BOOL ----------------

struct FSM_ConditionBool : public FSM_Condition
{
public:
	FSM_ConditionBool(bool condition_);
	~FSM_ConditionBool();

	bool Test(bool b);

	// SETTERS ----------------
	bool SetCondition(bool condition_);
	// ------------------------

	// GETTERS ----------------
	bool GetCondition()const;
	// ------------------------

private:
	bool condition;
};

	// ------------------------
	// INTEGERS ----------------

struct FSM_ConditionEqualInt : public FSM_Condition
{
public:
	FSM_ConditionEqualInt(int condition_);
	~FSM_ConditionEqualInt();

	bool Test(int i);

	// SETTERS ----------------
	bool SetCondition(int condition_);
	// ------------------------

	// GETTERS ----------------
	int GetCondition()const;
	// ------------------------

private:
	int condition;
};

struct FSM_ConditionGreaterThanInt : public FSM_Condition
{
public:
	FSM_ConditionGreaterThanInt(int condition_);
	~FSM_ConditionGreaterThanInt();

	bool Test(int i);

	// SETTERS ----------------
	bool SetCondition(int condition_);
	// ------------------------

	// GETTERS ----------------
	int GetCondition()const;
	// ------------------------

private:
	int condition;
};

struct FSM_ConditionGreaterEqualInt : public FSM_Condition
{
public:
	FSM_ConditionGreaterEqualInt(int condition_);
	~FSM_ConditionGreaterEqualInt();

	bool Test(int i);

	// SETTERS ----------------
	bool SetCondition(int condition_);
	// ------------------------

	// GETTERS ----------------
	int GetCondition()const;
	// ------------------------

private:
	int condition;
};

struct FSM_ConditionLowerThanInt : public FSM_Condition
{
public:
	FSM_ConditionLowerThanInt(int condition_);
	~FSM_ConditionLowerThanInt();

	bool Test(int i);

	// SETTERS ----------------
	bool SetCondition(int condition_);
	// ------------------------

	// GETTERS ----------------
	int GetCondition()const;
	// ------------------------

private:
	int condition;
};

struct FSM_ConditionLowerEqualInt : public FSM_Condition
{
public:
	FSM_ConditionLowerEqualInt(int condition_);
	~FSM_ConditionLowerEqualInt();

	bool Test(int i);

	// SETTERS ----------------
	bool SetCondition(int condition_);
	// ------------------------

	// GETTERS ----------------
	int GetCondition()const;
	// ------------------------

private:
	int condition;
};

	// ------------------------
	// FLOATS ----------------

struct FSM_ConditionEqualFloat : public FSM_Condition
{
public:
	FSM_ConditionEqualFloat(float condition_);
	~FSM_ConditionEqualFloat();

	bool Test(float i);

	// SETTERS ----------------
	bool SetCondition(float condition_);
	// ------------------------

	// GETTERS ----------------
	float GetCondition()const;
	// ------------------------

private:
	float condition;
};

struct FSM_ConditionGreaterThanFloat : public FSM_Condition
{
public:
	FSM_ConditionGreaterThanFloat(float condition_);
	~FSM_ConditionGreaterThanFloat();

	bool Test(float i);

	// SETTERS ----------------
	bool SetCondition(float condition_);
	// ------------------------

	// GETTERS ----------------
	float GetCondition()const;
	// ------------------------

private:
	float condition;
};

struct FSM_ConditionGreaterEqualFloat : public FSM_Condition
{
public:
	FSM_ConditionGreaterEqualFloat(float condition_);
	~FSM_ConditionGreaterEqualFloat();

	bool Test(float i);

	// SETTERS ----------------
	bool SetCondition(float condition_);
	// ------------------------

	// GETTERS ----------------
	float GetCondition()const;
	// ------------------------

private:
	float condition;
};

struct FSM_ConditionLowerThanFloat : public FSM_Condition
{
public:
	FSM_ConditionLowerThanFloat(float condition_);
	~FSM_ConditionLowerThanFloat();

	bool Test(float i);

	// SETTERS ----------------
	bool SetCondition(float condition_);
	// ------------------------

	// GETTERS ----------------
	float GetCondition()const;
	// ------------------------

private:
	float condition;
};

struct FSM_ConditionLowerEqualFloat : public FSM_Condition
{
public:
	FSM_ConditionLowerEqualFloat(float condition_);
	~FSM_ConditionLowerEqualFloat();

	bool Test(float i);

	// SETTERS ----------------
	bool SetCondition(float condition_);
	// ------------------------

	// GETTERS ----------------
	float GetCondition()const;
	// ------------------------

private:
	float condition;
};
	// ------------------------

// --------------------------------  CONDITIONS  -------------------------------- //

#endif // !COMPONENT_FINITE_STATE_MACHINE_H