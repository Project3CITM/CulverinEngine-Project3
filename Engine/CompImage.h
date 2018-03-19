#ifndef COMPONENT_IMAGE_H
#define COMPONENT_IMAGE_H
#include <vector>
#include "CompGraphic.h"
class ResourceMaterial;
class CompRectTransform;

struct AtlasTextureData;

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
	void FillAmount(float value);
	void GenerateFilledSprite(float fill, FillMethod method);
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
	void GenerateUVBuffers();
	float4 GetColor()const;
	ResourceMaterial* GetSourceImage()const;
	ResourceMaterial* GetCurrentTexture()const;

private:
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
	ResourceMaterial* overwrite_image = nullptr;
	Type type = SIMPLE;
	FillMethod method = HORITZONTAL;
	uint uuid_source_image = 0;
	bool select_source_image = false;
	float filled = 1.0f;

	//Animation values
	bool animated = false;
	bool AnimationEditorOpen = false;
//	AtlasTextureData AtlasData;
	std::vector<unsigned int> SpritesUV_ID;				//Image animation UVs IDs
	std::vector<float4> SpritesUV_Data;					//Image animation UVs: UV0 = X-Y		UV1 = Z-W

};

struct AtlasTextureData
{
	void Set(unsigned int ID, unsigned int width, unsigned int height, int columns = 1, int rows = 1, int frames = 1);

	unsigned int TextureID = 0;		
	unsigned int TextureW = 0;		
	unsigned int TextureH = 0;		

	int columns = 1;				
	int rows = 1;					
	int numberOfFrames = 1;
};
#endif//COMPONENT_IMAGE_
