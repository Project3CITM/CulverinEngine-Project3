#include "CompInteractive.h"
#include "Application.h"


CompInteractive::CompInteractive(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Canvas";
}

CompInteractive::CompInteractive(const CompInteractive & copy, GameObject * parent) :Component(copy.GetType(), parent)
{
}

CompInteractive::~CompInteractive()
{
}
