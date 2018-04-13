#ifndef COMPONENT_CANVAS_RENDER_H
#define COMPONENT_CANVAS_RENDER_H
#include "Component.h"
#include <vector>
#include "Math\float2.h"
#include "Math\float3.h"
#include "CompImage.h"
class CompText;
class CompImage;
class CompGraphic;
class CompCanvas;
class VertexUIData;
enum FillMethod;

struct CanvasVertex
{
	float3 position;
	float2 tex_coords;
};
class CompCanvasRender:public Component
{
public:
	CompCanvasRender(Comp_Type t, GameObject* parent);
	CompCanvasRender(const CompCanvasRender& copy, GameObject* parent);
	~CompCanvasRender();
	void Update(float dt);
	void Clear();

	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCanvasRender * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);
	void SyncComponent(GameObject* sync_parent);

	void DrawGraphic();

	void SetGraphic(CompGraphic* set_graphic);
	void SetVertex(VertexUIData& data);
	void CheckOpenGlError(std::string info);
	
private:
public:
private:
	CompGraphic* graphic = nullptr;
	std::vector<VertexUI> vertices;
	std::vector<uint> indices;

	uint vertices_id = 0;		/* VERTICES ID */
	uint indices_id = 0;		/* INDICES ID */
	//uint vertices_norm_id = 0;	/* NORMALS OF VERTICES ID */
};

#endif // !COMPONENT_CANVAS_RENDER_H
