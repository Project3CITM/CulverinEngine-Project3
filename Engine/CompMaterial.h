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
	void ShowTextureVariable(int index, TextureVar*);
	void ShowIntVariable(int index, intVar*);
	void ShowFloatVariable(int index, floatVar*);
	void ShowVec3Variable(int index, float3Var*);
	void ShowBoolVariable(int index, boolVar*);
	void ShowColorVariable(int index, ColorVar*);
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
	bool cant_comiple_shader = false;
	std::string last_error_shader;
	uint uuid_material = 0;
	
};

#endif