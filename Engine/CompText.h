#ifndef COMPONENT_TEXT_H
#define COMPONENT_TEXT_H
#include "CompGraphic.h"
#include "Math/float4.h"


class ResourceFont;
struct SDL_Surface;




class CompText: public CompGraphic
{
public:
	CompText(Comp_Type t, GameObject* parent);
	CompText(const CompText& copy, GameObject* parent);
	~CompText();
	void PreUpdate(float dt);
	void Update(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void UpdateText();
	void FreeFont();
	void CopyValues(const CompText * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	void SetRect(float x, float y, float width, float height);
	void SetString(std::string input);

	void GenerateText();
private:
	bool TextCanFit(float4 rect_transform, float4 rect_text);
public:
	SDL_Surface * s_font = nullptr;
	ResourceFont* text = nullptr;
	std::string text_str;
private:
	bool update_text = false;

	float4 text_rect;
	bool show_resource_font_windows = false;
	std::string input_text;
	int max_input = 20;
	int text_size = 12;
	uint id_font = 0;
	uint uuid_source_font = 0;

};
#endif//COMPONENT_TEXT_H

