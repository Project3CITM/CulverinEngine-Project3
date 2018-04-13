#include "VertexUIData.h"



VertexUIData::VertexUIData()
{
}


VertexUIData::~VertexUIData()
{
}

void VertexUIData::AddVertex(float3 pos, float2 tex_cord)
{
	vertex.push_back(VertexUI(pos, tex_cord));
	current_vertex_count++;
}

void VertexUIData::AddTriangleIndex(uint index01, uint index02, uint index03)
{
	indices.push_back(index01);
	indices.push_back(index02);
	indices.push_back(index03);

}

void VertexUIData::CleanUp()
{
	current_vertex_count = 0;
	vertex.clear();
	indices.clear();
}

int VertexUIData::GetVertexSize() const
{
	return vertex.size();
}

int VertexUIData::GetIndicesSize() const
{
	return indices.size();
}

std::vector<VertexUI> VertexUIData::GetVertex()
{
	return vertex;
}

std::vector<uint> VertexUIData::GetIndices()
{
	return indices;
}

VertexUI::VertexUI(float3 pos, float2 tex_cord):position(pos), tex_cord(tex_cord)
{
}
