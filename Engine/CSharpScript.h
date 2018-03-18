#ifndef _CSHARPSCRIPT_
#define _CSHARPSCRIPT_

// Info-> http://docs.go-mono.com/?link=root:/embed
// new Info? http://docs.go-mono.com/index.aspx?link=root:/classlib

#define DefaultParam 0

#include "Globals.h"
#include <string>
#include <vector>
#include <map>
#include <mono/metadata/metadata.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>


//std::string names = mono_class_get_name(mono_object_get_class(gameObject)); Name Class

class CSharpScript;
class GameObject;
class Component;
class CompScript;
typedef struct json_object_t JSON_Object;

enum FunctionBase
{
	CS_Start, 
	CS_Update, 
	CS_FixedUpdate, 
	CS_OnGUI, 
	CS_OnEnable, 
	CS_OnDisable, 
	CS_OnTriggerEnter, 
	CS_OnTriggerLost, 
	CS_OnContact,
	CS_OnClick
};

enum VarType
{
	Var_UNKNOWN = -1,
	Var_INT = 0,
	Var_FLOAT = 1,
	Var_BOOL = 2,
	Var_STRING = 3,
	Var_CLASS = 4,
	Var_GAMEOBJECT = 5
};

enum VarAccess
{
	Var_PUBLIC = 0,
	Var_PRIVATE,
};

class ScriptVariable
{
public:
	ScriptVariable(const char* name, VarType type, VarAccess access, CSharpScript* script);
	virtual ~ScriptVariable();

	void SetMonoValue(void* newVal);
	void EreaseMonoValue(void* newVal);
	void SetMonoField(MonoClassField* mfield);
	void SetMonoType(MonoType* mtype);

	//SAVE - LOAD ---------------
	void Save(JSON_Object* object, const std::string& name);
	void Load(const JSON_Object* object, const std::string& name, std::vector<uint>& re_load_values);

public:
	const char* name = nullptr;
	VarType type = Var_UNKNOWN;
	void* value = nullptr;
	std::string str_value;
	GameObject* game_object = nullptr;
	bool select_game_object = false; //Only use for GameObjects
	VarAccess access = VarAccess::Var_PRIVATE;

private:
	//Mono properties to link with he script
	MonoClassField* mono_field = nullptr;
	MonoType* mono_type = nullptr;

	//To access the script
	CSharpScript* script = nullptr;
};


struct MainMonoMethod
{
	FunctionBase type;
	MonoMethod* method = nullptr;
};

class CSharpScript
{
public:
	CSharpScript();
	~CSharpScript();

	//Importing/Reimporting/Initializing --------
	void LoadScript();
	bool ReImport(std::string pathdll, std::string nameClass = NULL);
	void Clear();
	MainMonoMethod CreateMainFunction(std::string function, int parameters, FunctionBase type);
	void DoMainFunction(FunctionBase function, void** parameters = nullptr);
	void DoFunction(MonoMethod* function, void ** parameter);

	bool CheckMonoObject(MonoObject* object);

	//GET functions ------------------------------------
	MonoObject* GetMonoObject() const;
	void SetMonoObject(MonoObject* new_object);
	MonoClass* GetMonoClass() const;

	// Functions set initial info ------------------------
	void SetDomain(MonoDomain* domain);
	void SetAssembly(MonoAssembly* assembly);
	void SetImage(MonoImage* image);
	void SetClass(MonoClass* klass);
	void SetClassName(std::string _name);
	void SetNameSpace(std::string _name_space);

	//Variables METHODS -------------------------------------------------
	void ResetScriptVariables();
	void CreateOwnGameObject();
	void GetScriptVariables();
	void UpdateScriptVariables();
	void RemoveReferences(GameObject* go);
	bool NeedToLinkGO() const;

	VarType GetTypeFromMono(MonoType* mtype);
	bool GetValueFromMono(ScriptVariable* variable, MonoClassField* mfield, MonoType* mtype);
	bool UpdateValueFromMono(ScriptVariable* variable, MonoClassField* mfield, MonoType* mtype);
	bool LinkVarToMono(ScriptVariable* variable, MonoClassField* mfield, MonoType* mtype);
	void SetVarValue(ScriptVariable* variable, void* new_val);
	// ------------------------------------------------------------------

	/*Input*/
	MonoObject* GetMousePosition();

	/* CulverinBehaviour */
	MonoObject* GetMonoObjectLink(std::string name);
	mono_bool	GetEnabled(MonoObject* object, MonoObject* gameobect);
	void		SetEnabled(MonoObject* object, mono_bool active, MonoObject* gameobject);

	/*Game Object*/
	bool		IsStatic(MonoObject* object);
	mono_bool	IsActive(MonoObject* object);
	void		SetActive(MonoObject* object, mono_bool active);
	void		SetName(MonoObject* object, MonoString* name);
	MonoString* GetName(MonoObject* object);
	void		SetTag(MonoObject* object, MonoString* tag);
	MonoString* GetTag(MonoObject* object);
	bool		CompareTag(MonoObject* object, MonoString* tag);
	MonoObject* FindGameObjectWithTag(MonoObject* object, MonoString* tag);

	int			ChildCount(MonoObject* object);
	MonoObject* GetChildByIndex(MonoObject* object, int index);
	MonoObject* GetChildByName(MonoObject* object, MonoString* name);

	MonoObject* GetOwnGameObject();
	void		SetCurrentGameObject(GameObject* current);
	void		SetOwnGameObject(GameObject* gameobject);
	void		CreateGameObject(MonoObject* object);
	bool		DestroyGameObject(MonoObject* object);
	MonoObject*	Instantiate(MonoObject* object, MonoString* prefab);
	MonoObject*	Instantiate_respawn(MonoObject* object, MonoString* prefab, float time);
	void		Destroy(MonoObject* object, float time);

	/* Transform */
	MonoObject* GetForwardVector(MonoObject* object);
	MonoObject* GetBackwardVector(MonoObject* object);
	MonoObject* GetUpVector(MonoObject* object);
	MonoObject* GetDownVector(MonoObject* object);
	MonoObject* GetRightVector(MonoObject* object);
	MonoObject* GetLeftVector(MonoObject* object);
	void		SetForwardVector(MonoObject* object, MonoObject* vector3);
	void		SetBackwardVector(MonoObject* object, MonoObject* vector3);
	void		SetUpVector(MonoObject* object, MonoObject* vector3);
	void		SetDownVector(MonoObject* object, MonoObject* vector3);
	void		SetRightVector(MonoObject* object, MonoObject* vector3);
	void		SetLeftVector(MonoObject* object, MonoObject* vector3);
	MonoObject* GetPosition(MonoObject* object);
	MonoObject* GetGlobalPosition(MonoObject* object);
	void		SetPosition(MonoObject* object, MonoObject* vector3);
	void		SetGlobalPosition(MonoObject* object, MonoObject* vector3);
	void		Translate(MonoObject* object, MonoObject* vector3);
	MonoObject* GetRotation(MonoObject* object);
	MonoObject* GetGlobalRotation(MonoObject* object);
	void		SetRotation(MonoObject* object, MonoObject* vector3);
	void		SetGlobalRotation(MonoObject* object, MonoObject* vector3);
	void		IncrementRotation(MonoObject* object, MonoObject* vector3);
	void		RotateAroundAxis(MonoObject* object, MonoObject* vector3, float value);
	void		SetScale(MonoObject* object, MonoObject* vector3);
	MonoObject* GetScale(MonoObject* object);
	void		LookAt(MonoObject* object, MonoObject* vector3);
	void		LookAtTrans(MonoObject * object, MonoObject * transform);
	MonoObject* GetLocalTransform(MonoObject * object);
	MonoObject* GetGlobalTransform(MonoObject * object);

	/*Particles*/

	void		ActivateEmission(MonoObject* obj, bool a);
	bool		IsEmitterActive(MonoObject* obj);

	/*Audio*/
	void		PlayAudioEvent(MonoObject* object, MonoString* event_name);
	void		StopAudioEvent(MonoObject* object, MonoString* event_name);
	void		SetAuxiliarySends(MonoObject* object, MonoString* bus, float value);

	/*Animation*/
	void		PlayAnimation(MonoObject* object, MonoString* name, mono_bool blending);
	void		SetTransition(MonoObject* object, MonoString* name, mono_bool condition);
	mono_bool	IsAnimationStopped(MonoObject* object, MonoString* name);
	mono_bool	IsAnimationRunning(MonoObject* object, MonoString* name);
	mono_bool	IsAnimOverXTime(MonoObject* object, float number_between_0_1);
	void		SetClipsSpeed(MonoObject* object, float speed_value);
	float		GetClipDuration(MonoObject* object, MonoString* name);
	void		SetClipDuration(MonoObject* object, MonoString* name, float duration);
	void		SetActiveBlendingClip(MonoObject* object, MonoString* name);
	void		SetActiveBlendingClipWeight(MonoObject* object, float weight);

	/*UI-Interactive*/
	void		Activate(MonoObject * object, int uid);
	void		Deactivate(MonoObject * object, int uid);
	void		Clicked(MonoObject* object);

	/*UI-Graphics*/
	void		SetRaycastTarget(MonoObject * object, mono_bool flag);
	void		FillAmount(MonoObject * object, float value);
	void		ActivateRender(MonoObject * object);
	void		DeactivateRender(MonoObject * object);

	/*Collider*/
	MonoObject* GetCollidedObject(MonoObject* object);
	void		MoveStaticColliderTo(MonoObject* object, MonoObject* positio);
	void		CallOnContact(MonoObject* object);
	void		CallOnTriggerEnter(MonoObject* object);

	/*RigidBody*/
	MonoObject* GetColliderPosition(MonoObject* object);
	MonoObject* GetColliderQuaternion(MonoObject* object);
	void		RemoveJoint(MonoObject* object);
	void		MoveKinematic(MonoObject* object, MonoObject* position, MonoObject* rotation);
	void		ApplyForce(MonoObject* object, MonoObject* force);
	void		ApplyImpulse(MonoObject* object, MonoObject* impulse);
	void		ApplyTorqueForce(MonoObject* object, MonoObject* force);
	void		ApplyTorqueImpulse(MonoObject* object, MonoObject* impulse);
	void		LockTransform(MonoObject* object);
	void		UnLockTransform(MonoObject* object);
	void		ResetForce(MonoObject* object);
	void		WakeUp(MonoObject* object);
	void		SetAtMaxJointPose(MonoObject* object);

	/*Joint*/
	void		DestroyJoint(MonoObject* object);

	/*Physics*/
	MonoObject*	RayCast(MonoObject* origin, MonoObject* direction, float distance);

	/*Material*/
	void		SetAlbedo(MonoObject* object, MonoString* string);
	void		SetNormals(MonoObject* object, MonoString* string);
	void		SetAmbientOcclusion(MonoObject* object, MonoString* string);

	/*Components*/
	MonoObject* GetComponent(MonoObject* object, MonoReflectionType* type);
	MonoObject* GetParentGameObject(MonoObject* object);

	/*Childs*/
	MonoObject* Find(MonoObject* object, MonoString* name);

	// Map --------------------
	MonoString* GetMapString(MonoObject* object);

	// LOAD - SAVE METHODS ------------------
	void Save(JSON_Object* object, std::string name) const;
	void Load(const JSON_Object* object, std::string name);
	void LoadValuesGO(GameObject* sync_parent);

	GameObject* GetGameObject() const;

public:
	//Variables/Info containers (public to have access through other modules)
	std::vector<ScriptVariable*> variables;

private:
	bool test = false;

	//Auxiliar map to fill variables vector with info
	std::map<MonoClassField*, MonoType*> field_type;
	std::string nameCSharp;
	std::string name_space;

	MonoDomain* CSdomain = nullptr;
	MonoAssembly* CSassembly = nullptr;
	MonoImage* CSimage = nullptr;
	MonoClass* CSClass = nullptr;
	MonoObject* CSObject = nullptr;
	//MonoObject* CSSelfObject = nullptr;
	GameObject* own_game_object = nullptr;

	// Main Functions
	MainMonoMethod Start;
	MainMonoMethod Update;
	MainMonoMethod FixedUpdate;
	MainMonoMethod OnGUI;
	MainMonoMethod OnEnable;
	MainMonoMethod OnDisable;
	MainMonoMethod OnTriggerEnter;
	MainMonoMethod OnTriggerLost;
	MainMonoMethod OnContact;
	MainMonoMethod OnClick;

	GameObject* current_game_object = nullptr;
	std::vector<uint> re_load_values;

	/* Components */
	std::vector<Component*> temp;

};

#endif


