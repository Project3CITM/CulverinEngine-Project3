#ifndef COMPONENT_IMAGE_H
#define COMPONENT_IMAGE_H
#include "CompGraphic.h"
class CompImage:public CompGraphic
{
public:
	CompImage(Comp_Type t, GameObject* parent);
	CompImage(const CompImage& copy, GameObject* parent);
	~CompImage();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompImage * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
};
#endif//COMPONENT_IMAGE_H
