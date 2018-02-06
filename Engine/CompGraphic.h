#ifndef COMPONENT_GRAPHIC_H
#define COMPONENT_GRAPHIC_H
#include "Component.h"

class CompGraphic : public Component
{
public:
	CompGraphic(Comp_Type t, GameObject* parent);
	CompGraphic(const CompGraphic& copy, GameObject* parent);
	~CompGraphic();
};

#endif//COMPONENT_GRAPHIC_H
