#include "CompGraphic.h"
#include "Application.h"
#include "GameObject.h"
#include "CompCanvasRender.h"
#include "CompCanvas.h"
#include "CompRectTransform.h"
#include "ModuleFS.h"
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

void CompGraphic::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);

	
	json_object_dotset_boolean_with_std(object, name + "Invalid", invalid);
	json_object_dotset_boolean_with_std(object, name + "Can draw", can_draw);
	json_object_dotset_boolean_with_std(object, name + "RayCast Target", raycast_target);

}

void CompGraphic::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	raycast_target = json_object_dotget_boolean_with_std(object, name + "RayCast Target");

	can_draw = json_object_dotget_boolean_with_std(object, name + "Can Draw");

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

		GenerateMesh();
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

	if (can_draw)
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
	can_draw = true;
}

void CompGraphic::DeactivateRender()
{
	can_draw = false;
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

void CompGraphic::SetAlpha(float alpha)
{
	color.w = alpha;
}

void CompGraphic::SetToRender(bool render)
{
	this->render = render;
}
void CompGraphic::SetCanDraw(bool render)
{
	if (this != nullptr)
	{
		this->can_draw = render;
	}
}

void CompGraphic::SetRaycastTarget(bool flag)
{
	raycast_target = flag;
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
	if (transform != nullptr)
	{
		return transform;
	}
	else
	return nullptr;
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

float CompGraphic::GetAlpha() const
{
	return color.w;
}
int CompGraphic::GetWidth()const
{
	if(transform==nullptr)
		return 0;
	return transform->GetWidth();
}
int CompGraphic::GetHeight()const
{
	if (transform == nullptr)
		return 0;
	return transform->GetHeight();
}
void CompGraphic::ResizeGenerateMesh()
{
	GenerateMesh();
}

void CompGraphic::ProcesQuad(std::vector<float3>& position, std::vector<float3>& texture_cord)
{
	uint indice_position = vertex_data.current_vertex_count;

	float2 res_fact = transform->GetResizeFactor();
	float3 curr_pos = float3::zero;
	for (uint i = 0; i < 4; i++)
	{
		curr_pos = position[i];
		curr_pos.x *= res_fact.x;
		curr_pos.y *= res_fact.y;
		vertex_data.AddVertex(curr_pos, float2(texture_cord[i].x, texture_cord[i].y));
	}
	
	vertex_data.AddTriangleIndex(indice_position, indice_position + 1, indice_position + 2);
	vertex_data.AddTriangleIndex(indice_position + 2, indice_position + 3, indice_position);

}
void CompGraphic::ProcesQuad(std::vector<float3>& position)
{
	std::vector<float3> tex_cord;
	tex_cord.push_back(float3(0.0f, 0.0f, 0.0f));
	tex_cord.push_back(float3(1.0f, 0.0f, 0.0f));
	tex_cord.push_back(float3(1.0f, 1.0f, 0.0f));
	tex_cord.push_back(float3(0.0f, 1.0f, 0.0f));
	ProcesQuad(position, tex_cord);
	

}
void CompGraphic::ExpandMesh()
{
}

void CompGraphic::GenerateMesh()
{
	if (my_canvas_render == nullptr)
		return;
	vertex_data.CleanUp();
	ExpandMesh();

	(vertex_data.current_vertex_count == 0) ? invalid = true : invalid = false;

	if (vertex_data.current_vertex_count == 0)
	{
		return;
	}
	my_canvas_render->SetVertex(vertex_data);
}

bool CompGraphic::CheckRender()
{
	if(invalid|| !IsActive())
		return false;
	return can_draw;
}

void CompGraphic::DeviceCheck()
{
}
