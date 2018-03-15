#ifndef COMPONENT_TEXT_H
#define COMPONENT_TEXT_H
#include "CompGraphic.h"
#include "Math/float4.h"


class ResourceFont;
struct SDL_Surface;


struct TextureRect
{
	float x = 0;
	float y = 0;
	float width = 0;
	float height = 0;

};

class CompText: public CompGraphic
{
public:
	CompText(Comp_Type t, GameObject* parent);
	CompText(const CompText& copy, GameObject* parent);
	~CompText();
	void PreUpdate(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void UpdateText();
	void FreeFont();
	void CopyValues(const CompText * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	void SetRect(float x, float y, float width, float height);

private:
public:
	SDL_Surface * s_font = nullptr;
	ResourceFont* text = nullptr;
	std::string text_str;
private:
	bool update_text = false;
	TextureRect text_rect;
	float4 color = float4::one;
	bool show_resource_font_windows = false;
	void SetString(std::string input);
	std::string input_text;
	int max_input = 20;
	int text_size = 12;
	uint uuid_source_font = 0;

};
#endif//COMPONENT_TEXT_H

