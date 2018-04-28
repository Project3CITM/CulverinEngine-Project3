#ifndef COMPONENT_BUTTON_H
#define COMPONENT_BUTTON_H
#include "CompInteractive.h"
#include "ClickAction.h"
#include <vector>

class CompScript;

class CompButton:public CompInteractive,public ClickAction
{
public:

	CompButton(Comp_Type t, GameObject* parent);
	CompButton(const CompButton& copy, GameObject* parent);
	~CompButton();
	void ShowOptions();
	void ShowInspectorInfo();
	void SelectAnimationState();
	void CopyValues(const CompButton * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);

	void SyncComponent(GameObject * sync_parent);

	void SyncScript();

	void OnClick();

private:

	void OnSubmit(Event event_input);

	void OnPointDown(Event event_input);

public:

private:
	int number_script = 0;
	int* uid_linked_scripts = nullptr;
	std::string	script_name;
};

#endif//COMPONENT_BUTTON_H
