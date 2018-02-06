#include "CompCheckBox.h"
#include "Application.h"


CompCheckBox::CompCheckBox(Comp_Type t, GameObject * parent) :CompInteractive(t, parent)
{
	uid = App->random->Int();
	name_component = "Check Box";
}

CompCheckBox::CompCheckBox(const CompCheckBox & copy, GameObject * parent) : CompInteractive(Comp_Type::C_MESH, parent)
{
}

CompCheckBox::~CompCheckBox()
{
}
