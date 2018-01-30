#include "ImportScript.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ModuleImporter.h"
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "ResourceScript.h"
#include "ModuleResourceManager.h"
#include "JSONSerialization.h"
#include "CSharpScript.h"
#include "Globals.h"
#include "Timer.h"

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
		return true;
	}

	return false;
}

void ImportScript::ShutdownMono()
{
	Unload_domain();
	mono_jit_cleanup(mono_domain_get());
}

bool ImportScript::Import(const char* file, uint uuid)
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
				CSharpScript* script = new CSharpScript();
				res_script->SetCSharp(script);
				return false;
			}
			else
			{
				LOG("Script: %s, Compiled without errors", App->fs->GetOnlyName(fileassets).c_str());
				res_script->InitInfo(path_dll, fileassets);
				res_script->SetState(Resource::State::LOADED);
				//now 
				CSharpScript* newCSharp = LoadScript_CSharp(path_dll);
				res_script->SetCSharp(newCSharp);
			}


			// Then Create Meta
			std::string Newdirectory = ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory();
			Newdirectory += "\\" + App->fs->FixName_directory(file);
			App->Json_seria->SaveScript(res_script, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());

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
			CSharpScript* newCSharp = LoadScript_CSharp(path_dll);
			resourceScript->SetCSharp(newCSharp);
		}


		// Then Create Meta
		std::string Newdirectory = ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory();
		Newdirectory += "\\" + App->fs->FixName_directory(file);
		App->Json_seria->SaveScript(resourceScript, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
	}
	return true;
}

bool ImportScript::ReImportScript(std::string fileAssets, std::string uid_script, ResourceScript* resourceScript)
{
	// UnloadDomain
	Unload_domain();

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
	//std::string Newdirectory = ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory();
	//Newdirectory += "\\" + App->fs->FixName_directory(fileAssets);
	//App->Json_seria->SaveScript(resourceScript, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
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
	ImGui::Begin("Create New Script", &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders);
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
						CSharpScript* newCSharp = LoadScript_CSharp(path_dll);
						res_script->SetCSharp(newCSharp);
					}


					// Then Create Meta
					std::string Newdirectory = ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory();
					Newdirectory += "/" + App->fs->GetOnlyName(fileassets) + ".cs";
					App->Json_seria->SaveScript(res_script, ((Project*)App->gui->winManager[WindowName::PROJECT])->GetDirectory(), Newdirectory.c_str());
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

	// Get mono directory -------------
	std::string command = GetMonoPath();

	// Save dll to Library Directory --------------------
	libraryScript = App->fs->GetFullPath("Library/Scripts/");
	std::string nameFile = uid;
	nameFile += ".dll";
	libraryScript += nameFile;

	// Compile the script -----------------------------
	command += "/monobin/mcs -target:library -out:" + libraryScript + " ";
	std::string CulverinEditorpath = App->fs->GetFullPath("ScriptManager/AssemblyReference/CulverinEditor.dll");
	command += "-r:" + CulverinEditorpath + " ";
	command += "-lib:" + CulverinEditorpath + " ";
	command += script_path;
	//ShowWindow(FindWindowA("ConsoleWindowClass", NULL), false); -> Hide console (good or bad?)
	LOG("%s", command.c_str());
	return system(command.c_str());
	//system("pause");
}

CSharpScript* ImportScript::LoadScript_CSharp(std::string file)
{
	if (file != "")
	{
		MonoAssembly* assembly = mono_domain_assembly_open(GetDomain(), file.c_str());
		if (assembly)
		{
			// First Get the Image
			MonoImage* image = mono_assembly_get_image(assembly);
			if (image)
			{
				CSharpScript* csharp = CreateCSharp(image);
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

CSharpScript* ImportScript::CreateCSharp(MonoImage* image)
{
	if (image)
	{
		std::string classname, name_space;
		MonoClass* entity = GetMonoClassFromImage(image, name_space, classname);
		if (entity)
		{
			CSharpScript* csharp = new CSharpScript();
			csharp->SetDomain(GetMainDomain());
			csharp->SetImage(image);
			csharp->SetClass(entity);
			csharp->SetClassName(classname);
			csharp->SetNameSpace(name_space);
			return csharp;
		}
		else
		{
			LOG("[error]Failed loading class %s\n", classname.c_str());
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
	for (int i = 0; i < rows; i++)
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
	return entity;
}

//This method is called once Mono domain is init to Link C# functions 
//that users will use in their scripts with C++ functions of the application
void ImportScript::LinkFunctions()
{
	//GAMEOBJECT FUNCTIONS ---------------
	mono_add_internal_call("CulverinEditor.GameObject::CreateGameObject",(const void*)CreateGameObject);
	mono_add_internal_call("CulverinEditor.GameObject::GetOwnGameObject", (const void*)GetOwnGameObject);
	mono_add_internal_call("CulverinEditor.GameObject::Destroy",(const void*)DeleteGameObject);
	mono_add_internal_call("CulverinEditor.GameObject::SetActive",(const void*)SetGOActive);
	mono_add_internal_call("CulverinEditor.GameObject::IsActive",(const void*)IsGOActive);
	//mono_add_internal_call("CulverinEditor.GameObject::SetParent",(const void*)SetParent);
	mono_add_internal_call("CulverinEditor.GameObject::SetName",(const void*)SetName);
	mono_add_internal_call("CulverinEditor.GameObject::GetName",(const void*)GetName);
	//mono_add_internal_call("CulverinEditor.GameObject::AddComponent",(const void*)AddComponent);
	mono_add_internal_call("CulverinEditor.GameObject::GetComponent",(const void*)GetComponent);
	mono_add_internal_call("CulverinEditor.Transform::GetPosition", (const void*)GetPosition);
	mono_add_internal_call("CulverinEditor.Transform::SetPosition", (const void*)SetPosition);
	mono_add_internal_call("CulverinEditor.Transform::SetRotation", (const void*)SetRotation);
	mono_add_internal_call("CulverinEditor.Transform::GetRotation", (const void*)GetRotation);
	mono_add_internal_call("CulverinEditor.Transform::RotateAroundAxis", (const void*)IncrementRotation);

	//CONSOLE FUNCTIONS ------------------
	mono_add_internal_call("CulverinEditor.Debug.Debug::Log", (const void*)ConsoleLog);
	//INPUT FUNCTIONS -------------------
	mono_add_internal_call("CulverinEditor.Input::KeyDown", (const void*)KeyDown);
	mono_add_internal_call("CulverinEditor.Input::KeyUp", (const void*)KeyUp);
	mono_add_internal_call("CulverinEditor.Input::KeyRepeat", (const void*)KeyRepeat);
	mono_add_internal_call("CulverinEditor.Input::MouseButtonDown", (const void*)MouseButtonDown);
	mono_add_internal_call("CulverinEditor.Input::MouseButtonUp", (const void*)MouseButtonUp);
	mono_add_internal_call("CulverinEditor.Input::MouseButtonRepeat", (const void*)MouseButtonRepeat);
	mono_add_internal_call("CulverinEditor.Input::GetMousePosition", (const void*)GetMousePosition);
	mono_add_internal_call("CulverinEditor.Input::GetMouseXAxis", (const void*)GetMouseXAxis);
	mono_add_internal_call("CulverinEditor.Input::GetMouseYAxis", (const void*)GetMouseYAxis);

	//TIME FUNCTIONS -------------------
	mono_add_internal_call("CulverinEditor.Time::DeltaTime", (const void*)GetDeltaTime);
}

//Log messages into Engine Console
void ImportScript::ConsoleLog(MonoString* string)
{
	if (string != nullptr)
	{
		//Pass from MonoString to const char*
		const char* message = mono_string_to_utf8(string);
		LOG(message);
	}
}

mono_bool ImportScript::KeyDown(MonoString* string)
{
	const char* key_ = mono_string_to_utf8(string);
	SDL_Scancode key = App->input->GetKeyFromName(key_);
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

mono_bool ImportScript::KeyUp(MonoString* string)
{
	const char* key_ = mono_string_to_utf8(string);
	SDL_Scancode key = App->input->GetKeyFromName(key_);
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

mono_bool ImportScript::KeyRepeat(MonoString* string)
{
	const char* key_ = mono_string_to_utf8(string);
	SDL_Scancode key = App->input->GetKeyFromName(key_);
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

mono_bool ImportScript::MouseButtonDown(int buttonmouse)
{
	if (buttonmouse >= 0 && buttonmouse < 4)
	{
		return (App->input->GetMouseButton(buttonmouse) == KEY_STATE::KEY_DOWN);
	}
	return false;
}

mono_bool ImportScript::MouseButtonUp(int buttonmouse)
{
	if (buttonmouse >= 0 && buttonmouse < 4)
	{
		return (App->input->GetMouseButton(buttonmouse) == KEY_STATE::KEY_UP);
	}
	return false;
}

mono_bool ImportScript::MouseButtonRepeat(int buttonmouse)
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

float ImportScript::GetDeltaTime()
{
	return App->gameTime.timeScale * App->realTime.dt;
}

mono_bool ImportScript::IsGOActive(MonoObject* object)
{
	return current->IsGOActive(object);
}

void ImportScript::SetGOActive(MonoObject* object, mono_bool active)
{
	current->SetGOActive(object, active);
}

MonoObject* ImportScript::GetOwnGameObject()
{
	return current->GetOwnGameObject();
}

void ImportScript::SetName(MonoObject* object, MonoString * name)
{
	current->SetGOName(object, name);
}

MonoString* ImportScript::GetName(MonoObject* object)
{
	return current->GetName(object);
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

MonoObject* ImportScript::GetPosition(MonoObject* object)
{
	return current->GetPosition(object);
}

void ImportScript::SetPosition(MonoObject* object, MonoObject* vector3)
{
	current->SetPosition(object, vector3);
}

MonoObject* ImportScript::GetRotation(MonoObject* object)
{
	return current->GetRotation(object);
}

void ImportScript::SetRotation(MonoObject* object, MonoObject* vector3)
{
	current->SetRotation(object, vector3);
}

void ImportScript::IncrementRotation(MonoObject* object, MonoObject* vector3)
{
	current->IncrementRotation(object, vector3);
}