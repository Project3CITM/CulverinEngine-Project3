#ifndef COMPONENT_IMAGE_H
#define COMPONENT_IMAGE_H
#include "CompGraphic.h"
class ResourceMaterial;
class CompRectTransform;
class CompImage:public CompGraphic
{
public:
	CompImage(Comp_Type t, GameObject* parent);
	CompImage(const CompImage& copy, GameObject* parent);
	~CompImage();
	void PreUpdate(float dt);
	void Update(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompImage * component);
	void Clear();
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	
	void SetSourceImage(ResourceMaterial* set_source_image);
	void SetColor(const float4& set_rgba);
	void SetColor(float set_r, float set_g, float set_b, float set_a);
	void SetTextureID(uint uid);
	void SetOverwriteImage(ResourceMaterial* overwrite_image);
	float4 GetColor()const;
	ResourceMaterial* GetSourceImage()const;
	ResourceMaterial* GetCurrentTexture()const;

private:
public:
private:
	ResourceMaterial* source_image = nullptr;
	ResourceMaterial* overwrite_image = nullptr;

	uint uuid_source_image = 0;
	bool raycast_target=true;
	bool select_source_image =false;

};
#endif//COMPONENT_IMAGE_H
