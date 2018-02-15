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
	void ClearVariables();

	bool StartScript();
	bool UpdateScript(float dt);

	// Play Engine -------
	bool CheckScript();
	bool CheckAllVariables();
	void RemoveReferences(GameObject* go);

	void SetCurrentGameObject(GameObject* current_);
	void SetOwnGameObject(GameObject* owenerofScript);
	void SetCSharp(CSharpScript* csharp_);

	// EDITOR METHODS ----------
	void ShowOptions();
	void ShowInspectorInfo();
	void ShowVariablesInfo();
	void ShowVarType(ScriptVariable* var);
	void ShowVarValue(ScriptVariable* var, int i);
	void CopyValues(const CompScript * component);
	// -------------------------

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------
	// LOAD - SAVE METHODS ------------------
	void SaveScript(JSON_Object* object, std::string name) const;
	void LoadScript(const JSON_Object* object, std::string name);
	void LoadValuesGameObjectScript();

public:

	std::string name_script;
	//Script_editor* editor = nullptr;
	CSharpScript* csharp = nullptr;
	ResourceScript* resource_script = nullptr;

private:

	bool select_script = false;
	uint uuid_resource_reimported = 0;
};


#endif
