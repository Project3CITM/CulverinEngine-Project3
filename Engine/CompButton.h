#ifndef COMPONENT_BUTTON_H
#define COMPONENT_BUTTON_H
#include "CompInteractive.h"
class CompImage;

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

private:
public:
private:
	CompImage* target_graphic = nullptr;


};

#endif//COMPONENT_BUTTON_H
