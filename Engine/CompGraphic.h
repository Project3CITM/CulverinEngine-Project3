#ifndef COMPONENT_GRAPHIC_H
#define COMPONENT_GRAPHIC_H
#include "Component.h"
#include "Math\float4.h"
#include "VertexUIData.h"
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
	void DrawRectTranforms();
	void ActivateRender();
	void DeactivateRender();

	void SetNullCanvas();

	void SetTextureID(uint set_texture_id);
	void SetColor(const float4& set_rgba);
	void SetColor(float set_r, float set_g, float set_b, float set_a);
	void SetAlpha(float alpha);
	void SetToRender(bool render);
	void SetCanDraw(bool render);
	void SetRaycastTarget(bool flag);
	void SetInteractive(CompInteractive* set_interactive);
	bool GetToRender()const;
	uint GetTextureID()const;
	CompRectTransform* GetRectTrasnform()const;
	CompCanvasRender* GetCanvasRender()const;
	float4 GetColor()const;
	bool GetParentActive();

	void ResizeGenerateMesh();

private:
protected:
	void ProcesQuad(std::vector<float3>& position, std::vector<float3>& texture_cord);

	void ProcesQuad(std::vector<float3>& position);

	virtual void ExpandMesh();
	void GenerateMesh();
	bool CheckRender();

protected:	
	bool can_draw = true;
	bool invalid = false;

	VertexUIData vertex_data;
	bool render = true;
	bool raycast_target = true;
	CompCanvas* my_canvas = nullptr;
	CompCanvasRender* my_canvas_render = nullptr;
	CompRectTransform* transform = nullptr;
	CompInteractive* interactive = nullptr;
	uint texture_id = 0;
	float4 color = float4::one;

	
};

#endif//COMPONENT_GRAPHIC_H
