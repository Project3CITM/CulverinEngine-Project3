#include "CompCanvas.h"
#include "Application.h"




CompCanvas::CompCanvas(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Canvas";
}

CompCanvas::CompCanvas(const CompCanvas & copy, GameObject * parent) :Component(Comp_Type::C_CANVAS, parent)
{
}

CompCanvas::~CompCanvas()
{
}
