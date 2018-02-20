#include "CSharpScript.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ModuleInput.h"
#include "ModuleImporter.h"
#include "ImportScript.h"
#include "ModuleMap.h"
#include "CompTransform.h"
#include "CompScript.h"
#include "CompInteractive.h"
#include "ResourceScript.h"
#include "ModuleResourceManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompAudio.h"
#include "CompButton.h"
#include "CompCollider.h"

//SCRIPT VARIABLE UTILITY METHODS ------
ScriptVariable::ScriptVariable(const char* name, VarType type, VarAccess access, CSharpScript* script) : name(name), type(type), access(access), script(script)
{
}

ScriptVariable::~ScriptVariable()
{
}

void ScriptVariable::SetMonoValue(void* newVal)
{
	if (newVal != nullptr)
	{
		mono_field_set_value(script->GetMonoObject(), mono_field, newVal);
		if (type == VarType::Var_GAMEOBJECT)
		{
			MonoObject* object = mono_field_get_value_object(App->importer->iScript->GetDomain(), mono_field, script->GetMonoObject());
			script->game_objects[object] = game_object;
		}
	}
	else
	{
		LOG("[error] new value to set was nullptr");
	}
}

void ScriptVariable::EreaseMonoValue(void* newVal)
{
	if (newVal != nullptr)
	{
		MonoObject* object = mono_field_get_value_object(App->importer->iScript->GetDomain(), mono_field, script->GetMonoObject());
		if (object)
		{
			script->game_objects.erase(script->game_objects.find(object));
			game_object = nullptr;
		}
	}
}

void ScriptVariable::SetMonoField(MonoClassField* mfield)
{
	if (mfield != nullptr)
	{
		mono_field = mfield;
	}
	else
	{
		LOG("[error] MonoClassField* pointer was nullptr");
	}
}

void ScriptVariable::SetMonoType(MonoType* mtype)
{
	if (mtype != nullptr)
	{
		mono_type = mtype;
	}
	else
	{
		LOG("[error] MonoType* pointer was nullptr");
	}
}

void ScriptVariable::Serialize(JSON_Object * object, const std::string& title)
{
	const char* type_name = nullptr;

	//SET VAR NAME --------------------
	json_object_dotset_string_with_std(object, title + "Name: ", name);
	
	//SAVE VAR TYPE -------------------
	switch (type)
	{
	case Var_UNKNOWN:
		type_name = "unknown";
		json_object_dotset_string_with_std(object, title + "Type: ", type_name);
		json_object_dotset_string_with_std(object, title + "Value: ", type_name);
		break;

	case Var_INT:
		type_name = "int";
		json_object_dotset_string_with_std(object, title + "Type: ", type_name);
		json_object_dotset_number_with_std(object, title + "Value: ", *(int*)value);
		break;

	case Var_FLOAT:
		type_name = "float";
		json_object_dotset_string_with_std(object, title + "Type: ", type_name);
		json_object_dotset_number_with_std(object, title + "Value: ", *(float*)value);
		break;

	case Var_BOOL:
		type_name = "bool";
		json_object_dotset_string_with_std(object, title + "Type: ", type_name);
		json_object_dotset_boolean_with_std(object, title + "Value: ", *(bool*)value);
		break;

	case Var_STRING:
		type_name = "string";
		json_object_dotset_string_with_std(object, title + "Type: ", type_name);
		json_object_dotset_string_with_std(object, title + "Value: ", str_value.c_str());
		break;

	case Var_CLASS:
		type_name = "class";
		json_object_dotset_string_with_std(object, title + "Type: ", type_name);
		break;

	case Var_GAMEOBJECT:
		type_name = "GameObject";
		json_object_dotset_string_with_std(object, title + "Type: ", type_name);
		if (game_object != nullptr)
		{
			json_object_dotset_number_with_std(object, title + "GameObject UUID: ", game_object->GetUUID());
		}
		else
		{
			//-1 Any GameObject is assigned
			json_object_dotset_number_with_std(object, title + "GameObject UUID: ", -1);
		}

		break;

	default:
		break;
	}

	//SAVE VAR VALUE ----------------------


}

//CSHARP SCRIPT FUNCTIONS ---------------
CSharpScript::CSharpScript()
{
}


CSharpScript::~CSharpScript()
{
	temp.clear();
}


void CSharpScript::LoadScript()
{
	if (CSClass)
	{
		CSObject = mono_object_new(App->importer->iScript->GetDomain(), CSClass);
	}
	if (CSObject)
	{
		mono_runtime_object_init(CSObject);

		//Create main Functions
		Start = CreateMainFunction("Start", DefaultParam, FunctionBase::CS_Start);
		Update = CreateMainFunction("Update", DefaultParam, FunctionBase::CS_Update);
		FixedUpdate = CreateMainFunction("FixedUpdate", DefaultParam, FunctionBase::CS_Update);
		OnGUI = CreateMainFunction("OnGUI", DefaultParam, FunctionBase::CS_OnGUI);
		OnEnable = CreateMainFunction("OnEnable", DefaultParam, FunctionBase::CS_OnEnable);
		OnDisable = CreateMainFunction("OnDisable", DefaultParam, FunctionBase::CS_OnDisable);
		OnTriggerEnter = CreateMainFunction("OnTriggerEnter", DefaultParam, FunctionBase::CS_OnTriggerEnter);
		OnTriggerLost = CreateMainFunction("OnTriggerLost", DefaultParam, FunctionBase::CS_OnTriggerLost);
		OnClick = CreateMainFunction("OnClick", DefaultParam, FunctionBase::CS_OnClick);

		//Get Script Variables info (from c# to c++)
		GetScriptVariables();
	}
}

MainMonoMethod CSharpScript::CreateMainFunction(std::string function, int parameters, FunctionBase type)
{
	MainMonoMethod Newmethod;
	if (CSClass != nullptr)
	{
		Newmethod.method = mono_class_get_method_from_name(CSClass, function.c_str(), parameters);
		Newmethod.type = type;
	}
	return Newmethod;
}

//Depending on the function passed, script will perform its actions
void CSharpScript::DoMainFunction(FunctionBase function, void** parameters)
{
	switch (function)
	{
	case FunctionBase::CS_Start:
	{
		if (Start.method != nullptr)
		{
			DoFunction(Start.method, nullptr);
			UpdateScriptVariables();
		}
		break;
	}
	case FunctionBase::CS_OnTriggerEnter:
	{
		if (OnTriggerEnter.method != nullptr)
		{
			DoFunction(OnTriggerEnter.method, parameters);
		}
		break;
	}
	case FunctionBase::CS_OnTriggerLost:
	{
		if (OnTriggerLost.method != nullptr)
		{
			DoFunction(OnTriggerLost.method, parameters);
		}
		break;
	}
	case FunctionBase::CS_Update:
	{
		if (Update.method != nullptr)
		{
			DoFunction(Update.method, nullptr);
			UpdateScriptVariables();
		}
		break;
	}
	case FunctionBase::CS_FixedUpdate:
	{
		if (FixedUpdate.method != nullptr)
		{
			DoFunction(FixedUpdate.method, nullptr);
			//UpdateScriptVariables();
		}
		break;
	}
	case FunctionBase::CS_OnGUI:
	{
		if (OnGUI.method != nullptr)
		{
			DoFunction(OnGUI.method, nullptr);
		}
		break;
	}
	case FunctionBase::CS_OnClick:
	{
		if (OnClick.method != nullptr)
		{
			DoFunction(OnClick.method, nullptr);
		}
		break;
	}
	case FunctionBase::CS_OnEnable:
	{
		if (OnEnable.method != nullptr)
		{
			DoFunction(OnEnable.method, nullptr);
		}
		break;
	}
	case FunctionBase::CS_OnDisable:
	{
		if (OnDisable.method != nullptr)
		{
			DoFunction(OnDisable.method, nullptr);
		}
		break;
	}
	}
}

void CSharpScript::DoFunction(MonoMethod* function, void ** parameter)
{
	MonoObject* exception = nullptr;

	// Do Main Function
	mono_runtime_invoke(function, CSObject, parameter, &exception);
	if (exception)
	{
		mono_print_unhandled_exception(exception);
	}
}

bool CSharpScript::CheckMonoObject(MonoObject* object)
{
	if (object != nullptr)
	{
		// Link MonoObject with GameObject to enable script control it
		current_game_object = game_objects[object];
		return true;
	}
	return false;
}

MonoObject* CSharpScript::GetMonoObject() const
{
	return CSObject;
}

MonoClass* CSharpScript::GetMonoClass() const
{
	return CSClass;
}

void CSharpScript::SetDomain(MonoDomain* domain)
{
	CSdomain = domain;
}

void CSharpScript::SetAssembly(MonoAssembly* assembly)
{
	CSassembly = assembly;
}

void CSharpScript::SetImage(MonoImage* image)
{
	CSimage = image;
}

void CSharpScript::SetClass(MonoClass* klass)
{
	CSClass = klass;
}

void CSharpScript::SetClassName(std::string _name)
{
	name = _name;
}

void CSharpScript::SetNameSpace(std::string _name_space)
{
	name_space = _name_space;
}

bool CSharpScript::ReImport(std::string pathdll)
{
	MonoAssembly* assembly_ = mono_domain_assembly_open(App->importer->iScript->GetDomain(), pathdll.c_str());
	if (assembly_)
	{
		MonoImage* image_ = mono_assembly_get_image(assembly_);
		if (image_)
		{
			std::string classname_, name_space_;
			MonoClass* entity_ = App->importer->iScript->GetMonoClassFromImage(image_, name_space_, classname_);
			if (entity_)
			{
				SetImage(image_);
				SetClass(entity_);
				SetClassName(classname_);
				SetNameSpace(name_space_);

				//Set script info and functionality
				LoadScript();

				SetAssembly(assembly_);
				SetDomain(App->importer->iScript->GetDomain());
			}
			else
			{
				LOG("[error]Failed loading class %s\n", classname_.c_str());
				return false;
			}
		}
		else
		{
			LOG("[error] Error with Image");
			return false;
		}
	}
	else
	{
		LOG("[error] Error with Assembly");
		return false;
	}
	return true;
}

void CSharpScript::Clear()
{
	for (uint i = 0; i < variables.size(); i++)
	{
		if (variables[i]->game_object != nullptr)
		{
			variables[i]->game_object->FixedDelete(true);
			variables[i]->select_game_object = false;
		}
	}
}

//Release memory allocated from old variables
void CSharpScript::ResetScriptVariables()
{
	for (uint i = 0; i < variables.size(); i++)
	{
		variables[i]->value = nullptr;
		variables[i]->game_object = nullptr;
		RELEASE(variables[i]);
	}

	variables.clear();
	field_type.clear();
}

void CSharpScript::SetOwnGameObject(GameObject* gameobject)
{
	own_game_object = gameobject;
	CreateOwnGameObject();
}

void CSharpScript::CreateOwnGameObject()
{
	MonoClass* c = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "GameObject");
	if (c)
	{
		MonoObject* new_object = mono_object_new(CSdomain, c);
		if (new_object)
		{
			CSSelfObject = new_object;
			game_objects[CSSelfObject] = own_game_object;
		}
	}
}

void CSharpScript::GetScriptVariables()
{
	//Reset previour info
	ResetScriptVariables();

	static uint32_t field_attr_public = 0x0006;
	static uint32_t flags;

	MonoClassField* field = nullptr;
	MonoType* type = nullptr;
	void* iter = nullptr;

	int num_fields = mono_class_num_fields(CSClass);

	//Fill field-type map from the script to after get its respective info
	for (uint i = 0; i < num_fields; i++)
	{
		field = mono_class_get_fields(CSClass, &iter);
		type = mono_field_get_type(field);

		//Insert this info pair into the map
		field_type.insert(std::pair<MonoClassField*, MonoType*>(field, type));
	}

	// From the previous map, fill VariablesScript vector that will contain info (name, type, value) of each variable
	for (std::map<MonoClassField*, MonoType*>::iterator it = field_type.begin(); it != field_type.end(); ++it)
	{
		VarType type = GetTypeFromMono(it->second);
		VarAccess access = VarAccess::Var_PRIVATE;

		//Set info about accessibility of the variable -> DOESN'T WORK!!!
		flags = mono_field_get_flags(field);
		if ((flags & MONO_FIELD_ATTR_PUBLIC))
		{
			access = VarAccess::Var_PUBLIC;
		}
		else if ((flags & MONO_FIELD_ATTR_PRIVATE))
		{
			access = VarAccess::Var_PRIVATE;
		}

		//Create variable
		ScriptVariable* new_var = new ScriptVariable(mono_field_get_name(it->first), type, access, this);

		//Set its value
		GetValueFromMono(new_var, it->first, it->second);

		//Link to Mono properties
		LinkVarToMono(new_var, it->first, it->second);

		//Put it in variables vector
		variables.push_back(new_var);
	}
}

void CSharpScript::UpdateScriptVariables()
{
	uint count = 0;
	// From the map, update the value of each script
	for (std::map<MonoClassField*, MonoType*>::iterator it = field_type.begin(); it != field_type.end(); ++it)
	{
		//Set its value
		UpdateValueFromMono(variables[count++], it->first, it->second);
	}
}

void CSharpScript::RemoveReferences(GameObject* go)
{
	//Set to null all references of the gameobject to be deleted
	for (uint i = 0; i < variables.size(); i++)
	{
		if (variables[i]->type == VarType::Var_GAMEOBJECT && variables[i]->game_object == go)
		{
			variables[i]->game_object = nullptr;
			variables[i]->select_game_object = false;
		}
	}
}



VarType CSharpScript::GetTypeFromMono(MonoType* mtype)
{
	if (mtype != nullptr)
	{
		std::string name = mono_type_get_name(mtype);
		if (name == "System.Int32")
		{
			return VarType::Var_INT;
		}

		if (name == "System.Single")
		{
			return VarType::Var_FLOAT;
		}

		if (name == "System.Boolean")
		{
			return VarType::Var_BOOL;
		}

		if (name == "CulverinEditor.GameObject")
		{
			return VarType::Var_GAMEOBJECT;
		}

		if (name == "System.String")
		{
			return VarType::Var_STRING;
		}
		else
		{
			LOG("Unknown variable type");
			return VarType::Var_UNKNOWN;
		}
	}
	else
	{
		LOG("MonoType* mtype was nullptr");
		return VarType::Var_UNKNOWN;
	}
}

bool CSharpScript::GetValueFromMono(ScriptVariable* variable, MonoClassField* mfield, MonoType* mtype)
{
	if (variable != nullptr && mfield != nullptr && mtype != nullptr)
	{
		//Free memory
		if (variable->value != nullptr)
		{
			RELEASE(variable->value);
			variable->value = nullptr;
		}

		if (variable->type != VarType::Var_STRING && variable->type != VarType::Var_GAMEOBJECT)
		{
			//Allocate memory
			variable->value = new char[mono_type_stack_size(mtype, NULL)];

			//Set value of the variable by passing it as a reference in this function
			mono_field_get_value(CSObject, mfield, variable->value);
		}
		else if (variable->type == VarType::Var_STRING)
		{
			MonoString* str = nullptr;
			//Set value of the variable by passing it as a reference in this function
			mono_field_get_value(CSObject, mfield, &str);

			if (str != nullptr)
			{
				//Copy string into str_value (specific for strings)
				variable->str_value = mono_string_to_utf8(str);
			}
			else
			{
				variable->str_value = "";
			}
		}
		else if (variable->type == VarType::Var_GAMEOBJECT)
		{
			variable->game_object = nullptr;
			//Set value of the variable by passing it as a reference in this function
			//mono_field_get_value(CSObject, mfield, variable->gameObject);
			
		}
		return true;
	}
	else
	{
		LOG("[error] There is some null pointer.");
		return false;
	}
}

bool CSharpScript::UpdateValueFromMono(ScriptVariable * variable, MonoClassField * mfield, MonoType * mtype)
{
	if (variable != nullptr && mfield != nullptr && mtype != nullptr)
	{
		if (variable->type != VarType::Var_STRING && variable->type != VarType::Var_GAMEOBJECT)
		{
			//Set value of the variable by passing it as a reference in this function
			mono_field_get_value(CSObject, mfield, variable->value);
		}
		else if (variable->type == VarType::Var_GAMEOBJECT)
		{
			if (variable->game_object != nullptr)
			{
				//Set value of the variable by passing it as a reference in this function
				mono_field_get_value(CSObject, mfield, variable->game_object);
			}
		}
		else
		{
			MonoString* str = nullptr;

			//Set value of the variable by passing it as a reference in this function
			mono_field_get_value(CSObject, mfield, &str);

			//Copy string into str_value (specific for strings)
			variable->str_value = mono_string_to_utf8(str);
		}
		return true;
	}
	else
	{
		LOG("[error] There is some null pointer.");
		return false;
	}
}

bool CSharpScript::LinkVarToMono(ScriptVariable* variable, MonoClassField * mfield, MonoType * mtype)
{
	if (variable != nullptr && mfield != nullptr && mtype != nullptr)
	{
		variable->SetMonoField(mfield);
		variable->SetMonoType(mtype);

		return true;
	}
	else
	{
		LOG("[error] There is some null pointer.");
		return false;
	}
}

MonoObject* CSharpScript::GetMousePosition()
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				float mouse_x = App->input->GetMouseX();
				float mouse_y = App->input->GetMouseY();

				if (x_field) mono_field_set_value(new_object, x_field, &mouse_x);
				if (y_field) mono_field_set_value(new_object, y_field, &mouse_y);
				if (z_field) mono_field_set_value(new_object, z_field, 0);

				return new_object;
			}
		}
	}
	return nullptr;
}

bool CSharpScript::IsStatic(MonoObject * object)
{
	if (!CheckMonoObject(object))
	{
		return false;
	}

	if (current_game_object != nullptr)
	{
		return current_game_object->IsStatic();
	}
	else
	{
		LOG("[error] GameObject was null.");
	}
}

mono_bool CSharpScript::IsActive(MonoObject* object)
{
	if (!CheckMonoObject(object))
	{
		return false;
	}

	if (current_game_object != nullptr)
	{
		return current_game_object->IsActive();
	}
	else
	{ 
		LOG("[error] GameObject was null.");
	}
}

void CSharpScript::SetActive(MonoObject* object, mono_bool active)
{
	if (!CheckMonoObject(object))
	{
		LOG("[error] MonoObject invalid");
	}

	else
	{
		if (current_game_object != nullptr)
		{
			current_game_object->SetActive(active);
		}
		else
		{
			LOG("[error] GameObject was null.");
		}
	}
}

MonoObject* CSharpScript::GetOwnGameObject()
{
	if (CSSelfObject != nullptr && own_game_object != nullptr)
	{
		return CSSelfObject;
	}
}

void CSharpScript::SetCurrentGameObject(GameObject* current)
{
	current_game_object = current;
}

void CSharpScript::SetVarValue(ScriptVariable* variable, void* new_val)
{

}

void CSharpScript::CreateGameObject(MonoObject* object)
{
	GameObject* gameobject = App->scene->CreateGameObject();
	game_objects[object] = gameobject;
}

bool CSharpScript::DestroyGameObject(MonoObject* object)
{
	if (!CheckMonoObject(object))
	{
		return false;
	}

	if (current_game_object == nullptr)
	{
		return false;
	}

	else
	{
		App->scene->DeleteGameObject(current_game_object);
	}
}

void CSharpScript::SetName(MonoObject * object, MonoString * name)
{
	if (!CheckMonoObject(object))
	{
		LOG("[error] MonoObject invalid");
	}
	else
	{
		if (current_game_object != nullptr)
		{
			current_game_object->SetName(mono_string_to_utf8(name));
			std::string temp_name = mono_string_to_utf8(name);

			current_game_object->NameNotRepeat(temp_name, false, current_game_object->GetParent());
		}
	}
}

MonoString* CSharpScript::GetName(MonoObject* object)
{
	if (!CheckMonoObject(object))
	{
		return nullptr;
	}
	if (current_game_object == nullptr)
	{
		return nullptr;
	}
	return mono_string_new(CSdomain, current_game_object->GetName());
}

void CSharpScript::SetTag(MonoObject * object, MonoString * tag)
{
	if (!CheckMonoObject(object))
	{
		LOG("[error] MonoObject invalid");
	}
	else
	{
		if (current_game_object != nullptr)
		{
			current_game_object->SetTag(mono_string_to_utf8(tag));
		}
	}
}

MonoString* CSharpScript::GetTag(MonoObject* object)
{
	if (!CheckMonoObject(object))
	{
		return nullptr;
	}
	if (current_game_object == nullptr)
	{
		return nullptr;
	}
	return mono_string_new(CSdomain, current_game_object->GetTag());
}

bool CSharpScript::CompareTag(MonoObject * object, MonoString * tag)
{
	return current_game_object->CompareTag(mono_string_to_utf8(tag));
}

MonoObject* CSharpScript::FindGameObjectWithTag(MonoObject * object, MonoString * tag)
{
	std::map<MonoObject*, GameObject*>::iterator iter = game_objects.begin();
	GameObject* target = nullptr;
	target = App->scene->FindGameObjectWithTag(mono_string_to_utf8(tag));
	if (target == nullptr)return nullptr;

	while (iter != game_objects.end())
	{
		if (iter._Ptr->_Myval.second == target)return iter._Ptr->_Myval.first;

		iter++;
	}
	return nullptr;
}

int CSharpScript::ChildCount(MonoObject * object)
{
	return current_game_object->GetNumChilds();
}

MonoObject * CSharpScript::GetChildByIndex(MonoObject * object, int index)
{
	GameObject* target = current_game_object->GetChildbyIndex(index);
	if (target == nullptr)return nullptr;
	
	std::map<MonoObject*,GameObject*>::iterator iter = game_objects.begin();
	while (iter != game_objects.end())
	{
		if (iter._Ptr->_Myval.second == target)
		{
			return iter._Ptr->_Myval.first;
		}

		iter++;
	}

	return nullptr;
}

MonoObject * CSharpScript::GetChildByName(MonoObject * object, MonoString * name)
{
	GameObject* target = current_game_object->GetChildbyName(mono_string_to_utf8(name));
	if (target == nullptr)return nullptr;

	std::map<MonoObject*, GameObject*>::iterator iter = game_objects.begin();
	while (iter != game_objects.end())
	{
		if (iter._Ptr->_Myval.second == target)
		{
			return iter._Ptr->_Myval.first;
		}

		iter++;
	}

	return nullptr;
}

MonoObject* CSharpScript::GetComponent(MonoObject* object, MonoReflectionType* type)
{
	if (!CheckMonoObject(object))
	{
		return nullptr;
	}

	if (current_game_object == nullptr)
	{
		return nullptr;
	}

	MonoType* t = mono_reflection_type_get_type(type);
	std::string name = mono_type_get_name(t);
	const char* comp_name = "";

	MonoClass* classT = nullptr;

	/* Components */
	if (name == "CulverinEditor.Transform")
	{
		comp_name = "Transform";
	}
	else if (name == "CulverinEditor.CompAudio")
	{
		comp_name = "CompAudio";
	}
	else if (name == "CulverinEditor.CompButton")
	{
		comp_name = "Button";
	}
	else if (name == "CulverinEditor.CompCollider")
	{
		comp_name = "CompCollider";
	}
	/* Scripts */
	if (comp_name == "")
	{
		if (App->resource_manager->GetResource(name.c_str()) != nullptr)
		{
			GameObject* actual_temp = game_objects[object];
			if (actual_temp != nullptr)
			{
				temp.clear();
				actual_temp->GetComponentsByType(Comp_Type::C_SCRIPT, &temp);
				for (int i = 0; i < temp.size(); i++)
				{
					if (strcmp(((CompScript*)temp[i])->resource_script->name, name.c_str()) == 0)
					{
						comp_name = name.c_str();
						classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "", comp_name);
					}
				}
			}
		}
	}
	else
	{
		GameObject* actual_temp = game_objects[object];
		if (actual_temp->GetComponentByName(comp_name) != nullptr) // if has component
		{
			classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", comp_name);
		}
	}

	if (classT)
	{
		MonoObject* new_object = mono_object_new(CSdomain, classT);
		if (new_object)
		{
			return new_object;
		}
	}
	return nullptr;
}

MonoObject* CSharpScript::GetParentGameObject()
{
	if (CSSelfObject != nullptr && own_game_object != nullptr)
	{
		return CSSelfObject;
	}
}

MonoObject* CSharpScript::Find(MonoObject * object, MonoString * name)
{
	GameObject* found = nullptr;
	found = current_game_object->GetChildbyName(mono_string_to_utf8(name));

	if (found != nullptr)
	{
		std::map<MonoObject*, GameObject*>::iterator iter = game_objects.begin();
		while (iter != game_objects.end())
		{
			if (iter._Ptr->_Myval.second == found)
			{
				return iter._Ptr->_Myval.first;
			}

			iter++;
		}
	}
	
	LOG("Obj not found!");

	return nullptr;
}

MonoObject * CSharpScript::GetForwardVector(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetForwardVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetBackwardVector(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetBackwardVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetUpVector(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetUpVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetDownVector(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetDownVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetRightVector(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetRightVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetLeftVector(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_vec;
				new_vec = transform->GetLeftVector();

				if (x_field) mono_field_set_value(new_object, x_field, &new_vec.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_vec.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_vec.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

void CSharpScript::SetForwardVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);
		
		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetForwardVector(new_vec);
	}
}

void CSharpScript::SetBackwardVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetBackwardVector(new_vec);
	}
}

void CSharpScript::SetUpVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetUpVector(new_vec);
	}
}

void CSharpScript::SetDownVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetDownVector(new_vec);
	}
}

void CSharpScript::SetRightVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetRightVector(new_vec);
	}
}

void CSharpScript::SetLeftVector(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_vec;

		if (x_field) mono_field_get_value(vector3, x_field, &new_vec.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_vec.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_vec.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetLeftVector(new_vec);
	}
}

MonoObject* CSharpScript::GetPosition(MonoObject* object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_pos;
				new_pos = transform->GetPos();

				if (x_field) mono_field_set_value(new_object, x_field, &new_pos.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_pos.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_pos.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetGlobalPosition(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_pos;
				new_pos = transform->GetPosGlobal();

				if (x_field) mono_field_set_value(new_object, x_field, &new_pos.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_pos.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_pos.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

// We need to pass the MonoObject* to get a reference on act
void CSharpScript::SetPosition(MonoObject* object, MonoObject* vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetPos(new_pos);
	}
}

void CSharpScript::SetGlobalPosition(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &new_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_pos.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetPosGlobal(new_pos);
	}
}

void CSharpScript::Translate(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 added_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &added_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &added_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &added_pos.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->Translate(added_pos);
	}
}

MonoObject* CSharpScript::GetRotation(MonoObject* object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_pos;
				new_pos = transform->GetRotEuler();

				if (x_field) mono_field_set_value(new_object, x_field, &new_pos.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_pos.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_pos.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetGlobalRotation(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 new_pos;
				new_pos = transform->GetGlobalRotEuler();

				if (x_field) mono_field_set_value(new_object, x_field, &new_pos.x);
				if (y_field) mono_field_set_value(new_object, y_field, &new_pos.y);
				if (z_field) mono_field_set_value(new_object, z_field, &new_pos.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

void CSharpScript::SetRotation(MonoObject* object, MonoObject* vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetRot(new_rot);
	}
}

void CSharpScript::SetGlobalRotation(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetRotGlobal(new_rot);
	}
}

void CSharpScript::IncrementRotation(MonoObject* object, MonoObject* vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->IncrementRot(new_rot);
	}
}

void CSharpScript::RotateAroundAxis(MonoObject * object, MonoObject * vector3, float value)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_rot;

		if (x_field) mono_field_get_value(vector3, x_field, &new_rot.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_rot.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_rot.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->RotateAroundAxis(new_rot, value);
	}
}

void CSharpScript::SetScale(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 new_scale;

		if (x_field) mono_field_get_value(vector3, x_field, &new_scale.x);
		if (y_field) mono_field_get_value(vector3, y_field, &new_scale.y);
		if (z_field) mono_field_get_value(vector3, z_field, &new_scale.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->SetScale(new_scale);
	}
}

MonoObject * CSharpScript::GetScale(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Vector3");
		if (classT)
		{
			MonoObject* new_object = mono_object_new(App->importer->iScript->GetDomain(), classT);
			if (new_object)
			{
				MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
				MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
				MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

				CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
				float3 get_scale;
				get_scale = transform->GetScale();

				if (x_field) mono_field_set_value(new_object, x_field, &get_scale.x);
				if (y_field) mono_field_set_value(new_object, y_field, &get_scale.y);
				if (z_field) mono_field_set_value(new_object, z_field, &get_scale.z);

				return new_object;
			}
		}
		return nullptr;
	}
}

void CSharpScript::LookAt(MonoObject * object, MonoObject * vector3)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(vector3);
		MonoClassField* x_field = mono_class_get_field_from_name(classT, "x");
		MonoClassField* y_field = mono_class_get_field_from_name(classT, "y");
		MonoClassField* z_field = mono_class_get_field_from_name(classT, "z");

		float3 target_pos;

		if (x_field) mono_field_get_value(vector3, x_field, &target_pos.x);
		if (y_field) mono_field_get_value(vector3, y_field, &target_pos.y);
		if (z_field) mono_field_get_value(vector3, z_field, &target_pos.z);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->LookAt(target_pos);
	}
}

void CSharpScript::LookAtTrans(MonoObject * object, MonoObject * transform)
{
	if (current_game_object != nullptr)
	{
		MonoClass* classT = mono_object_get_class(transform);
		MonoClassField* field = mono_class_get_field_from_name(classT, "position");

		float3 target_pos;

		if (field) mono_field_get_value(transform, field, &target_pos);

		CompTransform* transform = (CompTransform*)current_game_object->GetComponentTransform();
		transform->LookAt(target_pos);
	}
}

void CSharpScript::PlayAudioEvent(MonoObject* object, MonoString* event_name)
{
	if (current_game_object != nullptr)
	{
		CompAudio* audio = (CompAudio*)current_game_object->FindComponentByType(Comp_Type::C_AUDIO);
		if (audio != nullptr)
		{
			audio->PlayAudioEvent(mono_string_to_utf8(event_name));
		}
	}
}

void CSharpScript::StopAudioEvent(MonoObject* object, MonoString* event_name)
{
	if (current_game_object != nullptr)
	{
		CompAudio* audio = (CompAudio*)current_game_object->FindComponentByType(Comp_Type::C_AUDIO);
		if (audio != nullptr)
		{
			audio->StopAudioEvent(mono_string_to_utf8(event_name));
		}
	}
}

void CSharpScript::SetAuxiliarySends(MonoObject * object, MonoString * bus, float value)
{
	if (current_game_object != nullptr)
	{
		CompAudio* audio = (CompAudio*)current_game_object->FindComponentByType(Comp_Type::C_AUDIO);
		if (audio != nullptr)
		{
			audio->SetAuxiliarySend(mono_string_to_utf8(bus), value);
		}
	}
}

void CSharpScript::Activate(MonoObject* object, int uid)
{
	if (current_game_object != nullptr)
	{
		
		CompInteractive* interactive = (CompInteractive*)current_game_object->FindComponentByType(Comp_Type::C_BUTTON);
		if (interactive != nullptr)
		{
			interactive->Activate();
			return;
		}		
		interactive = (CompInteractive*)current_game_object->FindComponentByType(Comp_Type::C_CHECK_BOX);
		if (interactive != nullptr)
		{
			interactive->Activate();
			return;

		}
	}
}

void CSharpScript::Deactivate(MonoObject * object, int uid)
{
	if (current_game_object != nullptr)
	{

		CompInteractive* interactive = (CompInteractive*)current_game_object->FindComponentByType(Comp_Type::C_BUTTON);
		if (interactive != nullptr)
		{
			interactive->Deactive();
			return;
		}
		interactive = (CompInteractive*)current_game_object->FindComponentByType(Comp_Type::C_CHECK_BOX);
		if (interactive != nullptr)
		{
			interactive->Deactive();
			return;

		}
	}
}

void CSharpScript::Clicked(MonoObject * object)
{
	if (current_game_object != nullptr)
	{
		CompButton* interactive = (CompButton*)current_game_object->FindComponentByType(Comp_Type::C_BUTTON);
		if (interactive != nullptr)
		{
			interactive->OnClick();
		}
	}
}

MonoObject * CSharpScript::GetCollidedObject(MonoObject * object)
{
	GameObject* target = ((CompCollider*)current_game_object->FindComponentByType(Comp_Type::C_COLLIDER))->GetCollidedObject();
	if (target == nullptr)((CompCollider*)current_game_object->FindComponentByType(Comp_Type::C_COLLIDER))->GetCollidedObject();
	if (target == nullptr)return nullptr;

	std::map<MonoObject*, GameObject*>::iterator iter = game_objects.begin();
	while (iter != game_objects.end())
	{
		if (iter._Ptr->_Myval.second == target)
		{
			return iter._Ptr->_Myval.first;
		}

		iter++;
	}
	return nullptr;
}

// Map ------------------------------------------------
MonoString* CSharpScript::GetMapString(MonoObject* object)
{
	return mono_string_new(CSdomain, App->map->map_string.c_str());
}

void CSharpScript::Save(JSON_Object* object, std::string name) const
{
	std::string vars = name;
	vars += "Number of Variables: ";
	json_object_dotset_number_with_std(object, vars, variables.size());

	std::string temp_var = name;

	for (int i = 0; i < variables.size(); i++)
	{
		temp_var = name + "Variables.Variable " + std::to_string(i);
		temp_var += ".";
		
		variables[i]->Serialize(object, temp_var);

		if (variables[i]->type == VarType::Var_GAMEOBJECT)
		{
			if (variables[i]->game_object != nullptr)
			{
				json_object_dotset_number_with_std(object, name + "Variables GameObject UUID " + std::to_string(i), variables[i]->game_object->GetUUID());
			}
		}
	}
}

void CSharpScript::Load(const JSON_Object* object, std::string name)
{
	game_objects.clear(); // memory leak
	for (int i = 0; i < variables.size(); i++)
	{
		if (variables[i]->type == VarType::Var_GAMEOBJECT)
		{
			uint temp = json_object_dotget_number_with_std(object, name + "Variables GameObject UUID " + std::to_string(i));
			re_load_values.push_back(temp);
		}
	}
}

void CSharpScript::LoadValues()
{
	for (int i = 0, j = 0; i < variables.size(); i++)
	{
		if (variables[i]->type == VarType::Var_GAMEOBJECT && re_load_values.size() > 0)
		{
			variables[i]->game_object = App->scene->GetGameObjectbyuid(re_load_values[j++]);
			variables[i]->SetMonoValue(variables[i]->game_object);
		}
	}
	re_load_values.clear();
}
