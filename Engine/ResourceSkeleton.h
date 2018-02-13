#ifndef RESOURCE_SKELETON_H
#define RESOURCE_SKELETON_H

#include "Resource_.h"
#include "Math/float4x4.h"
#include <vector>

struct VertexWeight
{
	int vertex_id;
	float weight;
};

struct SkeletonBone
{
	~SkeletonBone();

	std::string name;
	float4x4 offset_matrix;
	std::vector<VertexWeight*> weights;
};

class ResourceSkeleton :public Resource
{
public:
	ResourceSkeleton(uint uid);
	~ResourceSkeleton();

	void InitInfo(const char* name, const char* path);
	bool LoadToMemory();
	void DeleteToMemory();

private:
	std::vector<SkeletonBone*> bones;
};

#endif