#ifndef COMPONENT_RECT_TRANSFORM_H
#define COMPONENT_RECT_TRANSFORM_H
#include "CompTransform.h"

class CompRectTransform:public CompTransform
{
public:
	CompRectTransform(Comp_Type t, GameObject* parent);
	CompRectTransform(const CompRectTransform& copy, GameObject* parent);
	~CompRectTransform();
	void Update(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void ShowTransform(float drag_speed);
	void CopyValues(const CompRectTransform * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);

	void SetWidth(int set_width);
	void SetHeight(int set_height);

	int GetWidth();
	int GetHeight();
private:
	int width = 0;
	int height = 0;

};

#endif //COMPONENT_RECT_TRANSFORM_H
