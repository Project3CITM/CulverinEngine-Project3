#include "Scene.h"
#include "ResourceScript.h"
#include "CSharpScript.h"
#include "Application.h"
#include "ModuleFS.h"
#include "GameObject.h"
#include "Script_editor.h"
#include "CompScript.h"

ResourceScript::ResourceScript(uint uid) : Resource(uid, Resource::Type::SCRIPT, Resource::State::UNLOADED)
{
	num_game_objects_use_me = 0;
	editor = new Script_editor(this);
	LOG("Resource Script Created!");
}

ResourceScript::~ResourceScript()
{
	path_dll.clear();
	path_assets.clear();
	RELEASE(editor);
}

void ResourceScript::InitInfo(std::string path_dll_, std::string path_assets_)
{
	name = App->GetCharfromConstChar(App->fs->GetOnlyName(path_assets_).c_str());
	path_dll = path_dll_;
	path_assets = path_assets_;
}

void ResourceScript::CreateNewScript(std::string nameScript)
{
	editor->Start(nameScript, true);
	editor->SaveScript();
}

void ResourceScript::SetScriptEditor(std::string nameScript)
{
	editor->Start(nameScript, false);
}

//void ResourceScript::SetCurrentGameObject(GameObject* current)
//{
//	if (csharp != nullptr)
//	{
//		csharp->SetCurrentGameObject(current);
//	}
//}
//
//void ResourceScript::SetOwnGameObject(GameObject* owenerofScript)
//{
//	if (csharp != nullptr)
//	{
//		csharp->SetOwnGameObject(owenerofScript);
//	}
//}

bool ResourceScript::ReImportScript(std::string path_dll_)
{
	bool ret = true;

	to_fill_vec.clear();

	IterateSceneToCheckScripts(App->scene->root, &to_fill_vec);

	if (to_fill_vec.size() > 0)
	{
		for (uint i = 0; i < to_fill_vec.size(); i++)
		{
			ret = to_fill_vec[i]->csharp->ReImport(path_dll_);
		}
	}

	return ret;
}


//void ResourceScript::SetCSharp(CSharpScript* csharp_)
//{
//	csharp = csharp_;
//}

//bool ResourceScript::Start()
//{
//	if (csharp != nullptr)
//	{
//		csharp->DoMainFunction(FunctionBase::CS_Start);
//		return true;
//	}
//	return false;
//}
//
//bool ResourceScript::Update(float dt)
//{
//	if (csharp != nullptr)
//	{
//		csharp->DoMainFunction(FunctionBase::CS_Update);
//		return true;
//	}
//	return false;
//}

std::string ResourceScript::GetPathAssets() const
{
	return path_assets;
}

std::string ResourceScript::GetPathdll() const
{
	return path_dll;
}

//CSharpScript* ResourceScript::GetCSharpScript()
//{
//	if (csharp != nullptr)
//	{
//		return csharp;
//	}
//}

void ResourceScript::ShowEditor(bool& active)
{
	editor->Show(active);
}

void ResourceScript::DeleteToMemory()
{
	//state = Resource::State::UNLOADED;
	//glDeleteTextures(1, &texture.id);
	LOG("UnLoaded Resource Script");
}

bool ResourceScript::LoadToMemory()
{
	state = Resource::State::LOADED;
	LOG("Loaded Resource Script");
	return true;
}

Resource::State ResourceScript::IsCompiled()
{
	return state;
}

void ResourceScript::IterateSceneToCheckScripts(GameObject * obj, std::vector<CompScript*>* to_fill_vec)
{
	for (uint i = 0; i < obj->GetNumChilds(); i++)
	{
		//Check if the GameObject has the script to reimport
		CompScript* temp = (CompScript*)obj->GetChildbyIndex(i)->GetComponentByName(name);
		if (temp != nullptr)
		{
			to_fill_vec->push_back(temp);
		}

		//Iterate childs
		IterateSceneToCheckScripts(obj->GetChildbyIndex(i), to_fill_vec);		
	}
}

//void ResourceScript::Save(JSON_Object* object, std::string name) const
//{
//	//Save Values
//	if (csharp != nullptr)
//	{
//		csharp->Save(object, name);
//	}
//}
//
//void ResourceScript::Load(const JSON_Object* object, std::string name)
//{
//	//Load Values
//	if (csharp != nullptr)
//	{
//		csharp->Load(object, name);
//	}
//}
//
//void ResourceScript::LoadValuesGameObject()
//{
//	if (csharp != nullptr)
//	{
//		csharp->LoadValues();
//	}
//}