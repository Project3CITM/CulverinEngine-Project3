#ifndef COMPONENT_CANVAS_H
#define COMPONENT_CANVAS_H

#include "Component.h"
#include "EventDef.h"
#include <vector>
class CompCanvasRender;
class CompRectTransform;
class CompCanvas:public Component
{
public:
	CompCanvas(Comp_Type t, GameObject* parent);
	CompCanvas(const CompCanvas& copy, GameObject* parent);
	~CompCanvas();
	void Update(float dt);
	void Clear();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCanvas * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	void AddCanvasRender(CompCanvasRender* to_add);
	void RemoveCanvasRender(CompCanvasRender* to_remove);
	void DrawCanvasRender();
private:
public:
private:
	Event draw_mode;
	CompRectTransform* my_transform = nullptr;
	std::vector<CompCanvasRender*> canvas_render;
};

#endif // !COMPONENT_CANVAS_H
