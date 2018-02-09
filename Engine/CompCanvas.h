#ifndef COMPONENT_CANVAS_H
#define COMPONENT_CANVAS_H

#include "Component.h"
#include <vector>
class CompCanvasRender;

class CompCanvas:public Component
{
public:
	CompCanvas(Comp_Type t, GameObject* parent);
	CompCanvas(const CompCanvas& copy, GameObject* parent);
	~CompCanvas();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCanvas * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	void DrawCanvasRender();
private:
public:
private:
	std::vector<CompCanvasRender*> canvas_render;
};

#endif // !COMPONENT_CANVAS_H
