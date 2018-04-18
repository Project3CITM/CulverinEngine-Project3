#ifndef COMPONENT_IMAGE_H
#define COMPONENT_IMAGE_H
#include "CompGraphic.h"
#include <vector>

class ResourceMaterial;
class CompRectTransform;

enum FillMethod
{
	HORITZONTAL,
	VERTICAL,
	RADIAL360,
	NONE
};

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
	void ShowMethodInfo();

	void FillAmount(float value);
	void DeviceCheck();
	void CopyValues(const CompImage * component);
	void Clear();
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	void UpdateSpriteId();
	void SetSourceImage(ResourceMaterial* set_source_image);
	void SetColor(const float4& set_rgba);
	void SetColor(float set_r, float set_g, float set_b, float set_a);
	void SetTextureID(uint uid);
	void SetOverwriteImage(ResourceMaterial* overwrite_image);
	void SetToFilled(bool filled);
	float4 GetColor()const;
	ResourceMaterial* GetSourceImage()const;
	ResourceMaterial* GetCurrentTexture()const;
private:
	void CorrectFillAmount();

	void GenerateFilledSprite();
	bool RadialCut(std::vector<float3>& position, std::vector<float3>& texture_cord, float fill_value, int box_corner, bool invert = true);
	void RadialCut(std::vector<float3>& modify,float cos, float sin, int box_corner, bool invert = false);
protected:
	void ExpandMesh();
public:
	enum Type
	{
		SIMPLE,
		FILLED
	};
	enum OriginHoritzontal
	{
		RIGHT,
		LEFT
	};
	enum OriginVertical
	{
		TOP,
		BOTTOM
	};

private:
	//CompImage * slide = nullptr;
	ResourceMaterial* source_image = nullptr;
	ResourceMaterial* controller_image = nullptr;
	ResourceMaterial* overwrite_image = nullptr;
	Type type = SIMPLE;
	FillMethod method = HORITZONTAL;
	uint uuid_source_image = 0;
	bool select_source_image = false;
	bool select_controller_source_image = false;

	float filled = 1.0f;
	bool radial_inverse = true;
};
#endif//COMPONENT_IMAGE_H
