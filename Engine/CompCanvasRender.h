#ifndef COMPONENT_CANVAS_RENDER_H
#define COMPONENT_CANVAS_RENDER_H
#include "Component.h"
class CompCanvasRender:public Component
{
public:
	CompCanvasRender(Comp_Type t, GameObject* parent);
	CompCanvasRender(const CompCanvasRender& copy, GameObject* parent);
	~CompCanvasRender();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCanvasRender * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
};

#endif // !COMPONENT_CANVAS_RENDER_H
