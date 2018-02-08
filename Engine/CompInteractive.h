#ifndef COMPONENT_INTERACTIVE_H
#define COMPONENT_INTERACTIVE_H
#include "Component.h"
#include <vector>
#include "Math\float4.h"


class ResourceMaterial;
class CompInteractive:public Component
{
public:
	CompInteractive(Comp_Type t, GameObject* parent);
	CompInteractive(const CompInteractive& copy, GameObject* parent);

	~CompInteractive();


	void PreUpdate(float dt);
	void ShowOptions();
	void CopyValues(const CompInteractive * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	//Setters Color tint parameters
	void SetNormalColor(const float4& set_rgba);
	void SetNormalColor(float set_r, float set_g, float set_b, float set_a);
	void SetHighlightedColor(const float4& set_rgba);
	void SetHighlightedColor(float set_r, float set_g, float set_b, float set_a);
	void SetPressedColor(const float4& set_rgba);
	void SetPressedColor(float set_r, float set_g, float set_b, float set_a);
	void SetDisabledColor(const float4& set_rgba);
	void SetDisabledColor(float set_r, float set_g, float set_b, float set_a);
	//Getters Sprite Swap  parameters
	void SetHighlightedSprite(ResourceMaterial* set_sprite);
	void SetPressedSprite(ResourceMaterial* set_sprite);
	void SetDisabledSprite(ResourceMaterial* set_sprite);
	//Getters Color tint parameters
	float4 GetNormalColor()const;
	float4 GetHighlightedColor()const;
	float4 GetPressedColor()const;
	float4 GetDisabledColor()const;
	//Getters Sprite Swap  parameters
	ResourceMaterial* GetHighligtedSprite()const;
	ResourceMaterial* GetPressedSprite()const;
	ResourceMaterial* GetDisabledSprite()const;

protected:
	//Color tint parameters
	float4 normal_color;
	float4 highlighted_color;
	float4 pressed_color;
	float4 disabled_color;
	float color_multiply=1.0f;
	float fade_duration=0.1f;
	//Sprite Swap parameters
	ResourceMaterial* sprite[3];
	uint uuid_reimported_sprite[3];

};
#endif // !COMPONENT_INTERACTIVE_H



