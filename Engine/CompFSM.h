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
	FSM_State* GetCurrentState()const;
	FSM_State* GetInitialState()const;
	// ------------------------

private:
	std::vector<FSM_State*> states;
	FSM_State* initial_state;
	FSM_State* current_state;
};

// --------------------------------  STATES  -------------------------------- //

class FSM_State
{
public:
	FSM_State();
	FSM_State(const FSM_State& copy);	//doesn't copy the transitions, makes no sense
	~FSM_State();

	//TODO
	void DoEntryAction();
	void DoAction();
	void DoExitAction();

	bool AddScript(CompScript* script_to_add);	//Only support 1 script per node yet
	FSM_Transition* AddTransition(FSM_State* target_state);

	// GETTERS ----------------
	FSM_Transition* GetTransitions()const;
	uint GetNumTransitions()const;
	// ------------------------

private:

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

	FSM_Condition* AddCondition(FSM_CONDITION_TYPE condition_type, bool condition);
	FSM_Condition* AddCondition(FSM_CONDITION_TYPE condition_type, int condition);
	FSM_Condition* AddCondition(FSM_CONDITION_TYPE condition_type, float condition);

	bool IsTriggered();
	FSM_State* GetTargetState();

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
	FSM_Condition(FSM_CONDITION_TYPE condition_type);
	~FSM_Condition();

	virtual bool Test() { return false; };
	virtual bool Test(int i) { return false; };
	virtual bool Test(float f) { return false; };

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

	bool Test();

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

private:
	int condition;
};

struct FSM_ConditionGreaterThanInt : public FSM_Condition
{
public:
	FSM_ConditionGreaterThanInt(int condition_);
	~FSM_ConditionGreaterThanInt();

	bool Test(int i);

private:
	int condition;
};

struct FSM_ConditionGreaterEqualInt : public FSM_Condition
{
public:
	FSM_ConditionGreaterEqualInt(int condition_);
	~FSM_ConditionGreaterEqualInt();

	bool Test(int i);

private:
	int condition;
};

struct FSM_ConditionLowerThanInt : public FSM_Condition
{
public:
	FSM_ConditionLowerThanInt(int condition_);
	~FSM_ConditionLowerThanInt();

	bool Test(int i);

private:
	int condition;
};

struct FSM_ConditionLowerEqualInt : public FSM_Condition
{
public:
	FSM_ConditionLowerEqualInt(int condition_);
	~FSM_ConditionLowerEqualInt();

	bool Test(int i);

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

private:
	float condition;
};

struct FSM_ConditionGreaterThanFloat : public FSM_Condition
{
public:
	FSM_ConditionGreaterThanFloat(float condition_);
	~FSM_ConditionGreaterThanFloat();

	bool Test(float i);

private:
	float condition;
};

struct FSM_ConditionGreaterEqualFloat : public FSM_Condition
{
public:
	FSM_ConditionGreaterEqualFloat(float condition_);
	~FSM_ConditionGreaterEqualFloat();

	bool Test(float i);

private:
	float condition;
};

struct FSM_ConditionLowerThanFloat : public FSM_Condition
{
public:
	FSM_ConditionLowerThanFloat(float condition_);
	~FSM_ConditionLowerThanFloat();

	bool Test(float i);

private:
	float condition;
};

struct FSM_ConditionLowerEqualFloat : public FSM_Condition
{
public:
	FSM_ConditionLowerEqualFloat(float condition_);
	~FSM_ConditionLowerEqualFloat();

	bool Test(float i);

private:
	float condition;
};
	// ------------------------

// --------------------------------  CONDITIONS  -------------------------------- //

#endif // !COMPONENT_FINITE_STATE_MACHINE_H