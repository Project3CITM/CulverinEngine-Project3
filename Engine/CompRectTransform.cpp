#include "CompRectTransform.h"
#include "Application.h"





CompRectTransform::CompRectTransform(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Rect Transform";
}

CompRectTransform::CompRectTransform(const CompRectTransform & copy, GameObject * parent) : Component(Comp_Type::C_MESH, parent)
{
}

CompRectTransform::~CompRectTransform()
{
}
