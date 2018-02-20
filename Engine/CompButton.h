#ifndef COMPONENT_BUTTON_H
#define COMPONENT_BUTTON_H
#include "CompInteractive.h"

#include <vector>

class CompScript;

class CompButton:public CompInteractive
{
public:

	CompButton(Comp_Type t, GameObject* parent);
	CompButton(const CompButton& copy, GameObject* parent);
	~CompButton();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompButton * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);

	void AddLinkedScript(const CompScript* script);


private:

	void OnClick();
	void OnPointDown(Event event_input);
	void ShowInspectorAnimationTransition();

public:

private:

	std::vector<CompScript*> linked_scripts;

};

#endif//COMPONENT_BUTTON_H
