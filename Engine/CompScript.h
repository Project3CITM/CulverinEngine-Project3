#ifndef _COMPSCRIPT_
#define _COMPSCRIPT_

#include "Component.h"
#include "Script_editor.h"

class ResourceScript;
class ScriptVariable;
class CSharpScript;

class CompScript : public Component
{
public:
	CompScript(Comp_Type t, GameObject* parent);
	CompScript(const CompScript& copy, GameObject* parent);
	~CompScript();

	void Init();
	void PreUpdate(float dt);

	void Start();
	void Update(float dt);
	void postUpdate();
	void Clear();

	bool StartScript();
	bool UpdateScript(float dt);

	// Play Engine -------
	bool CheckScript();
	bool CheckAllVariables();
	void RemoveReferences(GameObject* go);

	const char* GetScriptName() const;

	void SetCurrentGameObject(GameObject* current_);
	void SetOwnGameObject(GameObject* owenerofScript);
	void SetCSharp(CSharpScript* csharp_);

	// EDITOR METHODS ----------
	void ShowOptions();
	void ShowInspectorInfo();
	void ShowVariablesInfo();
	void ShowFSMInspectorInfo(); // Only a part of the inspector is shown in the FSM
	void ShowVarType(ScriptVariable* var);
	void ShowVarValue(ScriptVariable* var, int i);
	void CopyValues(const CompScript * component);
	bool SelectScript(bool& selecting);
	// -------------------------

	//Special --------------
	void AddScriptbyName(const char* name_script);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	void SyncComponent(GameObject* sync_parent);
	void LoadValuesGameObjectScript();
	// -------------------------------------

	// LOAD - SAVE METHODS (SCRIPT)------------------
	void SaveScript(JSON_Object* object, std::string name) const;
	void LoadScript(const JSON_Object* object, std::string name);

public:

	std::string name_script;
	//Script_editor* editor = nullptr;
	CSharpScript* csharp = nullptr;
	ResourceScript* resource_script = nullptr;
	bool p_active = false; // Use only with enable and disable 
	bool do_start = false; // Use only with Instantiate 

private:

	bool select_script = false;
	uint uuid_resource_reimported = 0;
	bool active_script = false;
};


#endif
