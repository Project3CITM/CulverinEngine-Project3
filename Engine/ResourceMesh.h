#ifndef _RESOURCEMESH_
#define _RESOURCEMESH_

#include "Resource_.h"
#include "Math\float4x4.h"
#include "Math/float3.h"
#include "Math/float2.h"

struct ImportBone;

struct Vertex
{
	float3 pos;
	float3 norm;
	float2 texCoords;
};

struct SkeletonSource
{
	float4x4 transform;
	uint num_bones = 0;
	ImportBone* bones = nullptr;
	char* bone_hirarchy_names = nullptr;
	uint* bone_hirarchy_num_childs = nullptr;
};

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint uid);
	virtual ~ResourceMesh();

	void Init(const float3* vert, const uint* ind, const float3* vert_normals, const float2* texCoord);
	void InitRanges(uint num_vert, uint num_ind, uint num_normals);
	void InitInfo(const char* name, const char* path);

	void DeleteToMemory();
	bool LoadToMemory();
	Resource::State IsLoadedToMemory();

	bool HasSkeleton() const;
	void SetSkeleton(SkeletonSource* skeleton);

public:
	bool has_normals = false;
	uint num_vertices = 0;
	uint num_indices = 0;
	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	std::vector<float3> vertices_normals;

	//std::vector<FaceCenter> face_centers;

	//uint VAO = 0;				/* Vertex Array Object */
	uint vertices_id = 0;		/* VERTICES ID */
	uint indices_id = 0;		/* INDICES ID */
	uint vertices_norm_id = 0;	/* NORMALS OF VERTICES ID */
	uint id_total_buffer;		/* MESH INFO BUFFER ID */

	SkeletonSource * skeleton = nullptr;

private:
};

#endif