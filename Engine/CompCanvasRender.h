#ifndef COMPONENT_CANVAS_RENDER_H
#define COMPONENT_CANVAS_RENDER_H
#include "Component.h"
class CompCanvasRender:public Component
{
public:
	CompCanvasRender(Comp_Type t, GameObject* parent);
	CompCanvasRender(const CompCanvasRender& copy, GameObject* parent);
	~CompCanvasRender();
};

#endif // !COMPONENT_CANVAS_RENDER_H
