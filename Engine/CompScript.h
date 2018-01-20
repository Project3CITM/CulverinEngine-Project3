#ifndef _COMPSCRIPT_
#define _COMPSCRIPT_

#include "Component.h"
#include "Script_editor.h"

class ResourceScript;
class ScriptVariable;

class CompScript : public Component
{
public:
	CompScript(Comp_Type t, GameObject* parent);
	CompScript(const CompScript& copy, GameObject* parent);
	~CompScript();

	void Init();
	void preUpdate(float dt);

	void Start();
	void Update(float dt);
	void ClearVariables();
	// Play Engine -------
	bool CheckScript();
	bool CheckAllVariables();
	void RemoveReferences(GameObject* go);

	// EDITOR METHODS ----------
	void ShowOptions();
	void ShowInspectorInfo();
	void ShowVariablesInfo();
	void ShowVarType(ScriptVariable* var);
	void ShowVarValue(ScriptVariable* var, int i);
	// -------------------------

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------

public:
	std::string nameScript;
	//Script_editor* editor = nullptr;
	ResourceScript* resourcescript = nullptr;

private:
	bool selectScript = false;
	uint uuidResourceReimported = 0;
};


#endif
