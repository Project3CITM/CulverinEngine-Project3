#ifndef COMPONENT_BUTTON_H
#define COMPONENT_BUTTON_H
#include "CompInteractive.h"

class CompButton:public CompInteractive
{
public:
	CompButton(Comp_Type t, GameObject* parent);
	CompButton(const CompButton& copy, GameObject* parent);
	~CompButton();
};

#endif//COMPONENT_BUTTON_H
