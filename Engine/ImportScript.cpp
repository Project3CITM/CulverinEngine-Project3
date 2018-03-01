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
#include <direct.h>
#pragma comment(lib, "mono-2.0-sgen.lib")

CSharpScript* ImportScript::current = nullptr;

ImportScript::ImportScript()
{
}


ImportScript::~ImportScript()
{
	//ShutdownMono();
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
	Unload_domain();
	mono_jit_cleanup(mono_domain_get());
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

bool ImportScript::ReImportScript(std::string fileAssets, std::string uid_script, ResourceScript* resourceScript)
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
	if (App->resource_manager->ReImportAllScripts() == false)
	{
		LOG("[error] Error With ReImport Script");
	}
	//CSharpScript* newCSharp = LoadScript_CSharp(path_dll);
	//resourceScript->SetCSharp(newCSharp);

	// Then Create Meta
	std::string Newdirectory = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
	Newdirectory += "/" + App->fs->FixName_directory(fileAssets);
	App->json_seria->SaveScript(resourceScript, ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
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
		MonoClass* c = mono_class_from_name(GetCulverinImage(), "CulverinEditor", "GameObject");
		if (c)
		{
			MonoObject* new_object = mono_object_new(GetMainDomain(), c);
			if (new_object)
			{
				//CSSelfObject = new_object;
				mono_map[new_object] = gameobject;
			}
		}
	}
}

void ImportScript::ClearMonoMap()
{
	mono_map.clear();
}

void ImportScript::UpdateMonoMap(GameObject* modificate)
{
	if (modificate->IsDeleteFixed())
	{
		// Delete from map
		//MonoClassField* mono_field = nullptr;
		//mono_field_set_value(GetMonoObject(), mono_field, newVal);
		//MonoObject* object = mono_field_get_value_object(App->importer->iScript->GetDomain(), mono_field, script->GetMonoObject());
		//get_mono_object

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
				//CSSelfObject = new_object;
				mono_map[new_object] = modificate;
			}
		}
	}
}

void ImportScript::UpdateMonoMap(GameObject* modificate, MonoObject* object)
{
	mono_map[object] = modificate;
}

MonoObject* ImportScript::GetMonoObject(GameObject* gameobject)
{
	if (gameobject != nullptr/* && gameobject->IsDeleteFixed()*/)
	{
		std::map<MonoObject*, GameObject*>::iterator it = mono_map.begin();
		while (it != mono_map.end())
		{
			if (it->second == gameobject)
			{
				return it->first;
			}
			it++;
		}
	}
}

GameObject* ImportScript::GetGameObject(MonoObject* monoobject)
{
	if (monoobject != nullptr)
	{
		std::map<MonoObject*, GameObject*>::iterator it = mono_map.find(monoobject);
		if (it != mono_map.end())
			return it->second;
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
		MonoAssembly* assembly = mono_domain_assembly_open(GetDomain(), file.c_str());
		if (assembly)
		{
			// First Get the Image
			//MonoImage* image = mono_assembly_get_image(assembly);
			if (App->importer->iScript->GetCulverinImage())
			{
				CSharpScript* csharp = CreateCSharp(App->importer->iScript->GetCulverinImage(), name);
				if (csharp != nullptr)
				{
					csharp->SetAssembly(assembly);
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
			csharp->SetClassName(classname);
			csharp->SetNameSpace(name_space);
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
	//GAMEOBJECT FUNCTIONS ---------------
	mono_add_internal_call("CulverinEditor.GameObject::GetTag", (const void*)GetTag);
	mono_add_internal_call("CulverinEditor.GameObject::SetTag", (const void*)SetTag);
	mono_add_internal_call("CulverinEditor.GameObject::CompareTag", (const void*)CompareTag);
	mono_add_internal_call("CulverinEditor.GameObject::FindGameObjectWithTag", (const void*)FindGameObjectWithTag);
	mono_add_internal_call("CulverinEditor.GameObject::CreateGameObject",(const void*)CreateGameObject);
	mono_add_internal_call("CulverinEditor.GameObject::GetOwnGameObject", (const void*)GetOwnGameObject);
	mono_add_internal_call("CulverinEditor.GameObject::ChildCount", (const void*)ChildCount);
	mono_add_internal_call("CulverinEditor.GameObject::GetChildByIndex", (const void*)GetChildByIndex);
	mono_add_internal_call("CulverinEditor.GameObject::GetChildByName", (const void*)GetChildByName);
	mono_add_internal_call("CulverinEditor.GameObject::Destroy",(const void*)DeleteGameObject);
	mono_add_internal_call("CulverinEditor.GameObject::SetActive",(const void*)SetActive);
	mono_add_internal_call("CulverinEditor.GameObject::IsActive",(const void*)IsActive);
	mono_add_internal_call("CulverinEditor.GameObject::IsStatic", (const void*)IsStatic);
	mono_add_internal_call("CulverinEditor.GameObject::Find", (const void*)Find);
	//mono_add_internal_call("CulverinEditor.GameObject::SetParent",(const void*)SetParent);
	mono_add_internal_call("CulverinEditor.GameObject::SetName",(const void*)SetName);
	mono_add_internal_call("CulverinEditor.GameObject::GetName",(const void*)GetName);
	//mono_add_internal_call("CulverinEditor.GameObject::AddComponent",(const void*)AddComponent);
	mono_add_internal_call("CulverinEditor.GameObject::GetComponent",(const void*)GetComponent);
	
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

	// Component ---------------------------
	mono_add_internal_call("CulverinEditor.Component::GetParentGameObject", (const void*)GetParentGameObject);
	mono_add_internal_call("CulverinEditor.Component::SetEnabled", (const void*)SetEnabled);
	mono_add_internal_call("CulverinEditor.Component::GetEnabled", (const void*)GetEnabled);
	mono_add_internal_call("CulverinEditor.Movement_Action::SetEnabled", (const void*)SetEnabled);
	mono_add_internal_call("CulverinEditor.Movement_Action::GetEnabled", (const void*)GetEnabled);
	mono_add_internal_call("CulverinEditor.Seek_Steering::SetEnabled", (const void*)SetEnabled);
	mono_add_internal_call("CulverinEditor.Seek_Steering::GetEnabled", (const void*)GetEnabled);
	mono_add_internal_call("CulverinEditor.Arrive_Steering::SetEnabled", (const void*)SetEnabled);
	mono_add_internal_call("CulverinEditor.Arrive_Steering::GetEnabled", (const void*)GetEnabled);

	//CONSOLE FUNCTIONS ------------------
	mono_add_internal_call("CulverinEditor.Debug.Debug::Log", (const void*)ConsoleLog);
	//INPUT FUNCTIONS -------------------
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

	//TIME FUNCTIONS -------------------
	mono_add_internal_call("CulverinEditor.Time::DeltaTime", (const void*)GetDeltaTime);

	//MAP FUNCTIONS ----------------------
	mono_add_internal_call("CulverinEditor.Map.Map::GetMapString", (const void*)GetMapString);
	mono_add_internal_call("CulverinEditor.Map.Map::GetHeightMap", (const void*)GetHeightMap);
	mono_add_internal_call("CulverinEditor.Map.Map::GetWidthMap", (const void*)GetWidthMap);

	//AUDIO FUNCTIONS --------------------
	mono_add_internal_call("CulverinEditor.Audio::StopAllSounds", (const void*)StopAllSounds);
	mono_add_internal_call("CulverinEditor.Audio::PauseAllSounds", (const void*)PauseAllSounds);
	mono_add_internal_call("CulverinEditor.Audio::ResumeAllSounds", (const void*)ResumeAllSounds);
	mono_add_internal_call("CulverinEditor.Audio::SetAudioVariableValue", (const void*)ChangeRTPC);
	mono_add_internal_call("CulverinEditor.Audio::ChangeState", (const void*)ChangeState);
	mono_add_internal_call("CulverinEditor.Audio::ChangeVolume", (const void*)ChangeVolume);
	mono_add_internal_call("CulverinEditor.Audio::Mute", (const void*)Mute);
	
	//COMPONENT AUDIO FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompAudio::PlayEvent", (const void*)PlayAudioEvent);
	mono_add_internal_call("CulverinEditor.CompAudio::StopEvent", (const void*)StopAudioEvent);
	mono_add_internal_call("CulverinEditor.CompAudio::SetAuxiliarySends", (const void*)SetAuxiliarySends);

	//COMPONENT UI_INTERACTIVE FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompInteractive::Activate", (const void*)Activate);
	mono_add_internal_call("CulverinEditor.CompInteractive::Deactivate", (const void*)Deactivate);
	
	//COMPONENT UI_INTERACTIVE FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompButton::Activate", (const void*)Activate);
	mono_add_internal_call("CulverinEditor.CompButton::Deactivate", (const void*)Deactivate);
	mono_add_internal_call("CulverinEditor.CompButton::Clicked", (const void*)Clicked);
	
	//COMPONENT UI_GRAPHIC FUNCTIONS -----------------
	mono_add_internal_call("CulverinEditor.CompGraphic::SetRaycastTarget", (const void*)SetRaycastTarget);
	mono_add_internal_call("CulverinEditor.CompImage::FillAmount", (const void*)FillAmount);

	//COMPONENT COLLIDER FUNCTIONS -----------------------
	mono_add_internal_call("CulverinEditor.CompCollider::GetCollidedObject", (const void*)GetCollidedObject);
	mono_add_internal_call("CulverinEditor.CompCollider::MoveKinematic", (const void*)MoveStaticColliderTo);


	//COMPONENT RIGID BODY FUNCTIONS -----------------------
	mono_add_internal_call("CulverinEditor.CompRigidBody::GetColliderPosition", (const void*)GetColliderPosition);
	mono_add_internal_call("CulverinEditor.CompRigidBody::GetColliderQuaternion", (const void*)GetColliderQuaternion);
	mono_add_internal_call("CulverinEditor.CompRigidBody::MoveKinematic", (const void*)MoveKinematic);


	//COMPONENT ANIMATION FUNCTIONS -------------------------
	mono_add_internal_call("CulverinEditor.CompAnimation::PlayAnimation", (const void*)PlayAnimation);
	mono_add_internal_call("CulverinEditor.CompAnimation::SetTransition", (const void*)SetTransition);
	mono_add_internal_call("CulverinEditor.CompAnimation::IsAnimationStopped", (const void*)IsAnimationStopped);
	mono_add_internal_call("CulverinEditor.CompAnimation::IsAnimationRunning", (const void*)IsAnimationRunning);
	mono_add_internal_call("CulverinEditor.CompAnimation::IsAnimOverXTime", (const void*)IsAnimOverXTime);
}

//Log messages into Engine Console
void ImportScript::ConsoleLog(MonoObject* string)
{
	if (string != nullptr)
	{
		//Pass from MonoString to const char*
		MonoString* strings = mono_object_to_string(string, NULL);
		const char* message = mono_string_to_utf8(strings);
		LOG(message);
	}
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

float ImportScript::GetDeltaTime()
{
	return App->game_time.time_scale * App->real_time.dt;
}

MonoObject* ImportScript::GetLinkedObject(MonoObject* object, MonoString* name)
{
	std::string name_variable = mono_string_to_utf8(name);
	if (name_variable == "lweapon_obj")
	{
		int c = 0;
	}
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

int ImportScript::ChildCount(MonoObject * object)
{
	return current->ChildCount(object);
}

MonoObject * ImportScript::GetChildByIndex(MonoObject * object, int index)
{
	return current->GetChildByIndex(object, index);
}

MonoObject * ImportScript::GetChildByName(MonoObject * object, MonoString * name)
{
	return current->GetChildByName(object, name);
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

mono_bool ImportScript::CompareTag(MonoObject * object, MonoString * tag)
{
	return current->CompareTag(object, tag);
}

MonoObject * ImportScript::FindGameObjectWithTag(MonoObject * object, MonoString * tag)
{
	return current->FindGameObjectWithTag(object, tag);
}

void ImportScript::CreateGameObject(MonoObject* object)
{
	current->CreateGameObject(object);
}

void ImportScript::DeleteGameObject(MonoObject* object)
{
	current->DestroyGameObject(object);
}

MonoObject* ImportScript::GetComponent(MonoObject* object, MonoReflectionType* type)
{
	return current->GetComponent(object, type);
}

MonoObject * ImportScript::GetForwardVector(MonoObject * object)
{
	return current->GetForwardVector(object);
}

MonoObject * ImportScript::GetBackwardVector(MonoObject * object)
{
	return current->GetBackwardVector(object);
}

MonoObject * ImportScript::GetUpVector(MonoObject * object)
{
	return current->GetUpVector(object);
}

MonoObject * ImportScript::GetDownVector(MonoObject * object)
{
	return current->GetDownVector(object);
}

MonoObject * ImportScript::GetRightVector(MonoObject * object)
{
	return current->GetRightVector(object);
}

MonoObject * ImportScript::GetLeftVector(MonoObject * object)
{
	return current->GetLeftVector(object);
}

void ImportScript::SetForwardVector(MonoObject * object, MonoObject * vector3)
{
	current->SetForwardVector(object, vector3);
}

void ImportScript::SetBackwardVector(MonoObject * object, MonoObject * vector3)
{
	current->SetBackwardVector(object, vector3);
}

void ImportScript::SetUpVector(MonoObject * object, MonoObject * vector3)
{
	current->SetUpVector(object, vector3);
}

void ImportScript::SetDownVector(MonoObject * object, MonoObject * vector3)
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

// Component ---------------------
MonoObject* ImportScript::GetParentGameObject()
{
	return current->GetParentGameObject();
}

bool ImportScript::GetEnabled(MonoObject * object)
{
	return current->GetEnabled(object);
}

void ImportScript::SetEnabled(MonoObject * object, bool val)
{
	current->SetEnabled(object, val);
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
}

void ImportScript::ChangeVolume(float volume)
{
	App->audio->ChangeVolume(volume);
}

void ImportScript::Mute(bool m)
{
	App->audio->Mute(m);
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

void ImportScript::Clicked(MonoObject * object)
{
	current->Clicked(object);
}

void ImportScript::SetRaycastTarget(MonoObject * object, mono_bool flag)
{
	current->SetRaycastTarget(object, flag);
}

void ImportScript::FillAmount(MonoObject * object, float value)
{
	current->FillAmount(object, value);
}

MonoObject * ImportScript::GetCollidedObject(MonoObject* object)
{
	return current->GetCollidedObject(object);
}

void ImportScript::MoveStaticColliderTo(MonoObject * object, MonoObject * position)
{
	current->MoveStaticColliderTo(object, position);
}

MonoObject* ImportScript::GetColliderPosition(MonoObject* object)
{
	return current->GetColliderPosition(object);
}

MonoObject* ImportScript::GetColliderQuaternion(MonoObject* object)
{
	return current->GetColliderQuaternion(object);
}

void ImportScript::MoveKinematic(MonoObject* object, MonoObject* position, MonoObject* rotation)
{
	current->MoveKinematic(object, position, rotation);
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
