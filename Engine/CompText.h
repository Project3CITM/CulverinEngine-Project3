#ifndef COMPONENT_TEXT_H
#define COMPONENT_TEXT_H
#include "CompGraphic.h"
class ResourceFont;
class CompText: public CompGraphic
{
public:
	CompText(Comp_Type t, GameObject* parent);
	CompText(const CompText& copy, GameObject* parent);
	~CompText();
	void PreUpdate(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompText * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
private:
public:
private:
	ResourceFont* resource_font = nullptr;
};
#endif//COMPONENT_TEXT_H

