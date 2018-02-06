#ifndef COMPONENT_INTERACTIVE_H
#define COMPONENT_INTERACTIVE_H
#include "Component.h"

class CompInteractive:public Component
{
public:
	CompInteractive(Comp_Type t, GameObject* parent);
	CompInteractive(const CompInteractive& copy, GameObject* parent);

	~CompInteractive();
};
#endif // !COMPONENT_INTERACTIVE_H



