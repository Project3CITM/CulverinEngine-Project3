#ifndef _RESOURCESCRIPT_
#define _RESOURCESCRIPT_

#include "Resource_.h"
#include "Globals.h"

#include <string>

class Script_editor;
class GameObject;
class CompScript;

class ResourceScript : public Resource
{
public:
	ResourceScript(uint uid);
	virtual ~ResourceScript();

	void InitInfo(std::string path_dll_, std::string path_assets_);

	// Name Class = name Script
	void CreateNewScript(std::string nameScript);
	void SetScriptEditor(std::string nameScript);
	bool ReImportScript(std::string path_dll_);

	std::string GetPathAssets() const;
	std::string GetPathdll() const;

	//To access script thorug extern modules
	//CSharpScript* GetCSharpScript();

	void ShowEditor(bool & active);
	void DeleteToMemory();
	bool LoadToMemory();
	Resource::State IsCompiled();

	void IterateSceneToCheckScripts(GameObject* obj, std::vector<CompScript*>* to_fill_vec);

	//// LOAD - SAVE METHODS ------------------
	//void Save(JSON_Object* object, std::string name) const;
	//void Load(const JSON_Object* object, std::string name);
	//void LoadValuesGameObject();

private:
	std::string path_dll;
	Script_editor* editor = nullptr;
	std::vector<CompScript*> to_fill_vec;
};


#endif