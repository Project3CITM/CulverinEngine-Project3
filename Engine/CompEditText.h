#ifndef COMPONENT_EDIT_TEXT_H
#define COMPONENT_EDIT_TEXT_H
#include "CompInteractive.h"

class CompEditText : public CompInteractive
{
public:
	CompEditText(Comp_Type t, GameObject* parent);
	CompEditText(const CompEditText& copy, GameObject* parent);
	~CompEditText();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompEditText * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
};

#endif//COMPONENT_EDIT_TEXT_H

