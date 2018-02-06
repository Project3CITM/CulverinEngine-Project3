#include "CompEditText.h"
#include "Application.h"


CompEditText::CompEditText(Comp_Type t, GameObject * parent) :CompInteractive(t, parent)
{
	uid = App->random->Int();
	name_component = "Edit Text";
}

CompEditText::CompEditText(const CompEditText & copy, GameObject * parent) : CompInteractive(Comp_Type::C_MESH, parent)
{
}

CompEditText::~CompEditText()
{
}
