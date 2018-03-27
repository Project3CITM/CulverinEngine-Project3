#ifndef _ON_CLICK_H_
#define _ON_CLICK_H_
#include <vector>
#include "Globals.h"
struct json_object_t;
typedef struct json_object_t JSON_Object;
class ResourceScript;
class CompScript;
class GameObject;
struct PublicMethod;
class CSharpScript;
class ScriptVariable;

struct ClickActionData
{
public:
	PublicMethod* method = nullptr;
	GameObject* attacked = nullptr;
	int UID_attacked = -1;
	bool select_game_object = false;
	int selected_mode = 1;
	CompScript* script = nullptr;
	std::string current_mode = "Runtime Only";
	std::string current_script = "No Script";
	std::string current_function = "No Function";

	void* value = nullptr;
	GameObject* value_go = nullptr;
	std::string value_string;
};

class ClickAction
{
public:
	ClickAction();
	~ClickAction();
	void ShowOnClickInfo();
	void ShowTypeMethod(int index);
	void ShowVariablesInfo();
	void ShowVarType(ScriptVariable * var);
	void ShowVarValue(ScriptVariable * var, int pushi);
	void InitValueParamater(int index);
	void SaveClickAction(JSON_Object* object, std::string name)const;
	void LoadClickAction(const JSON_Object* object, std::string name);
	void SyncClickAction();
public:
	std::vector<ClickActionData> actions;
	CSharpScript* csharp = nullptr;
	ResourceScript* resource_script = nullptr;


};

#endif //_ON_CLICK_H_
