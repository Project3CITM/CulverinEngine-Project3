#ifndef COMPONENT_H
#define COMPONENT_H

#include "Globals.h"
#include "ImGui/imgui.h"
#include "Math\float3.h"
#include "Math\Quat.h"
#include <string>

struct json_object_t;
typedef struct json_object_t JSON_Object;

class GameObject;

enum Comp_Type 
{	
	C_UNKNOWN = -1,
	C_TRANSFORM,
	C_RECT_TRANSFORM,
	C_MESH,
	C_MATERIAL,
	C_CAMERA,
	C_COLLIDER,
	C_RIGIDBODY,
	C_UNIQUE_SEPARATOR,   //Components above this will be unique
	C_SCRIPT,
	C_ANIMATION,
	//button 10
	//check 11
	//C_CANVAS 12
	//canvas render 13
	//C_TEXT 13
	//C_EDIT_TEXT 14
	//image 15
	C_AUDIO = 17,
	C_BONE,
	C_FSM,
	C_LIGHT,
	C_PARTICLE_SYSTEM,
	C_JOINT,

	//Graphic
	C_CANVAS = 50,
	C_CANVAS_RENDER,
	C_TEXT,
	C_IMAGE,
	//Interactive
	C_EDIT_TEXT,
	C_CHECK_BOX,
	C_BUTTON,
	C_SLIDER
};

class Component
{
public:
	Component(Comp_Type t, GameObject* parent);
	virtual ~Component();

	virtual bool Enable();
	virtual bool Disable();
	virtual bool GetEnabled();
	virtual void Init();
	virtual void PreUpdate(float dt);
	virtual void Update(float dt);
	virtual void Draw();
	virtual void Clear();

	// LOAD - SAVE METHODS ------------------
	virtual void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	virtual void Load(const JSON_Object* object, std::string name);
	virtual void SyncComponent(GameObject* sync_parent);

	// EDITOR METHODS -----------------
	virtual void ShowOptions();
	virtual void ShowInspectorInfo();
	// --------------------------------

	void SetActive(bool active);
	void SetUUIDRandom();

	uint GetUUID() const;
	Comp_Type GetType() const;
	bool IsActive() const;
	const char* GetName() const;
	bool WantDelete() const;

	float3 GetGameObjectPos() const;

	GameObject* GetParent()const;

private:
	Comp_Type type = C_UNKNOWN;

protected:
	bool active = true;

	GameObject* parent = nullptr; // Through this pointer we call some methods that modificate its data
	uint uid = 0;
	const char* name_component = nullptr;
	bool to_delete = false;
};

#endif