#ifndef COMPONENT_CANVAS_RENDER_H
#define COMPONENT_CANVAS_RENDER_H
#include "Component.h"
#include <vector>
#include "Math\float2.h"
#include "Math\float3.h"
class CompText;
class CompImage;
class CompGraphic;
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
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCanvasRender * component);
	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);

	void ProcessImage(CompImage* image);
	void PorcessText(CompText* text);
	void DrawGraphic();
private:
public:
private:
	CompGraphic* graphic = nullptr;
	std::vector<CanvasVertex> vertices;
	std::vector<uint> indices;

	uint vertices_id = 0;		/* VERTICES ID */
	uint indices_id = 0;		/* INDICES ID */
	//uint vertices_norm_id = 0;	/* NORMALS OF VERTICES ID */
	uint id_total_buffer=0; /*MESH INFO BUFFER ID*/
};

#endif // !COMPONENT_CANVAS_RENDER_H
