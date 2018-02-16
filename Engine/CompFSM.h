#ifndef COMPONENT_FINITE_STATE_MACHINE_H
#define COMPONENT_FINITE_STATE_MACHINE_H

#include "Component.h"
#include "CompScript.h"

class FSM_State;
class FSM_Transition;
class FSM_Condition;

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

	FSM_State* CreateState();	//TODO

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
	FSM_State(const FSM_State& copy);	//doesn't copy the transitions
	~FSM_State();

	//TODO
	void DoEntryAction();
	void DoAction();
	void DoExitAction();

	bool AddScript(CompScript* script_to_add);	//TODO
	FSM_Transition* AddTransition();	//TODO

	FSM_Transition* GetTransitions();

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

	FSM_Condition* AddCondition();	//TODO

	bool IsTriggered();
	FSM_State* GetTargetState();

private:
	FSM_State* target_state;
	std::vector<FSM_Condition*> conditions;
};

// --------------------------------  TRANSITIONS  -------------------------------- //

// --------------------------------  CONDITIONS  -------------------------------- //

class FSM_Condition
{
public:
	FSM_Condition();
	~FSM_Condition();

	virtual bool Test()			{ return false; };
	virtual bool Test(int i)	{ return false; };
	virtual bool Test(float f)	{ return false; };
};

struct FSM_ConditionBool : public FSM_Condition
{
public:
	FSM_ConditionBool(bool condition_);
	~FSM_ConditionBool();

	bool Test();

private:
	bool condition;
};

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

//TODO:
// Conditions: 
	// AND / OR / NOT
	// GreaterOrEqualThanInt
	// LowerThanInt
	// LowerOrEqualThanInt
	// GreaterOrEqualThanFloat
	// LowerThanFloat
	// LowerOrEqualThanFloat


// --------------------------------  CONDITIONS  -------------------------------- //

#endif // !COMPONENT_FINITE_STATE_MACHINE_H