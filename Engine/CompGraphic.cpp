#include "CompGraphic.h"
#include "Application.h"
#include "GameObject.h"
#include "CompCanvasRender.h"


CompGraphic::CompGraphic(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Graphic";
	transform = (CompRectTransform*)parent->FindComponentByType(Comp_Type::C_RECT_TRANSFORM);
	AddCanvasRender();
}

CompGraphic::CompGraphic(const CompGraphic & copy, GameObject * parent) : Component(copy.GetType(), parent)
{
}

CompGraphic::~CompGraphic()
{
}


void CompGraphic::AddCanvasRender()
{
	my_canvas_render = (CompCanvasRender*)parent->FindComponentByType(Comp_Type::C_CANVAS_RENDER);
	if (my_canvas_render != nullptr)
	{
		my_canvas_render->SetGraphic(this);
	}

}

void CompGraphic::SetTextureID(uint set_texture_id)
{
	texture_id = set_texture_id;
}

void CompGraphic::SetColor(const float4 & set_rgba)
{
	color = set_rgba;
}

void CompGraphic::SetColor(float set_r, float set_g, float set_b, float set_a)
{
	SetColor(float4(set_r, set_g, set_b, set_a));
}

uint CompGraphic::GetTextureID() const
{
	return texture_id;
}

CompRectTransform * CompGraphic::GetRectTrasnform() const
{
	return transform;
}

CompCanvasRender * CompGraphic::GetCanvasRender() const
{
	return my_canvas_render;
}

float4 CompGraphic::GetColor() const
{
	return color;
}
