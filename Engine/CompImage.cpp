#include "CompImage.h"
#include "Application.h"





CompImage::CompImage(Comp_Type t, GameObject * parent) :CompGraphic(t, parent)
{
	uid = App->random->Int();
	name_component = "Image";
}

CompImage::CompImage(const CompImage & copy, GameObject * parent) :CompGraphic(Comp_Type::C_MESH, parent)
{
}

CompImage::~CompImage()
{
}
