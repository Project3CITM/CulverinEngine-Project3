#include "CompButton.h"
#include "Application.h"





CompButton::CompButton(Comp_Type t, GameObject * parent) :CompInteractive(t, parent)
{
	uid = App->random->Int();
	name_component = "Button";
}


CompButton::CompButton(const CompButton & copy, GameObject * parent) : CompInteractive(Comp_Type::C_BUTTON, parent)
{
}

CompButton::~CompButton()
{
}
