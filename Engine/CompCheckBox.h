#ifndef COMPONENT_CHECK_BOX_H
#define COMPONENT_CHECK_BOX_H
#include "CompInteractive.h"

#include <vector>

class CompScript;

class CompCheckBox:public CompInteractive
{
public:
	CompCheckBox(Comp_Type t, GameObject* parent);
	CompCheckBox(const CompCheckBox& copy, GameObject* parent);
	~CompCheckBox();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCheckBox * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	void OnPointDown(Event event_input);
	void OnClick();

private:
	void OnPointDown(Event event_input);
private:
	std::vector<CompScript*> linked_scripts;
	bool active = false;
	CompImage* Check = nullptr;
};

#endif//COMPONENT_CHECK_BOX_H
