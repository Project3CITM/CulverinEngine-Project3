#ifndef VERTEX_UI_DATA_H
#define VERTEX_UI_DATA_H
#include <vector>
#include "MathGeoLib.h"
#include "Globals.h"
struct VertexUI
{
	VertexUI(float3 pos, float2 tex_cord);
	float3 position;
	float2 tex_cord;
};
class VertexUIData
{
public:
	VertexUIData();
	~VertexUIData();
	void AddVertex(float3 pos, float2 tex_cord);
	void AddTriangleIndex(uint index01, uint index02, uint index03);
	void CleanUp();
	int GetVertexSize()const;
	int GetIndicesSize()const;

	std::vector<VertexUI> GetVertex();
	std::vector<uint> GetIndices();

	//Don't touch
	uint current_vertex_count = 0;
private:
	std::vector<VertexUI> vertex;
	std::vector<uint> indices;
};

#endif//VERTEX_UI_DATA_H
