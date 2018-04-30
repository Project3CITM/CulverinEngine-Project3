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
	void FreeFont();
	void CopyValues(const CompText * component);
	void Clear();

	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);

	void GetOwnBufferSize(uint& buffer_size);
	void SaveBinary(char** cursor, int position) const;
	void LoadBinary(char** cursor);

	void SyncComponent(GameObject * sync_parent);
	void SetString(std::string input);
	int GetWidth()const;
	int GetHeight()const;
private:	
	void UpdateText();
	bool GenerateText();
	bool TextCanFit(float4 rect_transform);
	void ReSizeInput();
protected:
	void ExpandMesh();
public:
	SDL_Surface * s_font = nullptr;
	ResourceFont* text = nullptr;
	std::string text_str;
private:
	enum HorizontalPosition
	{
		RIGHT_HPOSITION,
		MIDDLE_HPOSITION,
		LEFT_HPOSITION
	};
	enum VerticalPosition
	{
		TOP_VPOSITION,
		MIDDLE_VPOSITION,
		BOTTOM_POSITION
	};
	bool update_text = false;
	int text_width = 0;
	int text_height = 0;
	bool show_resource_font_windows = false;
	//std::string input_text;
	char* input_text = nullptr;
	int max_input = 20;
	int text_size = 12;
	HorizontalPosition h_position= HorizontalPosition::RIGHT_HPOSITION;
	VerticalPosition v_position = VerticalPosition::TOP_VPOSITION;
	int horizontal_position = 0;
	int vertical_position = 0;
	uint id_font = 0;
	uint uuid_source_font = 0;

};
#endif//COMPONENT_TEXT_H

