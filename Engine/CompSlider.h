#ifndef COMPONENT_SLIDER_H
#define COMPONENT_SLIDER_H
#include "CompInteractive.h"

class CompSlider : public CompInteractive
{

public:
	CompSlider(Comp_Type t, GameObject* parent);
	CompSlider(const CompImage& copy, GameObject* parent);
	~CompSlider();

	void ShowOptions();
	void ShowInspectorInfo();
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);

	//void OnPointDown(Event event_input);
	void OnDrag(Event event_input);
	//bool PointerInside(float2 position);
	//void OnClick();


private:
public:
	CompImage * slide_bar = nullptr;
private:
	std::vector<CompScript*> linked_scripts;
	float min_pos = 0;
	float max_pos = 0;

};

#endif