#ifndef _COMPONENT_MATERIAL_
#define _COMPONENT_MATERIAL_

#include "Component.h"
#include "Color.h"
#include <string>

#include "ModuleShaders.h"


class ResourceMaterial;

class CompMaterial : public Component
{
public:
	CompMaterial(Comp_Type t, GameObject* parent);
	CompMaterial(const CompMaterial& copy, GameObject* parent);
	~CompMaterial();

	void PreUpdate(float dt);
	void Clear();
	void SetColor(float r, float g, float b, float a);
	void SetUUIDMesh(uint uuid);

	Color GetColor() const;
	uint GetTextureID() const;

	// EDITOR METHODS ----------
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompMaterial * component);
	void ShowTextureVariable(int index);
	void ShowShadersEditors();
	// -------------------------

	// SAVE - LOAD METHODS ------------------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// --------------------------------------------

public:

	//ResourceMaterial* resource_material = nullptr;
	ShaderProgram material_shader;

private:

	Color color = White;
	bool select_material = false;

	uint uuid_material = 0;
	
};

#endif