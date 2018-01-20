#ifndef _COMPONENT_
#define _COMPONENT_

#include "Globals.h"
#include "ImGui\imgui.h"
#include <string>

struct json_object_t;
typedef struct json_object_t JSON_Object;

class GameObject;

enum Comp_Type 
{	
	C_UNKNOWN = -1,
	C_TRANSFORM,
	C_MESH,
	C_MATERIAL,
	C_CAMERA,
	C_SCRIPT
};

class Component
{
public:
	Component(Comp_Type t, GameObject* parent);
	virtual ~Component();

	virtual bool Enable();
	virtual bool Disable();
	virtual void Init();
	virtual void preUpdate(float dt);
	virtual void Update(float dt);
	virtual void Draw();
	virtual void Clear();

	// EDITOR METHODS -----------------
	virtual void ShowOptions();
	virtual void ShowInspectorInfo();
	// --------------------------------

	Comp_Type GetType() const;
	bool isActive() const;
	void SetActive(bool active);
	uint GetUUID() const;

	const char* GetName() const
	{
		return nameComponent;
	}
	bool WantDelete()
	{
		return toDelete;
	}

	// LOAD - SAVE METHODS ------------------
	virtual void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	virtual void Load(const JSON_Object* object, std::string name);

private:
	Comp_Type type = C_UNKNOWN;
	bool active = true;

protected:
	GameObject* parent = nullptr; // Through this pointer we call some methods that modificate its data
	uint uid = 0;
	const char* nameComponent = nullptr;
	bool toDelete = false;
};

#endif