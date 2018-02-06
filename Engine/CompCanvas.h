#ifndef COMPONENT_CANVAS_H
#define COMPONENT_CANVAS_H
#include "Component.h"
class CompCanvas:public Component
{
public:
	CompCanvas(Comp_Type t, GameObject* parent);
	CompCanvas(const CompCanvas& copy, GameObject* parent);
	~CompCanvas();
};

#endif // !COMPONENT_CANVAS_H
