#include "ImportScript.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ModuleImporter.h"
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "ResourceScript.h"
#include "ModuleMap.h"
#include "Scene.h"
#include "ModuleResourceManager.h"
#include "JSONSerialization.h"
#include "CSharpScript.h"
#include "Timer.h"
#include "ModuleAudio.h"
#include "PlayerActions.h"
#include "CompScript.h"
#include <direct.h>
#include "ModuleRenderGui.h"
#include "ModuleEventSystemV2.h"
#pragma comment(lib, "mono-2.0-sgen.lib")

CSharpScript* ImportScript::current = nullptr;

ImportScript::ImportScript()
{
}


ImportScript::~ImportScript()
{
	ShutdownMono();
}

void ImportScript::Clear()
{
	nameScripts.clear();
	current = nullptr;

	mono_map.clear();
	mono_comp.clear();
	mono_pos.clear();
	mono_script.clear();
	mono_quat.clear();
	mono_material.clear();

}

bool ImportScript::InitScriptingSystem()
{
	//Set the working directory of the console to Mono/bin to execute "mcs" command (to compile scripts)
	//system("cd Mono/monobin");

	char my_path[FILENAME_MAX];
	// Fill my_path char array with the path of the .dll
	_getcwd(my_path, FILENAME_MAX);

	// Set where Mono Directory is placed in mono_path
	mono_path = my_path;
	mono_path += "/Mono";

	// Use the standard configuration ----
	mono_config_parse(NULL);

	// Get the correct dirs --------------
	std::string lib = mono_path;
	lib += "/lib";
	std::string etc = mono_path;
	etc += "/etc";

	// Setup the mono directories to start working with
	mono_set_dirs(lib.c_str(), etc.c_str());
	domain = mono_jit_init("CulverinEngine");

	//Register internal calls to set Scripting functions to work with .cs files
	LinkFunctions();

	// childDomain
	childDomain = Load_domain();

	MonoAssembly* culverin_assembly = mono_domain_assembly_open(childDomain, "./ScriptManager/AssemblyReference/CulverinEditor.dll");
	if (culverin_assembly)
	{
		culverin_mono_image = mono_assembly_get_image(culverin_assembly);
		std::string monoCmdOptions = "--soft-breakpoints";
		char *options = new char[monoCmdOptions.size() + 1];
		strcpy(options, monoCmdOptions.c_str());
		mono_jit_parse_options(1, &options);
		return true;
	}

	

	return false;
}

void ImportScript::ShutdownMono()
{
	//Unload_domain();
	//mono_jit_cleanup(domain);
}

bool ImportScript::Import(const char* file, uint uuid, bool isAutoImport)
{
	uint uuid_script = 0;
	if (uuid == 0) // if direfent create a new resource with the resource deleted
	{
		uuid_script = App->random->Int();
	}
	else
	{
		uuid_script = uuid;
	}
	if (IsNameUnique(App->fs->GetOnlyName(file)) == false)
	{
		LOG("[error] Script: %s, The Name of Script must be unique, there is already a script with that name.", App->fs->GetOnlyName(file).c_str());
		return false;
	}
	else
	{
		// First add name into nameScripts in ImportScript
		nameScripts.push_back(App->fs->GetOnlyName(file));

		// Now create Resource and Import C#
		ResourceScript* res_script = (ResourceScript*)App->resource_manager->CreateNewResource(Resource::Type::SCRIPT, uuid_script);
		if (res_script != nullptr)
		{
			std::string fileassets = App->fs->CopyFileToAssetsS(file);

			// Create TextEditor from the script.
			res_script->SetScriptEditor(App->fs->GetOnlyName(fileassets));

			// First Compile The CSharp
			std::string path_dll;
			if (CompileScript(fileassets.c_str(), path_dll, std::to_string(uuid_script).c_str()) != 0)
			{
				LOG("[error] Script: %s, Not Compiled", App->fs->GetOnlyName(fileassets).c_str());
				res_script->InitInfo(path_dll, fileassets);
				res_script->SetState(Resource::State::FAILED);
				//CSharpScript* script = new CSharpScript();
				//res_script->SetCSharp(script);
				return false;
			}
			else
			{
				LOG("Script: %s, Compiled without errors", App->fs->GetOnlyName(fileassets).c_str());
				res_script->InitInfo(path_dll, fileassets);
				res_script->SetState(Resource::State::LOADED);
				//now 
				//CSharpScript* newCSharp = LoadScript_CSharp(path_dll);
				//res_script->SetCSharp(newCSharp);
			}

			if (App->mode_game == false)
			{
				// Then Create Meta
				std::string Newdirectory = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
				Newdirectory += "\\" + App->fs->FixName_directory(file);
				if (isAutoImport)
				{
					App->json_seria->SaveScript(res_script, App->fs->GetFullPath(file).c_str(), Newdirectory.c_str());
				}
				else
				{
					App->json_seria->SaveScript(res_script, ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
				}
			}
		}
	}
	return true;
}

bool ImportScript::LoadResource(const char* file, ResourceScript* resourceScript)
{
	if (resourceScript != nullptr)
	{
		std::string path_dll;

		// First Compile The CSharp
		if (CompileScript(file, path_dll, std::to_string(resourceScript->GetUUID()).c_str()) != 0)
		{
			LOG("[error] Script: %s, Not Compiled", App->fs->GetOnlyName(file).c_str());
			resourceScript->InitInfo(path_dll, file);
			resourceScript->SetState(Resource::State::FAILED);
			return false;
		}
		else
		{
			LOG("Script: %s, Compiled without errors", App->fs->GetOnlyName(file).c_str());
			resourceScript->InitInfo(path_dll, file);
			resourceScript->SetState(Resource::State::LOADED);
			resourceScript->SetScriptEditor(App->fs->GetOnlyName(App->fs->GetOnlyName(file).c_str()));
			//now 
			//CSharpScript* newCSharp = LoadScript_CSharp(path_dll);
			//resourceScript->SetCSharp(newCSharp);
		}

		if (App->mode_game == false)
		{
			// Then Create Meta
			std::string Newdirectory = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
			Newdirectory += "\\" + App->fs->FixName_directory(file);
			App->json_seria->SaveScript(resourceScript, ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
		}
	}
	return true;
}

bool ImportScript::ReImportScript(std::string fileAssets, std::string uid_script, ResourceScript* resourceScript, bool auto_reimport)
{
	// UnloadDomain
	Unload_domain();

	if (resourceScript == nullptr)
	{
		resourceScript = (ResourceScript*)App->resource_manager->CreateNewResource(Resource::Type::SCRIPT, atoi(uid_script.c_str()));
	}

	// ReCompile The CSharp
	std::string path_dll;
	if (CompileScript(fileAssets.c_str(), path_dll, uid_script.c_str()) != 0)
	{
		LOG("[error] Script: %s, Not Compiled", App->fs->GetOnlyName(fileAssets).c_str());
		return false;
	}
	else
	{
		LOG("Script: %s, Compiled without errors", App->fs->GetOnlyName(fileAssets).c_str());
		resourceScript->InitInfo(path_dll, fileAssets);
	}
	// childDomain
	childDomain = Load_domain();
	MonoAssembly* culverin_assembly = mono_domain_assembly_open(childDomain, "./ScriptManager/AssemblyReference/CulverinEditor.dll");
	if (culverin_assembly)
	{
		culverin_mono_image = mono_assembly_get_image(culverin_assembly);
	}
	if (auto_reimport == false)
	{
		if (App->resource_manager->ReImportAllScripts() == false)
		{
			LOG("[error] Error With ReImport Script");
		}
		//CSharpScript* newCSharp = LoadScript_CSharp(path_dll);
		//resourceScript->SetCSharp(newCSharp);
		if (App->build_mode == false)
		{
			// Then Create Meta
			std::string Newdirectory = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
			Newdirectory += "/" + App->fs->FixName_directory(fileAssets);
			App->json_seria->SaveScript(resourceScript, ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
		}
	}

	return true;
}

MonoDomain* ImportScript::Load_domain()
{
	MonoDomain* newDomain = mono_domain_create_appdomain("CulverinEngine_Child", NULL);
	if (!newDomain) {
		LOG("[error] Error creating domain\n");
		return nullptr;
	}

	//mono_thread_push_appdomain_ref(newDomain);

	if (!mono_domain_set(newDomain, false)) {
		LOG("[error] Error setting domain\n");
		return nullptr;
	}

	return mono_domain_get();
}

void ImportScript::Unload_domain()
{
	MonoDomain* old_domain = mono_domain_get();
	if (old_domain && old_domain != mono_get_root_domain()) {
		if (!mono_domain_set(mono_get_root_domain(), false))
			LOG("[error] Error setting domain\n");

		//mono_thread_pop_appdomain_ref();
		mono_domain_unload(old_domain);
	}

	//unloading a domain is also a nice point in time to have the GC run.
	mono_gc_collect(mono_gc_max_generation());
}

bool ImportScript::CreateNewScript(bool& active)
{
	//ImGui::PushStyleVar() // Center
	ImGui::Begin("Create New Script", &active, ImGuiWindowFlags_NoCollapse);
	static Timer timeshowResult;
	static int result = 0;
	ImGui::Text("Put Name Name Class: ");
	char namedit[50];
	strcpy_s(namedit, 50, nameNewScript.c_str());
	ImGui::Bullet();
	if (ImGui::InputText("##nameNewScript", namedit, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (result == 0)
		{
			nameNewScript = App->fs->ConverttoChar(std::string(namedit).c_str());
			if (nameNewScript.compare("") != 0)
			{
				if (IsNameUnique(nameNewScript) == false)
				{
					result = 1;
					timeshowResult.Start();
					LOG("[error] This name is already used! Change name!");
				}
				else
				{
					nameScripts.push_back(nameNewScript);
					result = 2;
					timeshowResult.Start();
					uint uuid_script = App->random->Int();
					ResourceScript* res_script = (ResourceScript*)App->resource_manager->CreateNewResource(Resource::Type::SCRIPT, uuid_script);
					res_script->CreateNewScript(nameNewScript);
					// First Compile The CSharp
					std::string fileassets = nameNewScript;
					fileassets = "Assets/" + fileassets + ".cs";
					std::string path_dll;
					if (CompileScript(App->fs->GetFullPath(fileassets).c_str(), path_dll, std::to_string(uuid_script).c_str()) != 0)
					{
						LOG("[error] Script: %s, Not Compiled", App->fs->GetOnlyName(fileassets).c_str());
						res_script->InitInfo(path_dll, fileassets);
						res_script->SetState(Resource::State::FAILED);
						ImGui::End();
						return false;
					}
					else
					{
						LOG("Script: %s, Compiled without errors", App->fs->GetOnlyName(fileassets).c_str());
						res_script->InitInfo(path_dll, fileassets);
						res_script->SetState(Resource::State::LOADED);
						//now 
						//CSharpScript* newCSharp = LoadScript_CSharp(path_dll);
						//res_script->SetCSharp(newCSharp);
					}

					App->fs->UpdateFilesAssets();

					// Then Create Meta
					std::string Newdirectory = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
					Newdirectory += "/" + App->fs->GetOnlyName(fileassets) + ".cs";
					App->json_seria->SaveScript(res_script, ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
				}
			}
		}
	}
	ImGui::Text("Press 'Enter' to create a new Script");
	if (timeshowResult.ReadSec() < 2)
	{
		if (result == 1)
		{
			ImGui::TextColored(ImVec4(0.933, 0, 0, 1), "[error] This name is already used! Change name!");
		}
		else if (result == 2)
		{
			ImGui::TextColored(ImVec4(0.109, 0.933, 0, 1), "Script: %s, Compiled without errors", nameNewScript.c_str());
		}
	}
	else if (result != 0)
	{
		nameNewScript = "";
		result = 0;
	}

	ImGui::End();
	return true;
}

MonoDomain* ImportScript::GetMainDomain() const
{
	return domain;
}

MonoDomain* ImportScript::GetDomain() const
{
	return childDomain;
}

MonoImage* ImportScript::GetCulverinImage() const
{
	return culverin_mono_image;
}

std::string ImportScript::GetMonoPath() const
{
	return mono_path;
}

void ImportScript::SetCurrentScript(CSharpScript* current_)
{
	current = current_;
}

void ImportScript::SetMonoMap(GameObject* gameobject, bool is_root)
{
	for (int i = 0; i < gameobject->GetNumChilds(); i++)
	{
		SetMonoMap(gameobject->GetChildbyIndex(i), false);
	}

	if (is_root == false)
	{
		if (GetMonoObject(gameobject) != nullptr)
		{
			return;
		}
		if (gameobject->GetNumComponents() == 1)
		{
			if (strcmp(gameobject->GetTag(), "undefined") == 0)
			{
				return;
			}
		}
		if (strcmp(gameobject->GetTag(), "NoC#") == 0)
		{
			return;
		}
		MonoClass* c = mono_class_from_name(GetCulverinImage(), "CulverinEditor", "GameObject");
		if (c)
		{
			MonoObject* new_object = mono_object_new(GetMainDomain(), c);
			if (new_object)
			{
				mono_runtime_object_init(new_object);
				mono_map.insert(std::pair<uint, GameObject*>(mono_gchandle_new(new_object, false), gameobject));
			}
		}
	}
}

void ImportScript::ClearLinkVariables()
{
	map_link_variables.clear();
}

void ImportScript::ClearMonoMap()
{
	mono_map.clear();
	mono_comp.clear();
	mono_pos.clear();
	mono_script.clear();
	mono_quat.clear();
	mono_material.clear();
}

void ImportScript::UpdateMonoMap(GameObject* modificate, bool do_delete)
{
	if (do_delete)
	{
		std::multimap<uint, GameObject*>::iterator it = mono_map.begin();
		while (it != mono_map.end())
		{
			if (it->second == modificate)
			{
				it->second = nullptr;
				mono_map.erase(it->first);
				return;
			}
			it++;
		}
	}
	else
	{
		// Add in map.
		MonoClass* c = mono_class_from_name(GetCulverinImage(), "CulverinEditor", "GameObject");
		if (c)
		{
			MonoObject* new_object = mono_object_new(GetMainDomain(), c);
			if (new_object)
			{
				mono_runtime_object_init(new_object);
				mono_map.insert(std::pair<uint, GameObject*>(mono_gchandle_new(new_object, false), modificate));
			}
		}
	}
}

void ImportScript::UpdateMonoMap(GameObject* modificate, MonoObject* object)
{
	//mono_map.insert(std::pair<uint, GameObject*>(object, modificate));
}

MonoObject* ImportScript::GetMonoObject(GameObject* gameobject)
{
	if (gameobject != nullptr/* && gameobject->IsDeleteFixed()*/)
	{
		std::multimap<uint, GameObject*>::iterator it = mono_map.begin();
		while (it != mono_map.end())
		{
			if (it->second->GetUUID() == gameobject->GetUUID())
			{
				return mono_gchandle_get_target(it->first);
			}
			it++;
		}
	}
	return nullptr;
}

GameObject* ImportScript::GetGameObject(MonoObject* monoobject)
{
	if (monoobject != nullptr)
	{
		std::multimap<uint, GameObject*>::iterator it = mono_map.begin();
		while (it != mono_map.end())
		{
			if (mono_gchandle_get_target(it->first) == monoobject)
			{
				return it->second;
			}
			it++;
		}
	}
	LOG("RETURN NULLPTR");
	return nullptr;
}

void ImportScript::UpdateMonoComp(Component* modificate, uint csreference)
{
	mono_comp.insert(std::pair<uint, Component*>(csreference, modificate));
}

MonoObject* ImportScript::GetMonoObject(Component* component)
{
	if (component != nullptr/* && gameobject->IsDeleteFixed()*/)
	{
		std::multimap<uint, Component*>::iterator it = mono_comp.begin();
		while (it != mono_comp.end())
		{
			if (it->second == component)
			{
				return mono_gchandle_get_target(it->first);
			}
			it++;
		}		
	}
	return nullptr;
}

Component* ImportScript::GetComponentMono(MonoObject* monoobject)
{
	if (monoobject != nullptr)
	{
		std::multimap<uint, Component*>::iterator it = mono_comp.begin();
		while (it != mono_comp.end())
		{
			if (mono_gchandle_get_target(it->first) == monoobject)
			{
				return it->second;
			}
			it++;
		}
	}
	return nullptr;
}

void ImportScript::UpdateMonoPos(float3* pos, uint csreference)
{
	mono_pos.insert(std::pair<uint, float3*>(csreference, pos));
}

MonoObject* ImportScript::GetMonoObject(float3* pos)
{
	std::multimap<uint, float3*>::iterator it = mono_pos.begin();
	while (it != mono_pos.end())
	{
		if (it->second == pos)
		{
			return mono_gchandle_get_target(it->first);
		}
		it++;
	}
	return nullptr;
}

float3& ImportScript::GetPosMono(MonoObject* monoobject)
{
	if (monoobject != nullptr)
	{
		std::multimap<uint, float3*>::iterator it = mono_pos.begin();
		while (it != mono_pos.end())
		{
			if (mono_gchandle_get_target(it->first) == monoobject)
			{
				return *it->second;
			}
			it++;
		}
	}
	return float3(0,0,0);
}

void ImportScript::UpdateMonoMaterial(Material* modificate, uint csreference)
{
	mono_material.insert(std::pair<uint, Material*>(csreference, modificate));
}

MonoObject* ImportScript::GetMonoObject(Material* material)
{
	std::multimap<uint, Material*>::iterator it = mono_material.begin();
	while (it != mono_material.end())
	{
		if (it->second == material)
		{
			return mono_gchandle_get_target(it->first);
		}
		it++;
	}
	return nullptr;
}

Material* ImportScript::GetMaterialMono(MonoObject* monoobject)
{
	if (monoobject != nullptr)
	{
		std::multimap<uint, Material*>::iterator it = mono_material.begin();
		while (it != mono_material.end())
		{
			if (mono_gchandle_get_target(it->first) == monoobject)
			{
				return it->second;
			}
			it++;
		}
	}
	return nullptr;
}

void ImportScript::UpdateMonoScript(CSharpScript* script, uint csreference)
{
	mono_script.insert(std::pair<uint, CSharpScript*>(csreference, script));
}

MonoObject* ImportScript::GetMonoObject(CSharpScript* script)
{
	std::multimap<uint, CSharpScript*>::iterator it = mono_script.begin();
	while (it != mono_script.end())
	{
		if (it->second == script)
		{
			return mono_gchandle_get_target(it->first);
		}
		it++;
	}
	return nullptr;
}

CSharpScript* ImportScript::GetScriptMono(MonoObject* monoobject)
{
	if (monoobject != nullptr)
	{
		std::multimap<uint, CSharpScript*>::iterator it = mono_script.begin();
		while (it != mono_script.end())
		{
			if (mono_gchandle_get_target(it->first) == monoobject)
			{
				return it->second;
			}
			it++;
		}
	}
	return nullptr;
}

void ImportScript::RemoveGObjectVarFromScripting(GameObject * object)
{
	// Remove GameObject Variable form  multimap only during runtime
	if (object != nullptr)
	{
		std::multimap<std::string, GameObject*>::const_iterator it_link = map_link_variables.begin();
		for (; it_link != map_link_variables.end();)
		{
			if (it_link._Ptr->_Myval.second == object)
			{
				it_link = map_link_variables.erase(it_link);
			}
			else it_link++;
		}
	}
}

void ImportScript::RemoveGObjectFromMonoMap(GameObject* object)
{
	// Remove MonoObject form mono_map multimap only during runtime
	if (object != nullptr)
	{
		std::multimap<uint, GameObject*>::iterator it = mono_map.begin();
		while (it != mono_map.end())
		{
			if (it->second == object)
			{
				mono_gchandle_free(it->first);
				it = mono_map.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}


void ImportScript::RemoveComponentFromMonoList(Component* comp)
{
	// Remove Component from Components multimap only during runtime
	if (comp != nullptr)
	{
		std::multimap<uint, Component*>::const_iterator it_comp = mono_comp.begin();
		for (; it_comp != mono_comp.end();)
		{
			if (it_comp._Ptr->_Myval.second == comp)
			{
				mono_gchandle_free(it_comp->first);
				it_comp = mono_comp.erase(it_comp);
			}
			else it_comp++;
		}
	}
}

void ImportScript::RemoveTransformPosPointerFromMap(float3 * pospointer)
{
	// Remove Transform Position pointer from mono_pos multimap only during runtime
	if (pospointer != nullptr)
	{
		std::multimap<uint, float3*>::const_iterator it_pos = mono_pos.begin();
		for (; it_pos != mono_pos.end();)
		{
			if (it_pos._Ptr->_Myval.second == pospointer)
			{
				mono_gchandle_free(it_pos->first);
				it_pos = mono_pos.erase(it_pos);
			}
			else it_pos++;
		}
	}
}

void ImportScript::RemoveCSharpScriptFromMonoScript(CSharpScript * script)
{
	if (script != nullptr)
	{
		std::multimap<uint, CSharpScript*>::const_iterator it_pos = mono_script.begin();
		for (; it_pos != mono_script.end();)
		{
			if (it_pos._Ptr->_Myval.second == script)
			{
				mono_gchandle_free(it_pos->first);
				it_pos = mono_script.erase(it_pos);
				return;
			}
			else it_pos++;
		}
	}
}

void ImportScript::RemoveGObjectReferencesFromMonoScript(GameObject * object)
{
	// Set variables that reference to this object to null, only during runtime
	if (object != nullptr)
	{
		std::multimap<uint, CSharpScript*>::const_iterator it_script = mono_script.begin();
		for (; it_script != mono_script.end(); it_script++)
		{
			it_script._Ptr->_Myval.second->RemoveReferences(object);
		}
	}
}

bool ImportScript::IsNameUnique(std::string name) const
{
	if (name != "")
	{

		for (std::list<std::string>::const_iterator it = nameScripts.begin(); it != nameScripts.end(); it++)
		{
			if (it._Ptr->_Myval.compare(name) == 0)
			{
				return false;
			}
		}
	}
	return true;
}

int ImportScript::CompileScript(const char* file, std::string& libraryScript, const char* uid)
{
	// Get the path of the project ----
	std::string script_path = file;
	script_path= "\"" + script_path + "\"";

	// Save dll to Library Directory --------------------
	libraryScript = "Library/Scripts/";
	std::string nameFile = uid;
	nameFile += ".dll";
	libraryScript += nameFile;

	//Enclose the path with " " to make the console detect spaces
	std::string temp_path = "\"" + libraryScript + "\"";

	// Compile the script -----------------------------
	std::string command = "Mono\\monobin\\mcs -target:library -out:" + temp_path + " ";
	std::string CulverinEditorpath = "ScriptManager/AssemblyReference/CulverinEditor.dll";
	command += "-r:" + CulverinEditorpath + " ";
	command += "-lib:" + CulverinEditorpath + " ";
	command += script_path;

	return system(command.c_str());
}

CSharpScript* ImportScript::LoadScript_CSharp(std::string file, std::string name)
{
	if (file != "")
	{
		//MonoAssembly* assembly = mono_domain_assembly_open(GetDomain(), file.c_str());
		if (1)
		{
			// First Get the Image
			//MonoImage* image = mono_assembly_get_image(assembly);
			if (App->importer->iScript->GetCulverinImage())
			{
				CSharpScript* csharp = CreateCSharp(App->importer->iScript->GetCulverinImage(), name);
				if (csharp != nullptr)
				{
					csharp->SetAssembly(nullptr);
					csharp->LoadScript();
					return csharp;
				}
				else
				{
					LOG("[error] Can not create MonoClass, file: %s ", file.c_str());
				}
			}
			else
			{
				LOG("[error] Can not Get Image, file: %s ", file.c_str());
			}
		}
		else
		{
			LOG("[error] Can not Open Assembly, file: %s ", file.c_str());
		}
	}
	return nullptr;
}

CSharpScript* ImportScript::CreateCSharp(MonoImage* image, std::string nameClass)
{
	if (image)
	{
		std::string classname, name_space;
		MonoClass* entity = mono_class_from_name(image, "", nameClass.c_str());
		if (entity)
		{
			CSharpScript* csharp = new CSharpScript();
			csharp->SetDomain(GetDomain());
			csharp->SetImage(App->importer->iScript->GetCulverinImage());
			csharp->SetClass(entity);
			csharp->CreateCSObject();
			csharp->SetClassName(classname);
			csharp->SetNameSpace(name_space);

			void* iter = NULL;

			MonoMethod* method = NULL;
			while ((method = mono_class_get_methods(entity, &iter)))
			{
				uint t, flags;
				flags = mono_method_get_flags(method, &t);
				std::string name_method = mono_method_get_name(method);
				std::string fullname_method = mono_method_full_name(method, true);
				MonoMethodSignature* signature = mono_method_signature(method);
				uint num = mono_signature_get_param_count(signature);
				if (num > 1) // We can't use 2 or more paramaters (only 1 or 0)
				{
					continue;
				}
				if (!(flags & MONO_METHOD_ATTR_PUBLIC))// Use Public !!
				{
					continue;
				}
				if (strcmp(name_method.c_str(), ".ctor") == 0) // Dont enter with constructors...
				{
					continue;
				}


				if (num == 0)
				{
					PublicMethod temp;
					temp.name_method = name_method;
					temp.SetMonoMethod(method);
					temp.SetMonoObject(csharp->GetMonoObject());
					temp.references = csharp->GetReferences();
					temp.type = VarType::Var_NONE;
					csharp->methods.push_back(temp);
					continue;
				}
				char** pram = new char*[num];
				mono_method_get_param_names(method, (const char**)pram);
				if (pram[0] != nullptr)
				{
					std::string type = fullname_method;
					size_t EndName = type.find("(");
					if (EndName != std::string::npos)
					{
						type = type.substr(EndName + 1);
					}
					EndName = type.find(")");
					if (EndName != std::string::npos)
					{
						type = type.substr(0, EndName);
					}
					if (strcmp(type.c_str(), "int") == 0)
					{
						PublicMethod temp;
						temp.name_method = name_method;
						temp.name_param = pram[0];
						temp.SetMonoMethod(method);
						temp.SetMonoObject(csharp->GetMonoObject());
						temp.references = csharp->GetReferences();
						temp.type = VarType::Var_INT;
						csharp->methods.push_back(temp);
					}
					else if (strcmp(type.c_str(), "single") == 0)
					{
						PublicMethod temp;
						temp.name_method = name_method;
						temp.name_param = pram[0];
						temp.SetMonoMethod(method);
						temp.SetMonoObject(csharp->GetMonoObject());
						temp.references = csharp->GetReferences();
						temp.type = VarType::Var_FLOAT;
						csharp->methods.push_back(temp);
					}
					else if (strcmp(type.c_str(), "bool") == 0)
					{
						PublicMethod temp;
						temp.name_method = name_method;
						temp.name_param = pram[0];
						temp.SetMonoMethod(method);
						temp.SetMonoObject(csharp->GetMonoObject());
						temp.references = csharp->GetReferences();
						temp.type = VarType::Var_BOOL;
						csharp->methods.push_back(temp);
					}
					else if (strcmp(type.c_str(), "string") == 0)
					{
						PublicMethod temp;
						temp.name_method = name_method;
						temp.name_param = pram[0];
						temp.SetMonoMethod(method);
						temp.SetMonoObject(csharp->GetMonoObject());
						temp.references = csharp->GetReferences();
						temp.type = VarType::Var_STRING;
						csharp->methods.push_back(temp);
					}
					else if (strcmp(type.c_str(), "CulverinEditor.GameObject") == 0)
					{
						PublicMethod temp;
						temp.name_method = name_method;
						temp.name_param = pram[0];
						temp.SetMonoMethod(method);
						temp.SetMonoObject(csharp->GetMonoObject());
						temp.references = csharp->GetReferences();
						temp.type = VarType::Var_GAMEOBJECT;
						csharp->methods.push_back(temp);
					}
				}
				delete[] pram;
			}

			return csharp;
		}
		else
		{
			LOG("[error]Failed loading class %s\n", classname.c_str());
			std::list<MonoClass*> class_list;

			const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);

			int rows = mono_table_info_get_rows(table_info);

			/* For each row, get some of its values */
			for (int i = 1; i < rows; i++)
			{
				MonoClass* _class = nullptr;
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
				classname = name;
				const char* name_space_ = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				name_space = name_space_;
				_class = mono_class_from_name(image, name_space_, name);
				class_list.push_back(_class);
			}
			CSharpScript* csharp = new CSharpScript();
			csharp->SetDomain(GetMainDomain());
			csharp->SetImage(image);
			csharp->SetClass(class_list.begin()._Ptr->_Myval);
			csharp->SetClassName(classname);
			csharp->SetNameSpace(name_space);
			return csharp;
		}
	}
	return nullptr;
}



MonoClass* ImportScript::GetMonoClassFromImage(MonoImage* image, std::string& name_space, std::string& classname)
{
	MonoClass* entity = nullptr;
	const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
	int rows = mono_table_info_get_rows(table_info);
	/* For each row, get some of its values */
	for (int i = 0; i < 2; i++)
	{
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* classnameCS = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
		const char* name_spaceCS = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
		entity = mono_class_from_name(image, name_spaceCS, classnameCS);
		if (entity)
		{
			name_space = name_spaceCS;
			classname = classnameCS;
		}
	}
	entity = mono_class_from_name(image, name_space.c_str(), classname.c_str()); // Not correctly process! Todo elliot
	return entity;
}

//This method is called once Mono domain is init to Link C# functions 
//that users will use in their scripts with C++ functions of the application
void ImportScript::LinkFunctions()
{
	//GetLinkedObject
	mono_add_internal_call("CulverinEditor.CulverinBehaviour::GetLinkedObject", (const void*)GetLinkedObject);
	mono_add_internal_call("CulverinEditor.CulverinBehaviour::GetEnabled", (const void*)GetEnabled);
	mono_add_internal_call("CulverinEditor.CulverinBehaviour::SetEnabled", (const void*)SetEnabled);
	mono_add_internal_call("CulverinEditor.CulverinBehaviour::GetMaterialByName", (const void*)GetMaterialByName);

	//GAMEOBJECT FUNCTIONS ---------------
	mono_add_internal_call("CulverinEditor.GameObject::GetTag", (const void*)GetTag);
	mono_add_internal_call("CulverinEditor.GameObject::SetTag", (const void*)SetTag);
	mono_add_internal_call("CulverinEditor.GameObject::CompareTag", (const void*)CompareTag);
	mono_add_internal_call("CulverinEditor.GameObject::FindGameObjectWithTag", (const void*)FindGameObjectWithTag);
	//mono_add_internal_call("CulverinEditor.GameObject::CreateGameObject",(const void*)CreateGameObject);
	mono_add_internal_call("CulverinEditor.GameObject::GetOwnGameObject", (const void*)GetOwnGameObject);
	mono_add_internal_call("CulverinEditor.GameObject::ChildCount", (const void*)ChildCount);
	mono_add_internal_call("CulverinEditor.GameObject::GetChildByIndex", (const void*)GetChildByIndex);
	mono_add_internal_call("CulverinEditor.GameObject::GetChildByName", (const void*)GetChildByName);
	mono_add_internal_call("CulverinEditor.GameObject::GetChildByTagIndex", (const void*)GetChildByTagIndex);
	mono_add_internal_call("CulverinEditor.GameObject::SetActive",(const void*)SetActive);
	mono_add_internal_call("CulverinEditor.GameObject::IsActive",(const void*)IsActive);
	mono_add_internal_call("CulverinEditor.GameObject::IsStatic", (const void*)IsStatic);
	mono_add_internal_call("CulverinEditor.GameObject::Find", (const void*)Find);
	//mono_add_internal_call("CulverinEditor.GameObject::SetParent",(const void*)SetParent);
	mono_add_internal_call("CulverinEditor.GameObject::SetName",(const void*)SetName);
	mono_add_internal_call("CulverinEditor.GameObject::GetName",(const void*)GetName);
	//mono_add_internal_call("CulverinEditor.GameObject::AddComponent",(const void*)AddComponent);
	mono_add_internal_call("CulverinEditor.GameObject::GetComponent",(const void*)GetComponent);

	/* Object */
	mono_add_internal_call("CulverinEditor.Object::Instantiate(string)", (const void*)Instantiate);
	mono_add_internal_call("CulverinEditor.Object::Instantiate(string,single)", (const void*)Instantiate_respawn);
	mono_add_internal_call("CulverinEditor.Object::Destroy", (const void*)Destroy);
		mono_add_internal_call("CulverinEditor.Object::SpawnPrefabFromPos", (const MonoObject*)SpawnPrefabFromPos);
	// Transform ---------------------------
	mono_add_internal_call("CulverinEditor.Transform::GetForwardVector", (const void*)GetForwardVector);
	mono_add_internal_call("CulverinEditor.Transform::GetBackwardVector", (const void*)GetBackwardVector);
	mono_add_internal_call("CulverinEditor.Transform::GetUpVector", (const void*)GetUpVector);
	mono_add_internal_call("CulverinEditor.Transform::GetDownVector", (const void*)GetDownVector);
	mono_add_internal_call("CulverinEditor.Transform::GetLeftVector", (const void*)GetLeftVector);
	mono_add_internal_call("CulverinEditor.Transform::GetRightVector", (const void*)GetRightVector);
	mono_add_internal_call("CulverinEditor.Transform::SetForwardVector", (const void*)SetForwardVector);
	mono_add_internal_call("CulverinEditor.Transform::SetBackwardVector", (const void*)SetBackwardVector);
	mono_add_internal_call("CulverinEditor.Transform::SetUpVector", (const void*)SetUpVector);
	mono_add_internal_call("CulverinEditor.Transform::SetDownVector", (const void*)SetDownVector);
	mono_add_internal_call("CulverinEditor.Transform::SetRightVector", (const void*)SetRightVector);
	mono_add_internal_call("CulverinEditor.Transform::SetLeftVector", (const void*)SetLeftVector);
	mono_add_internal_call("CulverinEditor.Transform::GetPosition", (const void*)GetPosition);
	mono_add_internal_call("CulverinEditor.Transform::GetGlobalPosition", (const void*)GetGlobalPosition);
	mono_add_internal_call("CulverinEditor.Transform::SetPosition", (const void*)SetPosition);
	mono_add_internal_call("CulverinEditor.Transform::SetGlobalPosition", (const void*)SetGlobalPosition);
	mono_add_internal_call("CulverinEditor.Transform::Translate", (const void*)Translate);
	mono_add_internal_call("CulverinEditor.Transform::SetRotation", (const void*)SetRotation);
	mono_add_internal_call("CulverinEditor.Transform::SetGlobalRotation", (const void*)SetGlobalRotation);
	mono_add_internal_call("CulverinEditor.Transform::GetRotation", (const void*)GetRotation);
	mono_add_internal_call("CulverinEditor.Transform::GetGlobalRotation", (const void*)GetGlobalRotation);
	mono_add_internal_call("CulverinEditor.Transform::RotateAroundAxis", (const void*)IncrementRotation);
	mono_add_internal_call("CulverinEditor.Transform::SetScale", (const void*)SetScale);
	mono_add_internal_call("CulverinEditor.Transform::GetScale", (const void*)GetScale);
	mono_add_internal_call("CulverinEditor.Transform::LookAt", (const void*)LookAt);
	mono_add_internal_call("CulverinEditor.Transform::LookAtTrans", (const void*)LookAtTrans);
	mono_add_internal_call("CulverinEditor.Transform::RotateAroundAxis", (const void*)RotateAroundAxis);
	mono_add_internal_call("CulverinEditor.Transform::GetLocalTransform", (const void*)GetLocalTransform);
	mono_add_internal_call("CulverinEditor.Transform::GetGlobalTransform", (const void*)GetGlobalTransform);

	// Component ---------------------------
	mono_add_internal_call("CulverinEditor.Component::GetParentGameObject", (const void*)GetParentGameObject);

	//CONSOLE FUNCTIONS ------------------
	mono_add_internal_call("CulverinEditor.Debug.Debug::Log", (const void*)ConsoleLog);

	//SCENE MANAGEMENT FUNCTIONS ---------
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::LoadScene",(const void*)LoadScene);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::LoadSceneNoDestroy", (const void*)LoadSceneNoDestroy);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::LoadMultiSceneNoDestroy", (const void*)LoadMultisceneNoDestroy);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::CheckSceneReady", (const void*)CheckSceneReady);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::CheckMultiSceneReady", (const void*)CheckMultiSceneReady);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::RemoveNoDestroy", (const void*)RemoveNoDestroy);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::RemoveSecondaryScene", (const void*)RemoveSecondaryScene);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::ChangeToSecondaryScene", (const void*)ChangeToSecondaryScene);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::BlockGUIinput", (const void*)BlockGUIinput);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::QuitScene", (const void*)QuitScene);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::LoadNewWalkableMap", (const void*)LoadNewWalkableMap);
	//mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::PushSaveInfo(object)", (const void*)PushSaveInfoV2);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::PushSaveInfo(single)", (const void*)PushSaveInfo);
	mono_add_internal_call("CulverinEditor.SceneManagement.SceneManager::PopLoadInfo", (const void*)PopLoadInfo);
	//EVENT SYSTEM FUNCTIONS ----------------------------
	mono_add_internal_call("CulverinEditor.EventSystem.EventSystem::SendInteractiveSelected", (const void*)SendInteractiveSelected);
	mono_add_internal_call("CulverinEditor.EventSystem.EventSystem::GetInteractiveSelectedActive", (const void*)GetInteractiveSelectedActive);

	//INPUT FUNCTIONS -------------------
	
	mono_add_internal_call("CulverinEditor.Input::GetPressAnyKey", (const void*)GetPressAnyKey);
	mono_add_internal_call("CulverinEditor.Input::GetKeyDown", (const void*)GetKeyDown);
	mono_add_internal_call("CulverinEditor.Input::GetKeyUp", (const void*)GetKeyUp);
	mono_add_internal_call("CulverinEditor.Input::GetKeyRepeat", (const void*)GetKeyRepeat);
	mono_add_internal_call("CulverinEditor.Input::GetMouseButtonDown", (const void*)GetMouseButtonDown);
	mono_add_internal_call("CulverinEditor.Input::GetMouseButtonUp", (const void*)GetMouseButtonUp);
	mono_add_internal_call("CulverinEditor.Input::GetMouseButtonRepeat", (const void*)GetMouseButtonRepeat);
	mono_add_internal_call("CulverinEditor.Input::GetMousePosition", (const void*)GetMousePosition);
	mono_add_internal_call("CulverinEditor.Input::GetMouseXAxis", (const void*)GetMouseXAxis);
	mono_add_internal_call("CulverinEditor.Input::GetMouseYAxis", (const void*)GetMouseYAxis);
	mono_add_internal_call("CulverinEditor.Input::GetMouseMoutionX", (const void*)GetMouseMoutionX);
	mono_add_internal_call("CulverinEditor.Input::GetMouseMoutionY", (const void*)GetMouseMoutionY);
	mono_add_internal_call("CulverinEditor.Input::SetInputManagerActive", (const void*)SetInputManagerActive);
	mono_add_internal_call("CulverinEditor.Input::SetInputManagerBlock", (const void*)SetInputManagerBlock);
	mono_add_internal_call("CulverinEditor.Input::GetInputManagerActive", (const void*)GetInputManagerActive);
	mono_add_internal_call("CulverinEditor.Input::GetInputManagerBlock", (const void*)GetInputManagerBlock);
	mono_add_internal_call("CulverinEditor.Input::GetInput_KeyDown", (const void*)GetInput_KeyDown);
	mono_add_internal_call("CulverinEditor.Input::GetInput_KeyUp", (const void*)GetInput_KeyUp);
	mono_add_internal_call("CulverinEditor.Input::GetInput_KeyRepeat", (const void*)GetInput_KeyRepeat);
	mono_add_internal_call("CulverinEditor.Input::GetInput_MouseButtonDown", (const void*)GetInput_MouseButtonDown);
	mono_add_internal_call("CulverinEditor.Input::GetInput_MouseButtonUp", (const void*)GetInput_MouseButtonUp);
	mono_add_internal_call("CulverinEditor.Input::GetInput_ControllerAxis", (const void*)GetInput_ControllerAxis);

	mono_add_internal_call("CulverinEditor.Input::GetInput_ControllerActionName", (const void*)GetInput_ControllerActionName);
	mono_add_internal_call("CulverinEditor.Input::GetInput_ControllerKeyBindingName", (const void*)GetInput_ControllerKeyBindingName);
	mono_add_internal_call("CulverinEditor.Input::SetInputActionToChange", (const void*)SetInputActionToChange);
	mono_add_internal_call("CulverinEditor.Input::GetChangeInputActive", (const void*)GetChangeInputActive);
	mono_add_internal_call("CulverinEditor.Input::GetChangeInputState", (const void*)GetChangeInputState);
	mono_add_internal_call("CulverinEditor.Input::GetActualDeviceCombo", (const void*)GetActualDeviceCombo);

	
	mono_add_internal_call("CulverinEditor.Input::SavePlayerAction", (const void*)SavePlayerAction);
	mono_add_internal_call("CulverinEditor.Input::LoadDefaultPlayerAction", (const void*)LoadDefaultPlayerAction);

	mono_add_internal_call("CulverinEditor.Input::RumblePlay", (const void*)RumblePlay);

	//TIME FUNCTIONS -------------------
	mono_add_internal_call("CulverinEditor.Time::DeltaTime()", (const void*)GetDeltaTime);
	mono_add_internal_call("CulverinEditor.Time::RealtimeSinceStartup()", (const void*)RealtimeSinceStartup);
	mono_add_internal_call("CulverinEditor.Time::TimeScale()", (const void*)TimeScale);
	mono_add_internal_call("CulverinEditor.Time::TimeScale(single)", (const void*)TimeScaleSet);
	mono_add_internal_call("CulverinEditor.Time::StartTimePlay(single)", (const void*)StartTimePlay);
	mono_add_internal_call("CulverinEditor.Time::TimePlay()", (const void*)TimePlay);

	//MAP FUNCTIONS ----------------------
	mono_add_internal_call("CulverinEditor.Map.Map::GetMapString", (const void*)GetMapString);
	mono_add_internal_call("CulverinEditor.Map.Map::GetHeightMap", (const void*)GetHeightMap);
	mono_add_internal_call("CulverinEditor.Map.Map::GetWidthMap", (const void*)GetWidthMap);
	mono_add_internal_call("CulverinEditor.Map.Map::GetSeparation", (const void*)GetSeparation);

	//AUDIO FUNCTIONS --------------------
	mono_add_internal_call("CulverinEditor.Audio::StopAllSounds", (const void*)StopAllSounds);
	mono_add_internal_call("CulverinEditor.Audio::PauseAllSounds", (const void*)PauseAllSounds);
	mono_add_internal_call("CulverinEditor.Audio::ResumeAllSounds", (const void*)ResumeAllSounds);
	mono_add_internal_call("CulverinEditor.Audio::SetAudioVariableValue", (const void*)ChangeRTPC);
	mono_add_internal_call("CulverinEditor.Audio::ChangeState", (const void*)ChangeState);
	mono_add_internal_call("CulverinEditor.Audio::GetVolum", (const void*)GetVolume);

	mono_add_internal_call("CulverinEditor.Audio::ChangeVolume", (const void*)ChangeVolume);
	mono_add_internal_call("CulverinEditor.Audio::Mute", (const void*)Mute);
	
	//COMPONENT PARTICLE FUNCTION ----------------
	mono_add_internal_call("CulverinEditor.CompParticleSystem::ActivateEmission", (const void*)ActivateEmission);
	mono_add_internal_call("CulverinEditor.CompParticleSystem::IsEmitterActive", (const void*)IsEmitterActive);


	//COMPONENT AUDIO FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompAudio::PlayEvent", (const void*)PlayAudioEvent);
	mono_add_internal_call("CulverinEditor.CompAudio::StopEvent", (const void*)StopAudioEvent);
	mono_add_internal_call("CulverinEditor.CompAudio::SetAuxiliarySends", (const void*)SetAuxiliarySends);

	//COMPONENT UI_RECT_TRANSFORM FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompRectTransform::SetUIPosition", (const void*)SetUIPosition);
	mono_add_internal_call("CulverinEditor.CompRectTransform::GetUIPosition", (const void*)GetUIPosition);
	mono_add_internal_call("CulverinEditor.CompRectTransform::SetWidth", (const void*)SetWidth);
	mono_add_internal_call("CulverinEditor.CompRectTransform::SetHeight", (const void*)SetHeight);

	//COMPONENT UI_INTERACTIVE FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompInteractive::Activate", (const void*)Activate);
	mono_add_internal_call("CulverinEditor.CompInteractive::Deactivate", (const void*)Deactivate);
	mono_add_internal_call("CulverinEditor.CompInteractive::SetInteractivity", (const void*)SetInteractivity);
	mono_add_internal_call("CulverinEditor.CompInteractive::IsNormal", (const void*)IsNormal);
	mono_add_internal_call("CulverinEditor.CompInteractive::IsHighlighted", (const void*)IsHighlighted);
	mono_add_internal_call("CulverinEditor.CompInteractive::IsPressed", (const void*)IsPressed);
	mono_add_internal_call("CulverinEditor.CompInteractive::IsDisabled", (const void*)IsDisabled);

	
	//COMPONENT UI_INTERACTIVE FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompButton::Activate", (const void*)Activate);
	mono_add_internal_call("CulverinEditor.CompButton::Deactivate", (const void*)Deactivate);
	mono_add_internal_call("CulverinEditor.CompButton::Clicked", (const void*)Clicked);
	
	//COMPONENT UI_GRAPHIC FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompGraphic::SetRaycastTarget", (const void*)SetRaycastTarget);
	mono_add_internal_call("CulverinEditor.CompGraphic::SetRender", (const void*)SetRender);
	mono_add_internal_call("CulverinEditor.CompGraphic::ActivateRender", (const void*)ActivateRender);
	mono_add_internal_call("CulverinEditor.CompGraphic::DeactivateRender", (const void*)DeactivateRender);
	mono_add_internal_call("CulverinEditor.CompGraphic::SetColor", (const void*)SetColor);
	mono_add_internal_call("CulverinEditor.CompGraphic::SetAlpha", (const void*)SetAlpha);
	mono_add_internal_call("CulverinEditor.CompGraphic::GetWidth", (const void*)GetWidth);
	mono_add_internal_call("CulverinEditor.CompGraphic::GetHeight", (const void*)GetHeight);

	mono_add_internal_call("CulverinEditor.CompImage::FillAmount", (const void*)FillAmount);
	
	//COMPONENT TEXT
	mono_add_internal_call("CulverinEditor.CompText::SetText", (const void*)SetText);
	//COMPONENT UI_GRAPHIC FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompCanvas::SetCanvasAlpha", (const void*)SetCanvasAlpha);
	//CHECKBOX METHODS
	mono_add_internal_call("CulverinEditor.CompCheckBox::HideTick", (const void*)HideTick);
	//SLIDERFUNCIOTNRGAS
	mono_add_internal_call("CulverinEditor.CompSlider::GetFill", (const void*)GetFill);
	mono_add_internal_call("CulverinEditor.CompSlider::SetFill", (const void*)SetFill);

	//COMPONENT COLLIDER FUNCTIONS -----------------------
	mono_add_internal_call("CulverinEditor.CompCollider::GetCollidedObject", (const void*)GetCollidedObject);
	mono_add_internal_call("CulverinEditor.CompCollider::GetContactPoint", (const void*)GetContactPoint);
	mono_add_internal_call("CulverinEditor.CompCollider::GetContactNormal", (const void*)GetContactNormal);
	mono_add_internal_call("CulverinEditor.CompCollider::MoveKinematic", (const void*)MoveStaticColliderTo);
	mono_add_internal_call("CulverinEditor.CompCollider::CallOnContact", (const void*)CallOnContact);
	mono_add_internal_call("CulverinEditor.CompCollider::CallOnTriggerEnter", (const void*)CallOnTriggerEnter);
	mono_add_internal_call("CulverinEditor.CompCollider::CollisionActive", (const void*)CollisionActive);


	//MATERIAL FUNCTIONS
	mono_add_internal_call("CulverinEditor.Material::SetBool", (const void*)SetBool);
	mono_add_internal_call("CulverinEditor.Material::SetFloat", (const void*)SetFloat);
	
	//COMPONENT RIGID BODY FUNCTIONS -----------------------
	mono_add_internal_call("CulverinEditor.CompRigidBody::GetColliderPosition", (const void*)GetColliderPosition);
	mono_add_internal_call("CulverinEditor.CompRigidBody::GetColliderQuaternion", (const void*)GetColliderQuaternion);
	mono_add_internal_call("CulverinEditor.CompRigidBody::RemoveJoint", (const void*)RemoveJoint);
	mono_add_internal_call("CulverinEditor.CompRigidBody::MoveKinematic", (const void*)MoveKinematic);
	mono_add_internal_call("CulverinEditor.CompRigidBody::ApplyForce", (const void*)ApplyForce);
	mono_add_internal_call("CulverinEditor.CompRigidBody::ApplyImpulse", (const void*)ApplyImpulse);
	mono_add_internal_call("CulverinEditor.CompRigidBody::ApplyTorqueForce", (const void*)ApplyTorqueForce);
	mono_add_internal_call("CulverinEditor.CompRigidBody::ApplyTorqueImpulse", (const void*)ApplyTorqueImpulse);
	mono_add_internal_call("CulverinEditor.CompRigidBody::LockMotion", (const void*)LockMotion);
	mono_add_internal_call("CulverinEditor.CompRigidBody::LockRotation", (const void*)LockRotation);
	mono_add_internal_call("CulverinEditor.CompRigidBody::LockTransform", (const void*)LockTransform);
	mono_add_internal_call("CulverinEditor.CompRigidBody::UnLockMotion", (const void*)UnLockMotion);
	mono_add_internal_call("CulverinEditor.CompRigidBody::UnLockRotation", (const void*)UnLockRotation);
	mono_add_internal_call("CulverinEditor.CompRigidBody::UnLockTransform", (const void*)UnLockTransform);
	mono_add_internal_call("CulverinEditor.CompRigidBody::ResetForce", (const void*)ResetForce);
	mono_add_internal_call("CulverinEditor.CompRigidBody::WakeUp", (const void*)WakeUp);
	mono_add_internal_call("CulverinEditor.CompRigidBody::SetAtMaxJointPose", (const void*)SetAtMaxJointPose);

	//COMPONENT MATERIAL FUNCTIONS ---------------------------
	mono_add_internal_call("CulverinEditor.CompMaterial::SetAlbedo", (const void*)SetAlbedo);
	mono_add_internal_call("CulverinEditor.CompMaterial::SetNormals", (const void*)SetNormals);
	mono_add_internal_call("CulverinEditor.CompMaterial::SetAmbientOcclusion", (const void*)SetAmbientOcclusion);

	//COMPONENT JOINT FUNCTIONS ----------------------------
	mono_add_internal_call("CulverinEditor.CompJoint::DeleteJoint", (const void*)DeleteJoint);

	//COMPONENT ANIMATION FUNCTIONS -------------------------
	mono_add_internal_call("CulverinEditor.CompAnimation::PlayAnimation", (const void*)PlayAnimation);
	mono_add_internal_call("CulverinEditor.CompAnimation::SetTransition", (const void*)SetTransition);
	mono_add_internal_call("CulverinEditor.CompAnimation::IsAnimationStopped", (const void*)IsAnimationStopped);
	mono_add_internal_call("CulverinEditor.CompAnimation::IsAnimationRunning", (const void*)IsAnimationRunning);
	mono_add_internal_call("CulverinEditor.CompAnimation::IsAnimOverXTime", (const void*)IsAnimOverXTime);
	mono_add_internal_call("CulverinEditor.CompAnimation::SetClipsSpeed", (const void*)SetClipsSpeed);
	mono_add_internal_call("CulverinEditor.CompAnimation::GetClipDuration", (const void*)GetClipDuration);
	mono_add_internal_call("CulverinEditor.CompAnimation::SetClipDuration", (const void*)SetClipDuration);
	mono_add_internal_call("CulverinEditor.CompAnimation::SetFirstActiveBlendingClip", (const void*)SetFirstActiveBlendingClip);
	mono_add_internal_call("CulverinEditor.CompAnimation::SetFirstActiveBlendingClipWeight", (const void*)SetFirstActiveBlendingClipWeight);
	mono_add_internal_call("CulverinEditor.CompAnimation::SetBlendInTime", (const void*)SetBlendInTime);
	mono_add_internal_call("CulverinEditor.CompAnimation::PlayAnimationNode", (const void*)PlayAnimationNode);
	//MODULE PHYSICS FUNCTIONS ----------------------------
	mono_add_internal_call("CulverinEditor.PhysX::RayCast", (const void*)RayCast);

	//COMPONENT LIGHT FUNCTIONS ----------------------------
	mono_add_internal_call("CulverinEditor.CompLight::GetIntensity", (const void*)GetIntensity);
	mono_add_internal_call("CulverinEditor.CompLight::SetIntensity", (const void*)SetIntensity);
	mono_add_internal_call("CulverinEditor.CompLight::GetConstant", (const void*)GetConstant);
	mono_add_internal_call("CulverinEditor.CompLight::SetConstant", (const void*)SetConstant);
	mono_add_internal_call("CulverinEditor.CompLight::GetLinear", (const void*)GetLinear);
	mono_add_internal_call("CulverinEditor.CompLight::SetLinear", (const void*)SetLinear);
	mono_add_internal_call("CulverinEditor.CompLight::GetQuadratic", (const void*)GetQuadratic);
	mono_add_internal_call("CulverinEditor.CompLight::SetQuadratic", (const void*)SetQuadratic);

	//RANDOM FUNCTIONS ----------------------------
	mono_add_internal_call("CulverinEditor.Random::Range(int,int)", (const void*)RangeInt);
	mono_add_internal_call("CulverinEditor.Random::Range(single,single)", (const void*)RangeFloat);
	mono_add_internal_call("CulverinEditor.Random::Range(int,int,int)", (const void*)RangeIntNoRepeat);
	//SCREEN FUNCTRIONS ---------------------------
	mono_add_internal_call("CulverinEditor.Screen.Screen::SetFullScreen", (const void*)SetFullScreen);
	mono_add_internal_call("CulverinEditor.Screen.Screen::SetWindowed", (const void*)SetWindowed);
	mono_add_internal_call("CulverinEditor.Screen.Screen::SetBordeless", (const void*)SetBordeless);
	mono_add_internal_call("CulverinEditor.Screen.Screen::SetVSync", (const void*)SetVSync);
	mono_add_internal_call("CulverinEditor.Screen.Screen::ShowFPS", (const void*)ShowFPS);
	mono_add_internal_call("CulverinEditor.Screen.Screen::SwapControllerVibration", (const void*)SwapControllerVibration);
}

//Log messages into Engine Console
void ImportScript::ConsoleLog(MonoObject* string)
{
	if (string != nullptr && !App->mode_game)
	{
		//Pass from MonoString to const char*
		MonoString* strings = mono_object_to_string(string, NULL);
		const char* message = mono_string_to_utf8(strings);

		LOG(message);
	}
}

void ImportScript::LoadScene(MonoString* scene_name)
{
	if (scene_name != nullptr)
	{
		const char* scene = mono_string_to_utf8(scene_name);

		std::string directory_scene = DIRECTORY_ASSETS;
		directory_scene += scene;
		directory_scene += ".scene.json";
		/*
		----------------------------------------------------------------

		This is a vicente fix, in the future we need to make an event to change scene
		to turn the focus into nullptr
		*/

		//----------------------------------------------------------------


		App->SetActualScene(directory_scene.c_str());
		App->WantToLoad(true);
	}
}

void ImportScript::LoadSceneNoDestroy(MonoString* scene_name)
{
	if (scene_name != nullptr)
	{
		const char* scene = mono_string_to_utf8(scene_name);

		std::string directory_scene = DIRECTORY_ASSETS;
		directory_scene += scene;
		directory_scene += ".scene.json";

		/*
		----------------------------------------------------------------
		This is a vicente fix, in the future we need to make an event to change scene
		to turn the focus into nullptr
		*/
		//----------------------------------------------------------------
		App->SetActualScene(directory_scene.c_str());
		App->DontDestroyOnLoad();
	}
}

void ImportScript::LoadMultisceneNoDestroy(MonoString * main_scene_name, MonoString * secondary_scene_name)
{
	std::string directory_scene;
	if (main_scene_name != nullptr)
	{
		const char* scene = mono_string_to_utf8(main_scene_name);

		directory_scene = DIRECTORY_ASSETS;
		directory_scene += scene;
		directory_scene += ".scene.json";

		/*
		----------------------------------------------------------------
		This is a vicente fix, in the future we need to make an event to change scene
		to turn the focus into nullptr
		*/
		//----------------------------------------------------------------
		App->SetActualScene(directory_scene.c_str());
		App->LoadMultiScene();
	}
	if (secondary_scene_name != nullptr)
	{
		const char* scene2 = mono_string_to_utf8(secondary_scene_name);

		directory_scene = DIRECTORY_ASSETS;
		directory_scene += scene2;
		directory_scene += ".scene.json";

		/*
		----------------------------------------------------------------
		This is a vicente fix, in the future we need to make an event to change scene
		to turn the focus into nullptr
		*/
		//----------------------------------------------------------------
		App->SetSecondaryScene(directory_scene.c_str());
		App->LoadMultiScene();
	}
}

bool ImportScript::CheckSceneReady()
{
	return !App->dont_destroy_on_load;
}

bool ImportScript::CheckMultiSceneReady()
{
	return !App->load_multi_scene;
}

void ImportScript::RemoveNoDestroy()
{
	// Remove Dont Destroy On Load on final frame
	App->remove_dont_destroy_on_load = true;
}

void ImportScript::RemoveSecondaryScene()
{
	App->remove_secondary_scene = true;
}

void ImportScript::ChangeToSecondaryScene()
{
	App->change_to_secondary_scene = true;
}

void ImportScript::BlockGUIinput()
{
	App->activate_gui_input = true;
}

void ImportScript::QuitScene()
{
	App->input->quit = true;
}

void ImportScript::LoadNewWalkableMap(MonoString* walkable_map)
{
	if (walkable_map != nullptr)
	{
		const char* map = mono_string_to_utf8(walkable_map);
		App->map->imported_map = map;
		App->map->ImportMap(true);
	}
}

void ImportScript::PushSaveInfoV2(MonoObject* info)
{
	if (info != nullptr)
	{
		//MonoClass* classT = mono_object_get_class(info);
		//MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		//MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		//MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		//float3 info_p;

		//if (x_field) mono_field_get_value(info, x_field, &info_p.x);
		//if (y_field) mono_field_get_value(info, y_field, &info_p.y);
		//if (z_field) mono_field_get_value(info, z_field, &info_p.z);

		//App->scene->infoPlayer.push_back(info_p);
	}
}

void ImportScript::PushSaveInfo(float hp)
{
	App->scene->infoPlayer.push_back(hp);
}

float ImportScript::PopLoadInfo()
{
	if (App->scene->infoPlayer.size() > 0)
	{
		float ret = App->scene->infoPlayer[App->scene->infoPlayer.size() - 1];
		if (App->scene->infoPlayer.size() - 1 == -1)
		{
			return  1503.0f;
		}
		App->scene->infoPlayer.pop_back();
		return ret;
	}
	return  1503.0f;
}

void ImportScript::SendInteractiveSelected(MonoObject * interactive)
{
	if (interactive != nullptr)
	{
		
		GameObject*  go = App->importer->iScript->GetGameObject(interactive);
		if (go == nullptr)
			return;

		Event pass_selected;
		pass_selected.Set_event_data(EventType::EVENT_PASS_SELECTED);
		CompInteractive* to_send = (CompInteractive*)go->FindComponentByType(Comp_Type::C_BUTTON);
		if(to_send == nullptr)
			to_send = (CompInteractive*)go->FindComponentByType(Comp_Type::C_CHECK_BOX);
		if (to_send == nullptr)
			to_send = (CompInteractive*)go->FindComponentByType(Comp_Type::C_SLIDER);

		pass_selected.pass_selected.component = to_send;
		if (pass_selected.pass_selected.component == nullptr)
			return;

		PushEvent(pass_selected);
		
	}
}

bool ImportScript::GetInteractiveSelectedActive()
{
	return App->render_gui->selected!=nullptr;
}

mono_bool ImportScript::GetPressAnyKey()
{
	return App->input->GetPressAnyKey();
}

mono_bool ImportScript::GetKeyDown(int key)
{
	if (App->input->GetKey(key) == KEY_STATE::KEY_DOWN)
	{
		return true;
	}
	if(key == SDL_SCANCODE_UNKNOWN)
	{
		LOG("[error]Error with key pressed!");
	}
	return false;
}

mono_bool ImportScript::GetKeyUp(int key)
{
	if (App->input->GetKey(key) == KEY_STATE::KEY_UP)
	{
		return true;
	}
	if (key == SDL_SCANCODE_UNKNOWN)
	{
		LOG("[error]Error with key pressed!");
	}
	return false;
}

mono_bool ImportScript::GetKeyRepeat(int key)
{
	if (App->input->GetKey(key) == KEY_STATE::KEY_REPEAT)
	{
		return true;
	}
	if (key == SDL_SCANCODE_UNKNOWN)
	{
		LOG("[error]Error with key pressed!");
	}
	return false;
}

mono_bool ImportScript::GetMouseButtonDown(int buttonmouse)
{
	if (buttonmouse >= 0 && buttonmouse < 4)
	{
		return (App->input->GetMouseButton(buttonmouse) == KEY_STATE::KEY_DOWN);
	}
	return false;
}

mono_bool ImportScript::GetMouseButtonUp(int buttonmouse)
{
	if (buttonmouse >= 0 && buttonmouse < 4)
	{
		return (App->input->GetMouseButton(buttonmouse) == KEY_STATE::KEY_UP);
	}
	return false;
}

mono_bool ImportScript::GetMouseButtonRepeat(int buttonmouse)
{
	if (buttonmouse >= 0 && buttonmouse < 4)
	{
		return (App->input->GetMouseButton(buttonmouse) == KEY_STATE::KEY_REPEAT);
	}
	return false;
}

MonoObject* ImportScript::GetMousePosition()
{
	return current->GetMousePosition();
}

int ImportScript::GetMouseXAxis()
{
	return App->input->GetMouseXMotionNormalized();
}

int ImportScript::GetMouseYAxis()
{
	return App->input->GetMouseYMotionNormalized();
}

int ImportScript::GetMouseMoutionX()
{
	return App->input->GetMouseXMotionGlobal();
}

int ImportScript::GetMouseMoutionY()
{
	return App->input->GetMouseYMotionGlobal();
}

void ImportScript::SetInputManagerActive(MonoString* str, mono_bool active)
{
	App->input->player_action->SetInputManagerActive(mono_string_to_utf8(str),active);
}

void ImportScript::SetInputManagerBlock(MonoString* str, mono_bool active)
{
	App->input->player_action->SetInputManagerBlock(mono_string_to_utf8(str), active);
}

mono_bool ImportScript::GetInputManagerActive(MonoString * str, mono_bool active)
{

	return	App->input->player_action->GetInputManagerActive(mono_string_to_utf8(str));

}

mono_bool ImportScript::GetInputManagerBlock(MonoString * str, mono_bool active)
{
	return	App->input->player_action->GetInputManagerBlock(mono_string_to_utf8(str));

}

mono_bool ImportScript::GetInput_KeyDown(MonoString* name, MonoString* input) 
{
	return App->input->player_action->GetInput_KeyDown(mono_string_to_utf8(name), mono_string_to_utf8(input));
}

mono_bool ImportScript::GetInput_KeyUp(MonoString* name, MonoString* input)
{
	return App->input->player_action->GetInput_KeyUp(mono_string_to_utf8(name), mono_string_to_utf8(input));
}

mono_bool ImportScript::GetInput_KeyRepeat(MonoString* name, MonoString* input)
{
	return App->input->player_action->GetInput_KeyRepeat(mono_string_to_utf8(name), mono_string_to_utf8(input));
}

mono_bool ImportScript::GetInput_MouseButtonDown(MonoString* name, MonoString* input)
{
	return App->input->player_action->GetInput_MouseButtonDown(mono_string_to_utf8(name), mono_string_to_utf8(input));
}

mono_bool ImportScript::GetInput_MouseButtonUp(MonoString* name, MonoString* input)
{
	return App->input->player_action->GetInput_MouseButtonUp(mono_string_to_utf8(name), mono_string_to_utf8(input));
}

float ImportScript::GetInput_ControllerAxis(MonoString* name, MonoString* input)
{
	return App->input->player_action->GetInput_ControllerAxis(mono_string_to_utf8(name), mono_string_to_utf8(input));
}

MonoString * ImportScript::GetInput_ControllerActionName(MonoString * name, MonoString * input, int device, bool negative_key)
{
	return mono_string_new_wrapper(App->input->player_action->GetInput_ControllerActionName(mono_string_to_utf8(name), mono_string_to_utf8(input), device, negative_key));
}

MonoString * ImportScript::GetInput_ControllerKeyBindingName(MonoString * name, MonoString * input, int device, bool negative_key)
{
	return mono_string_new_wrapper(App->input->player_action->GetInput_ControllerKeyBindingName(mono_string_to_utf8(name), mono_string_to_utf8(input), device, negative_key));
}

void ImportScript::SetInputActionToChange(MonoString * name, MonoString * input, int device, bool negative_key)
{
	App->input->player_action->SetInputActionToChange(mono_string_to_utf8(name), mono_string_to_utf8(input), device, negative_key);
}
bool ImportScript::GetChangeInputActive()
{
	return App->input->player_action->GetChangeInputActive();
}
int ImportScript::GetChangeInputState()
{
	return App->input->player_action->GetChangeInputState();;
}
int ImportScript::GetActualDeviceCombo()
{
	return App->input->GetActualDeviceCombo();;
}
void ImportScript::SavePlayerAction()
{
	App->input->SavePlayerAction();
}
void ImportScript::LoadDefaultPlayerAction()
{
	App->input->LoadDefaultPlayerAction();
}
void ImportScript::RumblePlay(float intensity, int milliseconds)
{
	App->input->RumblePlay(intensity, milliseconds);
}

// TIME -----------------------------------------------
float ImportScript::GetDeltaTime()
{
	return App->game_time.time_scale * App->real_time.dt;
}

float ImportScript::RealtimeSinceStartup()
{
	return App->game_time.game_start_time;
}

float ImportScript::TimeScale()
{
	return App->game_time.time_scale;
}

void ImportScript::TimeScaleSet(float time)
{
	App->game_time.time_scale = time;
}

void ImportScript::StartTimePlay(float time)
{
	App->game_time.timePlay = time;
}

float ImportScript::TimePlay()
{
	return App->game_time.timePlay;
}


bool ImportScript::GetEnabled(MonoObject* object)
{
	return current->GetEnabled(object);
}

void ImportScript::SetEnabled(MonoObject* object, mono_bool active)
{
	current->SetEnabled(object, active);
}

MonoObject* ImportScript::GetLinkedObject(MonoObject* object, MonoString* name)
{
	std::string name_variable = mono_string_to_utf8(name);

	return current->GetMonoObjectLink(name_variable);
}

mono_bool ImportScript::IsStatic(MonoObject * object)
{
	return current->IsStatic(object);
}

mono_bool ImportScript::IsActive(MonoObject* object)
{
	return current->IsActive(object);
}

void ImportScript::SetActive(MonoObject* object, mono_bool active)
{
	current->SetActive(object, active);
}

MonoObject* ImportScript::Find(MonoObject* object, MonoString* name)
{
	return current->Find(object, name);
}

MonoObject* ImportScript::GetOwnGameObject()
{
	return current->GetOwnGameObject();
}

int ImportScript::ChildCount(MonoObject* object)
{
	return current->ChildCount(object);
}

MonoObject* ImportScript::GetChildByIndex(MonoObject* object, int index)
{
	return current->GetChildByIndex(object, index);
}

MonoObject* ImportScript::GetChildByName(MonoObject* object, MonoString* name)
{
	return current->GetChildByName(object, name);
}

MonoObject*	ImportScript::GetChildByTagIndex(MonoObject* object, MonoString* tag, int index)
{
	return current->GetChildByTagIndex(object, tag, index);
}

void ImportScript::SetName(MonoObject* object, MonoString* name)
{
	current->SetName(object, name);
}

MonoString* ImportScript::GetName(MonoObject* object)
{
	return current->GetName(object);
}

MonoString* ImportScript::GetTag(MonoObject* object)
{
	return current->GetTag(object);
}

void ImportScript::SetTag(MonoObject* object, MonoString* name)
{
	current->SetTag(object, name);
}

mono_bool ImportScript::CompareTag(MonoObject* object, MonoString* tag)
{
	return current->CompareTag(object, tag);
}

MonoObject* ImportScript::FindGameObjectWithTag(MonoObject* object, MonoString* tag)
{
	return current->FindGameObjectWithTag(object, tag);
}

void ImportScript::CreateGameObject(MonoObject* object)
{
	//current->CreateGameObject(object);
}

MonoObject* ImportScript::GetComponent(MonoObject* object, MonoReflectionType* type)
{
	return current->GetComponent(object, type);
}

MonoObject* ImportScript::Instantiate(MonoObject* object, MonoString* prefab)
{
	return current->Instantiate(object, prefab);
}

MonoObject*	ImportScript::Instantiate_respawn(MonoObject* object, MonoString* prefab, float time)
{
	return current->Instantiate_respawn(object, prefab, time);
}

void ImportScript::Destroy(MonoObject* object, MonoObject* gameobject, float time)
{
	current->Destroy(gameobject, time);
}

MonoObject* ImportScript::GetForwardVector(MonoObject* object)
{
	return current->GetForwardVector(object);
}

MonoObject* ImportScript::GetBackwardVector(MonoObject* object)
{
	return current->GetBackwardVector(object);
}

MonoObject* ImportScript::GetUpVector(MonoObject* object)
{
	return current->GetUpVector(object);
}

MonoObject* ImportScript::GetDownVector(MonoObject* object)
{
	return current->GetDownVector(object);
}

MonoObject* ImportScript::GetRightVector(MonoObject* object)
{
	return current->GetRightVector(object);
}

MonoObject* ImportScript::GetLeftVector(MonoObject* object)
{
	return current->GetLeftVector(object);
}

void ImportScript::SetForwardVector(MonoObject* object, MonoObject* vector3)
{
	current->SetForwardVector(object, vector3);
}

void ImportScript::SetBackwardVector(MonoObject* object, MonoObject* vector3)
{
	current->SetBackwardVector(object, vector3);
}

void ImportScript::SetUpVector(MonoObject* object, MonoObject* vector3)
{
	current->SetUpVector(object, vector3);
}

void ImportScript::SetDownVector(MonoObject* object, MonoObject* vector3)
{
	current->SetDownVector(object, vector3);
}

void ImportScript::SetRightVector(MonoObject * object, MonoObject * vector3)
{
	current->SetRightVector(object, vector3);
}

void ImportScript::SetLeftVector(MonoObject * object, MonoObject * vector3)
{
	current->SetLeftVector(object, vector3);
}

MonoObject* ImportScript::GetPosition(MonoObject* object)
{
	return current->GetPosition(object);
}

MonoObject * ImportScript::GetGlobalPosition(MonoObject * object)
{
	return current->GetGlobalPosition(object);
}

void ImportScript::SetPosition(MonoObject* object, MonoObject* vector3)
{
	current->SetPosition(object, vector3);
}

void ImportScript::SetGlobalPosition(MonoObject* object, MonoObject* vector3)
{
	current->SetGlobalPosition(object, vector3);
}

void ImportScript::Translate(MonoObject* object, MonoObject* vector3)
{
	current->Translate(object, vector3);
}

MonoObject* ImportScript::GetRotation(MonoObject* object)
{
	return current->GetRotation(object);
}

MonoObject * ImportScript::GetGlobalRotation(MonoObject* object)
{
	return current->GetGlobalRotation(object);
}

void ImportScript::SetRotation(MonoObject* object, MonoObject* vector3)
{
	current->SetRotation(object, vector3);
}

void ImportScript::SetGlobalRotation(MonoObject * object, MonoObject * vector3)
{
	current->SetGlobalRotation(object, vector3);
}

void ImportScript::IncrementRotation(MonoObject* object, MonoObject* vector3)
{
	current->IncrementRotation(object, vector3);
}

void ImportScript::RotateAroundAxis(MonoObject * object, MonoObject * vector3, float angle)
{
	current->RotateAroundAxis(object, vector3, angle);
}

void ImportScript::SetScale(MonoObject * object, MonoObject * vector3)
{
	current->SetScale(object, vector3);
}

MonoObject * ImportScript::GetScale(MonoObject * object)
{
	return current->GetScale(object);
}

void ImportScript::LookAt(MonoObject * object, MonoObject * vector3)
{
	current->LookAt(object, vector3);
}

void ImportScript::LookAtTrans(MonoObject * object, MonoObject * trans)
{
	current->LookAtTrans(object, trans);
}

MonoObject* ImportScript::GetLocalTransform(MonoObject* object)
{
	return current->GetLocalTransform(object);
}

MonoObject*	ImportScript::GetGlobalTransform(MonoObject* object)
{
	return current->GetGlobalTransform(object);
}

// Component ---------------------
MonoObject* ImportScript::GetParentGameObject(MonoObject* object)
{
	return current->GetParentGameObject(object);
}

// Map Functions -------------
MonoString* ImportScript::GetMapString(MonoObject* object)
{
	return current->GetMapString(object);
}

int ImportScript::GetHeightMap()
{
	return App->map->GetHeightMap();
}

int ImportScript::GetWidthMap()
{
	return App->map->GetWidthMap();
}

float ImportScript::GetSeparation()
{
	return App->map->GetSeparation();
}


// Module Audio Functions ------------

void ImportScript::StopAllSounds()
{
	App->audio->StopSounds();
}

void ImportScript::PauseAllSounds()
{
	App->audio->PauseSounds();
}

void ImportScript::ResumeAllSounds()
{
	App->audio->ResumeSounds();
}

void ImportScript::ChangeRTPC(MonoString* var_name, float value)
{
	App->audio->SetAudioVariableValue(mono_string_to_utf8(var_name), value);
}

void ImportScript::ChangeState(MonoString * group_name, MonoString * state_name)
{
	App->audio->ChangeState(mono_string_to_utf8(group_name), mono_string_to_utf8(state_name));
	//LOG("State: %s  Value %s", mono_string_to_utf8(group_name), mono_string_to_utf8(state_name))
}
float ImportScript::GetVolume(MonoString * group_name)
{
	return App->audio->GetVolume();
	//LOG("State: %s  Value %s", mono_string_to_utf8(group_name), mono_string_to_utf8(state_name))
}
void ImportScript::ChangeVolume(float volume)
{
	App->audio->ChangeVolume(volume);
}

void ImportScript::Mute(bool m)
{
	App->audio->Mute(m);
}

//Component Particle Functions ------------

void ImportScript::ActivateEmission(MonoObject* object, bool a)
{
	current->ActivateEmission(object, a);
}

bool ImportScript::IsEmitterActive(MonoObject* object)
{
	return current->IsEmitterActive(object);
}

// Component Audio Functions ------------

void ImportScript::PlayAudioEvent(MonoObject* object, MonoString* name)
{
	current->PlayAudioEvent(object, name);
}

void ImportScript::StopAudioEvent(MonoObject* object, MonoString* name)
{
	current->StopAudioEvent(object, name);
}

void ImportScript::SetAuxiliarySends(MonoObject * object, MonoString * bus, float value)
{
	current->SetAuxiliarySends(object, bus, value);
}

void ImportScript::Activate(MonoObject * object, int uid)
{
	current->Activate(object, uid);
}

void ImportScript::Deactivate(MonoObject * object, int uid)
{
	current->Deactivate(object, uid);
}

void ImportScript::SetInteractivity(MonoObject * object, mono_bool enable)
{
	current->SetInteractivity(object, enable);
}
bool ImportScript::IsNormal(MonoObject * object)
{
	return current->IsNormal(object);
}
bool ImportScript::IsHighlighted(MonoObject * object)
{
	return current->IsHighlighted(object);
}

bool ImportScript::IsPressed(MonoObject * object)
{
	return current->IsPressed(object);
}

bool ImportScript::IsDisabled(MonoObject * object)
{
	return current->IsDisabled(object);
}
void ImportScript::Clicked(MonoObject * object)
{
	current->Clicked(object);
}

void ImportScript::SetRaycastTarget(MonoObject * object, mono_bool flag)
{
	current->SetRaycastTarget(object, flag);
}
void ImportScript::SetRender(MonoObject * object, mono_bool flag)
{
	current->SetRender(object, flag);
}

void ImportScript::ActivateRender(MonoObject * object)
{
	current->ActivateRender(object);
}

void ImportScript::DeactivateRender(MonoObject * object)
{
	current->DeactivateRender(object);
}

void ImportScript::SetAlpha(MonoObject * object, float alpha)
{
	current->SetAlpha(object, alpha);
}

int ImportScript::GetHeight(MonoObject * object)
{
	return current->GetWidth(object);
}

int ImportScript::GetWidth(MonoObject * object)
{
	return current->GetHeight(object);
}

void ImportScript::HideTick(MonoObject * object)
{
	current->HideTick(object);
}

float ImportScript::GetFill(MonoObject * object)
{
	return current->GetFill(object);
}
void ImportScript::SetFill(MonoObject * object,float value)
{
	current->SetFill(object,value);
}

void ImportScript::SetText(MonoObject * object, MonoString * text)
{
	current->SetText(object, text);
}

void ImportScript::SetCanvasAlpha(MonoObject * object, float alpha)
{
	current->SetCanvasAlpha(object, alpha);

}

void ImportScript::FillAmount(MonoObject * object, float value)
{
	current->FillAmount(object, value);
}

void ImportScript::SetColor(MonoObject * object, MonoObject * color, float alpha)
{
	current->SetColor(object, color, alpha);
}

MonoObject * ImportScript::GetCollidedObject(MonoObject* object)
{
	return current->GetCollidedObject(object);
}

MonoObject * ImportScript::GetContactPoint(MonoObject * object)
{
	return current->GetContactPoint(object);
}

MonoObject * ImportScript::GetContactNormal(MonoObject * object)
{
	return current->GetContactNormal(object);
}

void ImportScript::MoveStaticColliderTo(MonoObject * object, MonoObject * position)
{
	current->MoveStaticColliderTo(object, position);
}

void ImportScript::CallOnContact(MonoObject * object)
{
	current->CallOnContact(object);
}

void ImportScript::CallOnTriggerEnter(MonoObject * object)
{
	current->CallOnTriggerEnter(object);
}

void ImportScript::CollisionActive(MonoObject * object, bool active)
{
	current->CollisionActive(object, active);
}

MonoObject* ImportScript::GetColliderPosition(MonoObject* object)
{
	return current->GetColliderPosition(object);
}

MonoObject* ImportScript::GetColliderQuaternion(MonoObject* object)
{
	return current->GetColliderQuaternion(object);
}

void ImportScript::RemoveJoint(MonoObject * object)
{
	current->RemoveJoint(object);
}

void ImportScript::MoveKinematic(MonoObject* object, MonoObject* position, MonoObject* rotation)
{
	current->MoveKinematic(object, position, rotation);
}

void ImportScript::ApplyForce(MonoObject * object, MonoObject * force)
{
	current->ApplyForce(object, force);
}

void ImportScript::ApplyImpulse(MonoObject * object, MonoObject * impulse)
{
	current->ApplyImpulse(object, impulse);
}

void ImportScript::ApplyTorqueForce(MonoObject * object, MonoObject * force)
{
	current->ApplyTorqueForce(object, force);
}

void ImportScript::ApplyTorqueImpulse(MonoObject * object, MonoObject * impulse)
{
	current->ApplyTorqueImpulse(object, impulse);
}

void ImportScript::LockMotion(MonoObject * object)
{
	current->LockMotion(object);
}

void ImportScript::LockRotation(MonoObject * object)
{
	current->LockRotation(object);
}

void ImportScript::LockTransform(MonoObject * object)
{
	current->LockTransform(object);
}

void ImportScript::UnLockMotion(MonoObject * object)
{
	current->UnLockMotion(object);
}

void ImportScript::UnLockRotation(MonoObject * object)
{
	current->UnLockRotation(object);
}

void ImportScript::UnLockTransform(MonoObject * object)
{
	current->UnLockTransform(object);
}

void ImportScript::ResetForce(MonoObject * object)
{
	current->ResetForce(object);
}

void ImportScript::WakeUp(MonoObject * object)
{
	current->WakeUp(object);
}

void ImportScript::SetAtMaxJointPose(MonoObject * object)
{
	current->SetAtMaxJointPose(object);
}

void ImportScript::DeleteJoint(MonoObject * object)
{
	current->DestroyJoint(object);
}

void ImportScript::PlayAnimation(MonoObject * object, MonoString * string, mono_bool blending)
{
	current->PlayAnimation(object, string, blending);
}

void ImportScript::SetTransition(MonoObject * object, MonoString * string, mono_bool condition)
{
	current->SetTransition(object, string, condition);
}

mono_bool ImportScript::IsAnimationStopped(MonoObject * object, MonoString * string)
{
	return current->IsAnimationStopped(object, string);
}

mono_bool ImportScript::IsAnimationRunning(MonoObject * object, MonoString * string)
{
	return current->IsAnimationRunning(object, string);
}

mono_bool ImportScript::IsAnimOverXTime(MonoObject * object, float num_between_0_1)
{
	return current->IsAnimOverXTime(object, num_between_0_1);
}

void ImportScript::SetClipsSpeed(MonoObject * object, float speed_value)
{
	current->SetClipsSpeed(object, speed_value);
}

float ImportScript::GetClipDuration(MonoObject * object, MonoString * string)
{
	return current->GetClipDuration(object, string);
}

void ImportScript::SetClipDuration(MonoObject * object, MonoString * string, float duration)
{
	current->SetClipDuration(object, string, duration);
}

void ImportScript::SetFirstActiveBlendingClip(MonoObject * object, MonoString * string)
{
	current->SetFirstActiveBlendingClip(object, string);
}

void ImportScript::SetFirstActiveBlendingClipWeight(MonoObject * object, float weight)
{
	current->SetFirstActiveBlendingClipWeight(object, weight);
}

void ImportScript::SetSecondActiveBlendingClip(MonoObject * object, MonoString * string)
{
	current->SetSecondActiveBlendingClip(object,string);
}

void ImportScript::SetSecondctiveBlendingClipWeight(MonoObject * object, float weight)
{
	current->SetSecondActiveBlendingClipWeight(object, weight);
}

void ImportScript::SetBlendInTime(MonoObject * object, MonoString * string, float time)
{
	current->SetBlendInTime(object, string, time);
}

void ImportScript::PlayAnimationNode(MonoObject * object, MonoString * string)
{
	current->PlayAnimationNode(object, string);
}

MonoObject * ImportScript::SpawnPrefabFromPos(MonoObject * object, MonoString * prefab_name, MonoObject* realposition, MonoObject* realrotation, MonoObject* prefabpos)
{
	return current->SpawnPrefabFromPos(object, prefab_name, realposition, realrotation, prefabpos);
}

void ImportScript::SetAlbedo(MonoObject * object, MonoString * string)
{
	current->SetAlbedo(object, string);
}

void ImportScript::SetNormals(MonoObject * object, MonoString * string)
{
	current->SetNormals(object, string);
}

void ImportScript::SetAmbientOcclusion(MonoObject * object, MonoString * string)
{
	current->SetAmbientOcclusion(object, string);
}

float ImportScript::GetIntensity(MonoObject * object)
{
	return current->GetIntensity(object);
}

void ImportScript::SetIntensity(MonoObject * object,float value)
{
	current->SetIntensity(object, value);
}

float ImportScript::GetConstant(MonoObject * object)
{
	return current->GetConstant(object);
}

void ImportScript::SetConstant(MonoObject * object, float value)
{
	current->SetConstant(object, value);
}

float ImportScript::GetLinear(MonoObject * object)
{
	return current->GetLinear(object);
}

void ImportScript::SetLinear(MonoObject * object, float value)
{
	current->SetLinear(object, value);
}

float ImportScript::GetQuadratic(MonoObject * object)
{
	return current->GetQuadratic(object);
}

void ImportScript::SetQuadratic(MonoObject * object, float value)
{
	current->SetQuadratic(object, value);
}

void ImportScript::SetUIPosition(MonoObject * object, MonoObject * vector3)
{
	current->SetUIPosition(object, vector3);
}

MonoObject * ImportScript::GetUIPosition(MonoObject * object)
{
	return current->GetUIPosition(object);
}

void ImportScript::SetWidth(MonoObject * object, int value)
{
	current->SetWidth(object, value);
}

void ImportScript::SetHeight(MonoObject * object, int value)
{
	current->SetHeight(object, value);
}
MonoObject * ImportScript::RayCast(MonoObject * origin, MonoObject * direction, float distance)
{
	return current->RayCast(origin, direction, distance);
}

void ImportScript::SetBool(MonoObject * object, MonoString * name, bool value)
{
	current->SetBool(object, name, value);
}

void ImportScript::SetFloat(MonoObject * object, MonoString * name, float value)
{
	current->SetFloat(object, name, value);
}


MonoObject* ImportScript::GetMaterialByName(MonoObject * object, MonoString * name)
{
	return current->GetMaterialByName(name);
}

int ImportScript::RangeInt(int min, int max)
{
	return App->random->Int(min, max);
}

float ImportScript::RangeFloat(float min, float max)
{
	return App->random->Float(min, max);;
}

int ImportScript::RangeIntNoRepeat(int min, int max, int norepeat)
{
	if (norepeat >= min && norepeat <= max)
	{
		int num = App->random->Int(min, max);
		if (num != norepeat)
		{
			return num;
		}
		else
		{
			while (num == norepeat)
			{
				num = App->random->Int(min, max);
			}
			return num;
		}
	}
	return 0;
}

void ImportScript::SetFullScreen(MonoObject * object)
{
	App->SetFullScreen();
}

void ImportScript::SetWindowed(MonoObject * object)
{
	App->SetWindowed();
}

void ImportScript::SetBordeless(MonoObject * object)
{
	App->SetBordeless();
}

void ImportScript::SetVSync(MonoObject * object)
{
	App->vsync = !(App->vsync);
}

void ImportScript::ShowFPS(MonoObject * object)
{
	App->active_fps = !(App->active_fps);
}

void ImportScript::SwapControllerVibration(MonoObject * object)
{
	App->input->SwapVibration();
}