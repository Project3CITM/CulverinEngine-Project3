#ifndef COMPONENT_IMAGE_H
#define COMPONENT_IMAGE_H
#include "CompGraphic.h"
class CompImage:public CompGraphic
{
public:
	CompImage(Comp_Type t, GameObject* parent);
	CompImage(const CompImage& copy, GameObject* parent);
	~CompImage();
};
#endif//COMPONENT_IMAGE_H
