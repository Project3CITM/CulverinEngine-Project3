#ifndef _COMPONENT_MATERIAL_
#define _COMPONENT_MATERIAL_

#include "Component.h"
#include "Color.h"
#include <string>

class ResourceMaterial;

class CompMaterial : public Component
{
public:
	CompMaterial(Comp_Type t, GameObject* parent);
	CompMaterial(const CompMaterial& copy, GameObject* parent);
	~CompMaterial();

	void preUpdate(float dt);
	void Clear();
	void SetColor(float r, float g, float b, float a);
	void SetUUIDMesh(uint uuid);

	Color GetColor() const;
	uint GetTextureID() const;

	// EDITOR METHODS ----------
	void ShowOptions();
	void ShowInspectorInfo();
	// -------------------------

	// SAVE - LOAD METHODS ------------------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// --------------------------------------------

public:
	ResourceMaterial* resourceMaterial = nullptr;

private:
	Color color = White;
	bool selectMaterial = false;

	uint uuid_material = 0;
	uint uuidResourceReimported = 0;
};

#endif