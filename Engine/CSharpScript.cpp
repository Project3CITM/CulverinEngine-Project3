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
#include "JSONSerialization.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompAudio.h"
#include "CompAnimation.h"
#include "CompButton.h"
#include "CompCollider.h"
#include "CompRigidBody.h"
#include "CompGraphic.h"
#include "CompImage.h"
#include "ModuleEventSystemV2.h"
#include "ModuleShaders.h"

//SCRIPT VARIABLE UTILITY METHODS ------
ScriptVariable::ScriptVariable(const char* name, VarType type, VarAccess access, CSharpScript* script) : name(name), type(type), access(access), script(script)
{
}

ScriptVariable::~ScriptVariable()
{
	if (value != nullptr && type != VarType::Var_STRING && type != VarType::Var_GAMEOBJECT)
	{
		delete[](char*)value;
		value = nullptr;
	}
}

void ScriptVariable::SetMonoValue(void* newVal)
{
	if (newVal != nullptr)
	{
		mono_field_set_value(script->GetMonoObject(), mono_field, newVal);
		if (type == VarType::Var_GAMEOBJECT)
		{
			/*MonoObject* object = mono_field_get_value_object(App->importer->iScript->GetDomain(), mono_field, script->GetMonoObject());

			//script->game_objects[object] = game_object;*/
			App->importer->iScript->UpdateMonoMap(game_object);
			std::string temp = mono_field_get_name(mono_field);
			App->importer->iScript->map_link_variables.insert(std::pair<std::string, GameObject*>(temp, game_object));
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
		//MonoObject* object = App->importer->iScript->GetMonoObject((GameObject*)newVal);
		//MonoObject* object = mono_field_get_value_object(App->importer->iScript->GetDomain(), mono_field, script->GetMonoObject());
		if (1)
		{
			//GameObject* temp = App->importer->iScript->GetGameObject(object);
			//App->importer->iScript->UpdateMonoMap(temp, true);
			std::map<std::string, GameObject*>::iterator it = App->importer->iScript->map_link_variables.begin();
			while (it != App->importer->iScript->map_link_variables.end())
			{
				if (it->second == game_object)
				{
					App->importer->iScript->map_link_variables.erase(it);
					break;
				}
				it++;
			}
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

void ScriptVariable::Save(JSON_Object * object, const std::string& title)
{
	const char* type_name = nullptr;

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

	//SET VAR NAME --------------------
	json_object_dotset_string_with_std(object, title + "Name: ", name);
}

void ScriptVariable::Load(const JSON_Object * object, const std::string& title, std::vector<uint>& re_load_values)
{
	// Load values from inspector previously saved
	switch (type)
	{
	case Var_UNKNOWN:
	{
		break;
	}

	case Var_INT:
	{
		int ival = json_object_dotget_number_with_std(object, title + "Value: ");
		*(int*)value = ival;
		SetMonoValue((int*)value);
		break;
	}

	case Var_FLOAT:
	{
		float fval = json_object_dotget_number_with_std(object, title + "Value: ");
		*(float*)value = fval;
		SetMonoValue((float*)value);
		break;
	}

	case Var_BOOL:
	{
		bool bval = json_object_dotget_boolean_with_std(object, title + "Value: ");
		*(bool*)value = bval;
		SetMonoValue((bool*)value);
		break;
	}

	case Var_STRING:
	{
		break;
	}

	case Var_CLASS:
	{
		break;
	}

	case Var_GAMEOBJECT:
	{
		uint obj_uid = json_object_dotget_number_with_std(object, title + "GameObject UUID: ");
		re_load_values.push_back(obj_uid);
		break;
	}

	default:
	{
		break;
	}
	}
}

//CSHARP SCRIPT FUNCTIONS ---------------
CSharpScript::CSharpScript()
{
}


CSharpScript::~CSharpScript()
{
	temp.clear();
	methods.clear();
}


void CSharpScript::LoadScript()
{
	if (CSObject)
	{
		//mono_runtime_object_init(CSObject);

		//Create main Functions
		Start = CreateMainFunction("Start", DefaultParam, FunctionBase::CS_Start);
		Update = CreateMainFunction("Update", DefaultParam, FunctionBase::CS_Update);
		FixedUpdate = CreateMainFunction("FixedUpdate", DefaultParam, FunctionBase::CS_Update);
		OnGUI = CreateMainFunction("OnGUI", DefaultParam, FunctionBase::CS_OnGUI);
		OnEnable = CreateMainFunction("OnEnable", DefaultParam, FunctionBase::CS_OnEnable);
		OnDisable = CreateMainFunction("OnDisable", DefaultParam, FunctionBase::CS_OnDisable);
		OnTriggerEnter = CreateMainFunction("OnTriggerEnter", DefaultParam, FunctionBase::CS_OnTriggerEnter);
		OnTriggerLost = CreateMainFunction("OnTriggerLost", DefaultParam, FunctionBase::CS_OnTriggerLost);
		OnContact = CreateMainFunction("OnContact", DefaultParam, FunctionBase::CS_OnContact);
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
	case FunctionBase::CS_OnContact:
	{
		if (OnContact.method != nullptr)
		{
			DoFunction(OnContact.method, parameters);
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
	current_game_object = own_game_object;
	// Do Main Function
	mono_runtime_invoke(function, GetMonoObject(), parameter, &exception);
	if (exception)
	{
		mono_print_unhandled_exception(exception);
	}
}

void CSharpScript::DoPublicMethod(PublicMethod* function, void** parameter)
{
	MonoObject* exception = nullptr;
	// Do Main Function
	mono_runtime_invoke(function->GetMethod(), mono_gchandle_get_target(function->references), parameter, &exception);
	if (exception)
	{
		mono_print_unhandled_exception(exception);
	}
}

bool CSharpScript::CheckMonoObject(MonoObject* object)
{
	if (object != nullptr)
	{
		current_game_object = App->importer->iScript->GetGameObject(object);
		return true;
	}
	return false;
}

MonoObject* CSharpScript::GetMonoObject() const
{
	return mono_gchandle_get_target(CSReference);
}

uint CSharpScript::GetReferences() const
{
	return CSReference;
}

void CSharpScript::SetMonoObject(MonoObject* new_object)
{
	CSObject = new_object;
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

void CSharpScript::CreateCSObject()
{
	if (CSClass)
	{
		CSObject = mono_object_new(App->importer->iScript->GetDomain(), CSClass);
		CSReference = mono_gchandle_new(CSObject, false);
		mono_runtime_object_init(CSObject);
		App->importer->iScript->UpdateMonoScript(this, CSReference);
	}
	else
	{
		LOG("[error] Cant create MonoObject!");
	}
}

void CSharpScript::SetClass(MonoClass* klass)
{
	CSClass = klass;
}

void CSharpScript::SetClassName(std::string _name)
{
	nameCSharp = _name;
}

void CSharpScript::SetNameSpace(std::string _name_space)
{
	name_space = _name_space;
}

MonoObject* CSharpScript::GetMonoObjectLink(std::string name)
{
	for (int i = 0; i < variables.size(); i++)
	{
		if (strcmp(variables[i]->name, name.c_str()) == 0)
		{
			std::map<std::string, GameObject*>::iterator it = App->importer->iScript->map_link_variables.begin();
			while (it != App->importer->iScript->map_link_variables.end())
			{
				if (strcmp(name.c_str(), it->first.c_str()) == 0)
				{
					GameObject* temp = it->second;
					if (temp == variables[i]->game_object)
					{
						return App->importer->iScript->GetMonoObject(temp);
					}
				}
				it++;
			}
		}
	}
	std::multimap<std::string, GameObject*>::iterator it = App->importer->iScript->map_link_variables.begin();
	while (it != App->importer->iScript->map_link_variables.end())
	{
		if (it->first == name)
		{
			return App->importer->iScript->GetMonoObject(it->second);
		}
		it++;
	}
	//return App->importer->iScript->GetMonoObject(App->importer->iScript->map_link_variables[name]);
	return nullptr;
}

bool CSharpScript::ReImport(std::string pathdll, std::string nameClass)
{
	MonoAssembly* assembly_ = mono_domain_assembly_open(App->importer->iScript->GetDomain(), pathdll.c_str());
	if (assembly_)
	{
		MonoImage* image_ = mono_assembly_get_image(assembly_);
		if (image_)
		{
			std::string classname_, name_space_;
			MonoClass* entity = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "", nameClass.c_str());
			//MonoClass* entity_ = App->importer->iScript->GetMonoClassFromImage(image_, name_space_, nameClass);
			if (entity)
			{
				SetImage(image_);
				SetClass(entity);
				SetClassName(nameClass);
				SetNameSpace(name_space_);

				//Set script info and functionality
				LoadScript();

				SetAssembly(assembly_);
				SetDomain(App->importer->iScript->GetDomain());
			}
			else
			{
				LOG("[error]Failed loading class %s\n", nameClass.c_str());
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
	App->importer->iScript->RemoveCSharpScriptFromMonoScript(this);
	for (uint i = 0; i < variables.size(); i++)
	{
		RELEASE(variables[i]);
	}
}

//Release memory allocated from old variables
void CSharpScript::ResetScriptVariables()
{
	for (uint i = 0; i < variables.size(); i++)
	{
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
	App->importer->iScript->UpdateMonoMap(own_game_object);
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

	int num_fields = 0;
	MonoClass* parent = mono_class_get_parent(CSClass);
	while (parent != nullptr)
	{
		num_fields = mono_class_num_fields(parent);
		for (uint i = 0; i < num_fields; i++)
		{
			field = mono_class_get_fields(parent, &iter);
			if (field != NULL)
			{
				type = mono_field_get_type(field);
				XaviStruct temp;
				temp.mono_field = field;
				temp.mono_type = type;
				//Insert this info pair into the map
				field_type.insert(std::pair<uint, XaviStruct>(count_variables++, temp));
			}
		}
		parent = mono_class_get_parent(parent);
		iter = nullptr;
	}
	iter = nullptr;

	num_fields = mono_class_num_fields(CSClass);
	//Fill field-type map from the script to after get its respective info
	for (uint i = 0; i < num_fields; i++)
	{
		field = mono_class_get_fields(CSClass, &iter);
		if (field != NULL)
		{
			type = mono_field_get_type(field);
			XaviStruct temp;
			temp.mono_field = field;
			temp.mono_type = type;
			//Insert this info pair into the map
			field_type.insert(std::pair<uint, XaviStruct>(count_variables++, temp));
		}
	}

	// From the previous map, fill VariablesScript vector that will contain info (name, type, value) of each variable
	for (std::multimap<uint, XaviStruct>::iterator it = field_type.begin(); it != field_type.end(); ++it)
	{
		VarType type = GetTypeFromMono(it->second.mono_type);
		VarAccess access = VarAccess::Var_PRIVATE;

		//Set info about accessibility of the variable -> DOESN'T WORK!!!
		flags = mono_field_get_flags(it->second.mono_field);
		if (flags == MONO_FIELD_ATTR_FAMILY)
		{
			access = VarAccess::Var_PRIVATE;
		}
		if (flags == MONO_FIELD_ATTR_PUBLIC)
		{
			access = VarAccess::Var_PUBLIC;
		}
		if (flags == MONO_FIELD_ATTR_PRIVATE)
		{
			access = VarAccess::Var_PRIVATE;
		}

		//Create variable
		ScriptVariable* new_var = new ScriptVariable(mono_field_get_name(it->second.mono_field), type, access, this);

		//Set its value
		GetValueFromMono(new_var, it->second.mono_field, it->second.mono_type);

		//Link to Mono properties
		LinkVarToMono(new_var, it->second.mono_field, it->second.mono_type);

		//Put it in variables vector
		variables.push_back(new_var);
	}
}

void CSharpScript::UpdateScriptVariables()
{
	uint count = 0;
	// From the map, update the value of each script
	for (std::multimap<uint, XaviStruct>::iterator it = field_type.begin(); it != field_type.end(); ++it)
	{
		//Set its value
		UpdateValueFromMono(variables[count++], it->second.mono_field, it->second.mono_type);
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

bool CSharpScript::NeedToLinkGO() const
{
	return re_load_values.size() > 0;
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
		if (variable->value != nullptr && variable->type != VarType::Var_STRING && variable->type != VarType::Var_GAMEOBJECT)
		{
			delete[](char*)variable->value;
			variable->value = nullptr;
		}

		if (variable->type != VarType::Var_STRING && variable->type != VarType::Var_GAMEOBJECT)
		{
			//Allocate memory
			variable->value = new char[mono_type_stack_size(mtype, NULL)];

			//Set value of the variable by passing it as a reference in this function
			mono_field_get_value(GetMonoObject(), mfield, variable->value);
		}
		else if (variable->type == VarType::Var_STRING)
		{
			MonoString* str = nullptr;
			//Set value of the variable by passing it as a reference in this function
			mono_field_get_value(GetMonoObject(), mfield, &str);

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
		if (variable->access == VarAccess::Var_PUBLIC)
		{
			if (variable->type != VarType::Var_STRING && variable->type != VarType::Var_GAMEOBJECT)
			{
				//Set value of the variable by passing it as a reference in this function
				mono_field_get_value(GetMonoObject(), mfield, variable->value);
			}
			else if (variable->type == VarType::Var_GAMEOBJECT)
			{
				if (variable->game_object != nullptr)
				{
					//Set value of the variable by passing it as a reference in this function
					mono_field_get_value(GetMonoObject(), mfield, variable->game_object);
				}
			}
			else
			{
				MonoString* str = nullptr;

				//Set value of the variable by passing it as a reference in this function
				mono_field_get_value(GetMonoObject(), mfield, &str);

				//Copy string into str_value (specific for strings)
				variable->str_value = mono_string_to_utf8(str);
			}
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

bool CSharpScript::IsStatic(MonoObject* object)
{
	if (object == nullptr)
	{
		return false;
	}
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current != nullptr)
	{
		return current->IsStatic();
	}
	else
	{
		LOG("[error] GameObject was null.");
	}
	return false;
}

mono_bool CSharpScript::IsActive(MonoObject* object)
{
	if (object == nullptr)
	{
		return false;
	}

	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current != nullptr)
	{
		return current->IsActive();
	}
	else
	{
		LOG("[error] GameObject was null.");
	}
	return false;
}

void CSharpScript::SetActive(MonoObject* object, mono_bool active)
{
	if (object == nullptr)
	{
		LOG("[error] MonoObject invalid");
	}
	else
	{
		GameObject* current = App->importer->iScript->GetGameObject(object);
		if (current != nullptr)
		{
			current->SetActive(active);
		}
		else
		{
			LOG("[error] GameObject was null.");
		}
	}
}

MonoObject* CSharpScript::GetOwnGameObject()
{
	return App->importer->iScript->GetMonoObject(own_game_object);
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
	//GameObject* gameobject = App->scene->CreateGameObject();
	//App->importer->iScript->UpdateMonoMap(gameobject, object);
	//game_objects[object] = gameobject;
}

void CSharpScript::SetName(MonoObject * object, MonoString * name)
{
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current == nullptr)
	{
		LOG("[error] MonoObject invalid");
	}
	else
	{
		if (current != nullptr)
		{
			current->SetName(mono_string_to_utf8(name));
			std::string temp_name = mono_string_to_utf8(name);

			current->NameNotRepeat(temp_name, false, current->GetParent());
		}
	}
}

MonoString* CSharpScript::GetName(MonoObject* object)
{
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current == nullptr)
	{
		return nullptr;
	}

	return mono_string_new(CSdomain, current->GetName());
}

void CSharpScript::SetTag(MonoObject * object, MonoString * tag)
{
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current == nullptr)
	{
		LOG("[error] MonoObject invalid");
	}
	else
	{
		if (current != nullptr)
		{
			current->SetTag(mono_string_to_utf8(tag));
		}
	}
}

MonoString* CSharpScript::GetTag(MonoObject* object)
{
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current == nullptr)
	{
		return nullptr;
	}
	return mono_string_new(CSdomain, current->GetTag());
}

bool CSharpScript::CompareTag(MonoObject * object, MonoString * tag)
{
	if (object)
	{
		GameObject* actual = App->importer->iScript->GetGameObject(object);
		if (actual)
		{
			return actual->CompareTag(mono_string_to_utf8(tag));
		}
	}
	return false;
}

MonoObject* CSharpScript::FindGameObjectWithTag(MonoObject* object, MonoString * tag)
{
	GameObject* target = App->scene->FindGameObjectWithTag(mono_string_to_utf8(tag));
	return App->importer->iScript->GetMonoObject(target);
}

int CSharpScript::ChildCount(MonoObject * object)
{
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current == nullptr) 
	{
		return current->GetNumChilds();
	}
	return 0;
}

MonoObject * CSharpScript::GetChildByIndex(MonoObject* object, int index)
{
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current == nullptr)
	{
		return nullptr;
	}
	if (current != nullptr)
	{
		GameObject* target = current->GetChildbyIndex(index);
		if (target != nullptr)
		{
			return App->importer->iScript->GetMonoObject(target);
		}
		return nullptr;
	}
}

MonoObject * CSharpScript::GetChildByName(MonoObject * object, MonoString * name)
{
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current == nullptr)
	{
		return nullptr;
	}
	GameObject* target = current->GetChildbyName(mono_string_to_utf8(name));
	return App->importer->iScript->GetMonoObject(target);
}

MonoObject* CSharpScript::GetChildByTagIndex(MonoObject* object, MonoString* tag, int index)
{
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current == nullptr)
	{
		return nullptr;
	}
	GameObject* target = current->GetChildByTagIndex(mono_string_to_utf8(tag), index);
	return App->importer->iScript->GetMonoObject(target);
}

MonoObject*	CSharpScript::Instantiate(MonoObject* object, MonoString* prefab_)
{
	const char* prefab = mono_string_to_utf8(prefab_);
	std::string directory_prebaf = App->fs->GetMainDirectory();
	directory_prebaf += "/";
	directory_prebaf += prefab;
	directory_prebaf += ".prefab.json";
	GameObject* gameobject = App->json_seria->GetLoadPrefab(directory_prebaf.c_str(), true);
	if (gameobject != nullptr)
	{
		App->scene->root->AddChildGameObject(gameobject);
		App->importer->iScript->UpdateMonoMap(gameobject);
		return App->importer->iScript->GetMonoObject(gameobject);
	}
	LOG("[error] with load prefab");
	return nullptr;
}

MonoObject*	CSharpScript::Instantiate_respawn(MonoObject* object, MonoString* prefab_, float time)
{
	const char* prefab = mono_string_to_utf8(prefab_);
	std::string directory_prebaf = App->fs->GetMainDirectory();
	directory_prebaf += "/";
	directory_prebaf += prefab;
	directory_prebaf += ".prefab.json";
	GameObject* gameobject = App->json_seria->GetLoadPrefab(directory_prebaf.c_str(), true);
	if (gameobject != nullptr)
	{
		//Event system
		Event e;
		e.Set_event_data_f(EventType::EVENT_SPAWN_GAMEOBJECT, 0, time);
		e.spawnGO.Tospawn = gameobject;
		PushEvent(e);
		App->importer->iScript->UpdateMonoMap(gameobject);
		return App->importer->iScript->GetMonoObject(gameobject);
	}
	LOG("[error] with load prefab");
	return nullptr;
}

void CSharpScript::Destroy(MonoObject* object, float time)
{
	GameObject* gameobject = App->importer->iScript->GetGameObject(object);
	if (gameobject != nullptr)
	{
		Event e;
		LOG("Destroy ITEM gameobject %s", gameobject->GetName());
		
		e.Set_event_data(EventType::EVENT_DELETE_GO, 0, time);
		e.delete_go.Todelte = gameobject;
		e.delete_go.uuid = gameobject->GetUUID();
		PushEvent(e);
	}
	else
	{
		LOG("[error] GameObject = null");
	}
}

MonoObject* CSharpScript::GetComponent(MonoObject* object, MonoReflectionType* type)
{
	//if (!CheckMonoObject(object))
	//{
	//	return nullptr;
	//}

	//if (current_game_object == nullptr)
	//{
	//	return nullptr;
	//}

	MonoType* t = mono_reflection_type_get_type(type);
	MonoClass* class_temp_comp = mono_type_get_class(t);
	std::string name_class = mono_class_get_name(class_temp_comp);
	std::string comp_name = "";

	MonoClass* classT = nullptr;

	/* Components */
	if (name_class == "Transform")
	{
		comp_name = "Transform";
	}
	else if (name_class == "CompAudio")
	{
		comp_name = "CompAudio";
	}
	else if (name_class == "CompButton")
	{
		comp_name = "CompButton";
	}
	else if (name_class == "CompImage")
	{
		comp_name = "CompImage";
	}
	else if (name_class == "CompCollider")
	{
		comp_name = "CompCollider";
	}
	else if (name_class == "CompRigidBody")
	{
		comp_name = "CompRigidBody";
	}
	else if (name_class == "CompJoint")
	{
		comp_name = "CompJoint";
	}
	else if (name_class == "CompAnimation")
	{
		comp_name = "CompAnimation";
	}
	else if (name_class == "CompParticleSystem")
	{
		comp_name = "CompParticleSystem";
	}
	else if (name_class == "CompMesh")
	{
		comp_name = "CompMesh";
	}
	else if (name_class == "CompRectTransform")
	{
		comp_name = "CompRectTransform";
	}
	else if (name_class == "CompMaterial")
	{
		comp_name = "CompMaterial";
	}
	else if (name_class == "CompLight")
	{
		comp_name = "CompLight";
	}
	else if (name_class == "CompText")
	{
		comp_name = "CompText";
	}
	else if (name_class == "CompCanvas")
	{
		comp_name = "CompCanvas";
	}
	/* Scripts */
	if (comp_name == "")
	{
		if (App->resource_manager->GetResource(name_class.c_str()) != nullptr)
		{
			GameObject* actual_temp = App->importer->iScript->GetGameObject(object);
			if (actual_temp != nullptr)
			{
				temp.clear();
				actual_temp->GetComponentsByType(Comp_Type::C_SCRIPT, &temp);
				for (int i = 0; i < temp.size(); i++)
				{
					CompScript* script = ((CompScript*)temp[i]);
					if (strcmp(script->resource_script->name.c_str(), name_class.c_str()) == 0)
					{
						return script->csharp->GetMonoObject();
					}
				}
			}
		}
	}
	else
	{
		GameObject* actual_temp = App->importer->iScript->GetGameObject(object);
		if (actual_temp)
		{
			Component* comp = actual_temp->GetComponentByName(comp_name.c_str());
			if (comp != nullptr) // if has component
			{
				MonoObject* obj = App->importer->iScript->GetMonoObject(comp);
				if (obj != nullptr)
				{
					return obj;
				}
				else
				{
					classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", comp_name.c_str());
					MonoObject* new_object = mono_object_new(CSdomain, classT);
					if (new_object)
					{
						App->importer->iScript->UpdateMonoComp(comp, mono_gchandle_new(new_object, false));
						comp->SetInScripting();
						return new_object;
					}
				}
			}
		}
	}
	return nullptr;
}

MonoObject* CSharpScript::GetParentGameObject(MonoObject* object)
{
	if (object != nullptr)
	{
		CSharpScript* temp = App->importer->iScript->GetScriptMono(object);
		if (temp != nullptr)
		{
			return App->importer->iScript->GetMonoObject(temp->own_game_object);
		}
		else
		{
			return App->importer->iScript->GetMonoObject(own_game_object);
		}
	}
	return nullptr;
}

mono_bool CSharpScript::GetEnabled(MonoObject* object)
{
	Component* comp = App->importer->iScript->GetComponentMono(object);
	if (comp != nullptr)
	{
		return comp->GetEnabled();
	}
	else
	{
		MonoClass* class_temp = mono_object_get_class(object);
		std::string name_component = mono_class_get_name(class_temp);
		CSharpScript* temp = App->importer->iScript->GetScriptMono(object);
		comp = temp->own_game_object->GetComponentByName(name_component.c_str());
		if (comp != nullptr)
		{
			return comp->GetEnabled();
		}
	}
	LOG("[error] MonoObject invalid");
	return NULL;
}

void CSharpScript::SetEnabled(MonoObject* object, mono_bool active)
{
	Component* comp = App->importer->iScript->GetComponentMono(object);
	MonoClass* class_temp = mono_object_get_class(object);
	std::string name_component = mono_class_get_name(class_temp);
	if (comp == nullptr)
	{
		//comp = current_game_object->GetComponentByName(name_component.c_str());
		CSharpScript* temp = App->importer->iScript->GetScriptMono(object);
		comp = temp->own_game_object->GetComponentByName(name_component.c_str());
	}

	if (comp != nullptr && comp->GetEnabled() != active)
	{
		if (comp->GetType() != Comp_Type::C_SCRIPT)
		{
			if (active == true)
			{
				comp->Enable();
			}
			else
			{
				comp->Disable();
			}
		}
		else
		{
			if (active == true)
			{
				comp->Enable();
				((CompScript*)comp)->p_active = true;
			}
			else
			{
				comp->Disable();
				Event script;
				script.Set_event_data(EventType::EVENT_SCRIPT_DISABLED);
				script.script_disabled.script = (CompScript*)comp;
				PushEvent(script);
			}
		}
	}
	else
	{
		if (comp == nullptr)
		{
			LOG("[error] %s does not have %s", current_game_object->GetName(), name_component.c_str());
		}
		else if (comp->GetEnabled() != active)
		{
			if (active == true)
			{
				LOG("Component is already active");
			}
			else
			{
				LOG("Component is already not active");
			}
		}
	}
}

MonoObject* CSharpScript::Find(MonoObject * object, MonoString * name)
{
	GameObject* current = App->importer->iScript->GetGameObject(object);
	if (current == nullptr)
	{
		return nullptr;
	}
	GameObject* found = nullptr;
	found = current->GetChildbyName(mono_string_to_utf8(name));
	return App->importer->iScript->GetMonoObject(found);
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

		variables[i]->Save(object, temp_var);
	}
}

void CSharpScript::Load(const JSON_Object* object, std::string name)
{
	if (re_load_values.size() > 0)
		re_load_values.clear();
	std::string temp_var = name;

	//Once set the default values for the variables, update them to the inspector values saved previously
	int num_variables = json_object_dotget_number_with_std(object, name + "Number of Variables: ");
	for (int i = 0; i < num_variables; i++)
	{
		temp_var = name + "Variables.Variable " + std::to_string(i);
		temp_var += ".";


		std::string name_variable_temp = json_object_dotget_string_with_std(object, temp_var + "Name: ");
		for (int j = 0; j < variables.size(); j++)
		{
			if (strcmp(variables[j]->name, name_variable_temp.c_str()) == 0)
			{
				variables[j]->Load(object, temp_var, re_load_values);
			}
		}
	}
}

// Link script variables that has GameObjects assigned
void CSharpScript::LoadValuesGO(GameObject* sync_parent)
{
	for (int i = 0, j = 0; i < variables.size(); i++)
	{
		if (variables[i]->type == VarType::Var_GAMEOBJECT && re_load_values.size() > 0 && re_load_values.size() > j)
		{
			if (sync_parent != nullptr)
			{
				variables[i]->game_object = sync_parent->GetGameObjectbyuid(re_load_values[j++]);
			}
			else
			{
				variables[i]->game_object = App->scene->GetGameObjectbyuid(re_load_values[j++]);
			}
			variables[i]->SetMonoValue(variables[i]->game_object);
		}
	}
	re_load_values.clear();
}

GameObject * CSharpScript::GetGameObject() const
{
	return own_game_object;
}

void CSharpScript::SetBool(MonoObject * object, MonoString * name, bool value)
{
	Material * mat = App->importer->iScript->GetMaterialMono(object);

	for (auto it = mat->bool_variables.begin(); it != mat->bool_variables.end(); it++) {
		if (strcmp((*it).var_name.c_str(), mono_string_to_utf8(name)) == 0) {
			(*it).value = value;
			break;
		}
	}
}

void CSharpScript::SetFloat(MonoObject * object, MonoString * name, float value)
{
	Material * mat = App->importer->iScript->GetMaterialMono(object);

	for (auto it = mat->float_variables.begin(); it != mat->float_variables.end(); it++) {
		if (strcmp((*it).var_name.c_str(), mono_string_to_utf8(name)) == 0) {
			(*it).value = value;
			break;
		}
	}
}

MonoObject * CSharpScript::GetMaterialByName(MonoString * name)
{
	MonoClass* classT = nullptr;

	Material* comp = App->module_shaders->GetMaterialByName(mono_string_to_utf8(name));
	if (comp != nullptr) // if has component
	{
		MonoObject* obj = App->importer->iScript->GetMonoObject(comp);
		if (obj != nullptr)
		{
			return obj;
		}
		else
		{
			classT = mono_class_from_name(App->importer->iScript->GetCulverinImage(), "CulverinEditor", "Material");
			MonoObject* new_object = mono_object_new(CSdomain, classT);
			if (new_object)
			{
				mono_runtime_object_init(new_object);
				App->importer->iScript->UpdateMonoMaterial(comp, mono_gchandle_new(new_object, false));

				return new_object;
			}
		}
	}

	return nullptr;
}