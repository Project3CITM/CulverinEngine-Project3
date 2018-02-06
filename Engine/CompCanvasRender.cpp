#include "CompCanvasRender.h"
#include "Application.h"

CompCanvasRender::CompCanvasRender(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Canvas Render";
}

CompCanvasRender::CompCanvasRender(const CompCanvasRender & copy, GameObject * parent) : Component(Comp_Type::C_MESH, parent)
{
}

CompCanvasRender::~CompCanvasRender()
{
}
