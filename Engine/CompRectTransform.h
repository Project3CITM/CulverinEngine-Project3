#ifndef COMPONENT_RECT_TRANSFORM_H
#define COMPONENT_RECT_TRANSFORM_H
#include "Component.h"

class CompRectTransform:public Component
{
public:
	CompRectTransform(Comp_Type t, GameObject* parent);
	CompRectTransform(const CompRectTransform& copy, GameObject* parent);
	~CompRectTransform();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompRectTransform * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
};

#endif //COMPONENT_RECT_TRANSFORM_H
