#ifndef COMPONENT_IMAGE_H
#define COMPONENT_IMAGE_H
#include "Math\float4.h"
#include "CompGraphic.h"
class ResourceMaterial;
class CompImage:public CompGraphic
{
public:
	CompImage(Comp_Type t, GameObject* parent);
	CompImage(const CompImage& copy, GameObject* parent);
	~CompImage();
	void PreUpdate(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompImage * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);

private:
public:
private:
	ResourceMaterial* source_image = nullptr;
	uint uuid_source_image = 0;
	float4 color;
	bool raycast_target=true;

};
#endif//COMPONENT_IMAGE_H
