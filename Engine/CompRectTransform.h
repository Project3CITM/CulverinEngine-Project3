#ifndef COMPONENT_RECT_TRANSFORM_H
#define COMPONENT_RECT_TRANSFORM_H
#include "Component.h"
class CompRectTransform:public Component
{
public:
	CompRectTransform(Comp_Type t, GameObject* parent);
	CompRectTransform(const CompRectTransform& copy, GameObject* parent);
	~CompRectTransform();
};

#endif //COMPONENT_RECT_TRANSFORM_H
