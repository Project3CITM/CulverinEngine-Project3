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

	void OnDrag(Event event_input);



private:
public:
	CompImage * slide_bar = nullptr;
	//CompButton * slider = nullptr;
private:
	//Button caract
	int number_script = 0;
	std::vector<CompScript*> linked_scripts;
	int* uid_linked_scripts = nullptr;
	std::string	script_name;
	//
	float min_pos = 0;
	float max_pos = 0;
};

#endif