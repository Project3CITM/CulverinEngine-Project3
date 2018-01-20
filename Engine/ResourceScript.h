#ifndef _RESOURCESCRIPT_
#define _RESOURCESCRIPT_

#include "Resource_.h"
#include "Globals.h"

#include <string>

class CSharpScript;
class Script_editor;
class GameObject;

class ResourceScript : public Resource
{
public:
	ResourceScript(uint uid);
	virtual ~ResourceScript();

	void InitInfo(std::string path_dll_, std::string path_assets_);
	void SetCSharp(CSharpScript* csharp_);

	// Name Class = name Script
	void SetCurrentGameObject(GameObject * current_);
	void CreateNewScript(std::string nameScript);
	void SetScriptEditor(std::string nameScript);
	void SetOwnGameObject(GameObject* owenerofScript);
	bool ReImportScript(std::string path_dll_);

	bool Start();
	bool Update(float dt);

	std::string GetPathAssets() const;
	std::string GetPathdll() const;

	//To access script thorug extern modules
	CSharpScript* GetCSharpScript();

	void ShowEditor(bool & active);
	void DeleteToMemory();
	bool LoadToMemory();
	Resource::State IsCompiled();

	// LOAD - SAVE METHODS ------------------
	void Save(JSON_Object* object, std::string name) const;
	void Load(const JSON_Object* object, std::string name);
	void LoadValuesGameObject();

private:
	std::string path_dll;
	CSharpScript* csharp = nullptr;
	Script_editor* editor = nullptr;
};


#endif