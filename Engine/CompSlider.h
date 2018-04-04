#ifndef COMPONENT_SLIDER_H
#define COMPONENT_SLIDER_H
#include "CompInteractive.h"
#include "ClickAction.h"
#include <vector>


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

	void OnDrag(Event event_input);
	void OnPointDown(Event event_input);

private:
public:
	CompImage * slide_bg = nullptr;
	CompImage * slide_bar = nullptr;
	CompImage * slide_ball = nullptr;
private:
	int number_script = 0;
	int* uid_linked_scripts = nullptr;
	std::string	script_name;
	//slider
	float min_pos = 0;
	float max_pos = 0;

};

#endif