#ifndef COMPONENT_EDIT_TEXT_H
#define COMPONENT_EDIT_TEXT_H
#include "CompInteractive.h"

class CompEditText : public CompInteractive
{
public:
	CompEditText(Comp_Type t, GameObject* parent);
	CompEditText(const CompEditText& copy, GameObject* parent);
	~CompEditText();
};

#endif//COMPONENT_EDIT_TEXT_H

