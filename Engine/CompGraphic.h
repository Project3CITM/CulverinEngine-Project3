#ifndef COMPONENT_GRAPHIC_H
#define COMPONENT_GRAPHIC_H
#include "Component.h"
#include "Math\float4.h"

class CompRectTransform;
class CompCanvasRender;
class CompCanvas;
class CompInteractive;
class CompGraphic : public Component
{
public:
	CompGraphic(Comp_Type t, GameObject* parent);
	CompGraphic(const CompGraphic& copy, GameObject* parent);
	~CompGraphic();
	void Clear();
	void AddCanvas();
	void AddCanvasRender();
	void AddRectTransform();
	void SyncComponent(GameObject* sync_parent);
	void DrawGraphic();

	void ActivateRender();
	void DeactivateRender();

	void SetNullCanvas();

	void SetTextureID(uint set_texture_id);
	void SetColor(const float4& set_rgba);
	void SetColor(float set_r, float set_g, float set_b, float set_a);
	void SetToRender(bool render);
	void SetRaycastTarget(bool flag);
	void SetInteractive(CompInteractive* set_interactive);
	bool GetToRender()const;
	uint GetTextureID()const;
	CompRectTransform* GetRectTrasnform()const;
	CompCanvasRender* GetCanvasRender()const;
	float4 GetColor()const;
	bool GetParentActive();

private:
public:
protected:
	bool render = true;
protected:
	bool raycast_target = true;
	CompCanvas* my_canvas = nullptr;
	CompCanvasRender* my_canvas_render = nullptr;
	CompRectTransform* transform = nullptr;
	CompInteractive* interactive = nullptr;
	uint texture_id = 0;
	float4 color = float4::one;

	
};

#endif//COMPONENT_GRAPHIC_H
