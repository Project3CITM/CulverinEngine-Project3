#ifndef COMPONENT_H
#define COMPONENT_H

#include "Globals.h"
#include "ImGui/imgui.h"
#include "Math\float3.h"
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
	C_UNIQUE_SEPARATOR,
	C_SCRIPT,
	C_ANIMATION,
	C_BUTTON,
	C_CHECK_BOX,
	C_CANVAS,
	C_CANVAS_RENDER,
	C_TEXT,
	C_EDIT_TEXT,
	C_IMAGE,
	C_AUDIO,
	C_BONE,
	C_LIGHT
};

class Component
{
public:
	Component(Comp_Type t, GameObject* parent);
	virtual ~Component();

	virtual bool Enable();
	virtual bool Disable();
	virtual void Init();
	virtual void PreUpdate(float dt);
	virtual void Update(float dt);
	virtual void Draw();
	virtual void Clear();

	// LOAD - SAVE METHODS ------------------
	virtual void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	virtual void Load(const JSON_Object* object, std::string name);

	// EDITOR METHODS -----------------
	virtual void ShowOptions();
	virtual void ShowInspectorInfo();
	// --------------------------------

	void SetActive(bool active);

	uint GetUUID() const;
	Comp_Type GetType() const;
	bool IsActive() const;
	const char* GetName() const;
	bool WantDelete() const;

	float3 GetGameObjectPos() const;

private:
	Comp_Type type = C_UNKNOWN;
	bool active = true;

protected:
	GameObject* parent = nullptr; // Through this pointer we call some methods that modificate its data
	uint uid = 0;
	const char* name_component = nullptr;
	bool to_delete = false;
};

#endif