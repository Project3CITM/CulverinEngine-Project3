#include "CompGraphic.h"
#include "Application.h"
#include "GameObject.h"
#include "CompCanvasRender.h"
#include "CompCanvas.h"
#include "CompRectTransform.h"
CompGraphic::CompGraphic(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Graphic";
	
}

CompGraphic::CompGraphic(const CompGraphic & copy, GameObject * parent) : Component(copy.GetType(), parent)
{
}

CompGraphic::~CompGraphic()
{
}

void CompGraphic::Clear()
{
	if(my_canvas!=nullptr)
		my_canvas->RemoveGraphic(this);
	my_canvas = nullptr;
	my_canvas_render = nullptr;
	transform = nullptr;

	
}

void CompGraphic::AddCanvas()
{
	CompCanvas* current = my_canvas;
	my_canvas = (CompCanvas*)parent->FindParentComponentByType(Comp_Type::C_CANVAS);
	if (current != my_canvas)
	{
		if (current != nullptr)
		{
			current->RemoveGraphic(this);
		}
		my_canvas->AddGraphic(this);
	//	if(graphic)
	}
}


void CompGraphic::AddCanvasRender()
{
	my_canvas_render = (CompCanvasRender*)parent->FindComponentByType(Comp_Type::C_CANVAS_RENDER);
	if (my_canvas_render != nullptr)
	{
		
		my_canvas_render->SetGraphic(this);
		my_canvas_render->ProcessQuad(transform->GenerateQuadVertices());

	}

}
void CompGraphic::AddRectTransform()
{
	transform = (CompRectTransform*)parent->FindComponentByType(Comp_Type::C_RECT_TRANSFORM);
	

}
void CompGraphic::SyncComponent(GameObject* sync_parent)
{
	AddRectTransform();
	AddCanvasRender();
	AddCanvas();
}
void CompGraphic::DrawGraphic()
{
	if (my_canvas_render == nullptr)
		return;

	if (render)
	{
		my_canvas_render->DrawGraphic();
	}
	
}

void CompGraphic::DrawRectTranforms()
{
	if (transform == nullptr)
		return;
	transform->DrawRectTransform();
}

void CompGraphic::ActivateRender()
{
	render = true;
}

void CompGraphic::DeactivateRender()
{
	render = false;
}

void CompGraphic::SetNullCanvas()
{
	my_canvas = nullptr;
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

void CompGraphic::SetToRender(bool render)
{
	this->render = render;
}

void CompGraphic::SetRaycastTarget(bool flag)
{
	raycast_target = flag;
}

void CompGraphic::SetInteractive(CompInteractive * set_interactive)
{
	if (this != nullptr)
	{
		interactive = set_interactive;
	}
}

bool CompGraphic::GetToRender() const
{
	if (this != nullptr)
	{
		return render;
	}
	return nullptr;
}


uint CompGraphic::GetTextureID() const
{
	return texture_id;
}

CompRectTransform * CompGraphic::GetRectTrasnform() const
{
	if (this != nullptr)
	{
		return transform;
	}
	else return nullptr;
}

CompCanvasRender * CompGraphic::GetCanvasRender() const
{
	return my_canvas_render;
}

float4 CompGraphic::GetColor() const
{
	return color;
}

bool CompGraphic::GetParentActive()
{
	return parent->IsActive();
}
