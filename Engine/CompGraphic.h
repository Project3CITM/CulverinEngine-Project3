#ifndef COMPONENT_GRAPHIC_H
#define COMPONENT_GRAPHIC_H
#include "Component.h"
#include "Math\float4.h"

class CompRectTransform;
class CompCanvasRender;
class CompGraphic : public Component
{
public:
	CompGraphic(Comp_Type t, GameObject* parent);
	CompGraphic(const CompGraphic& copy, GameObject* parent);
	~CompGraphic();
	void AddCanvasRender();

	void SetTextureID(uint set_texture_id);
	void SetColor(const float4& set_rgba);
	void SetColor(float set_r, float set_g, float set_b, float set_a);
	uint GetTextureID()const;
	CompRectTransform* GetRectTrasnform()const;
	CompCanvasRender* GetCanvasRender()const;
	float4 GetColor()const;

private:
public:
protected:
	CompCanvasRender* my_canvas_render = nullptr;
	CompRectTransform* transform = nullptr;
	uint texture_id = 0;
	float4 color = float4::one;

};

#endif//COMPONENT_GRAPHIC_H
