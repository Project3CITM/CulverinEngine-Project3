#ifndef _IMPORT_MESH_
#define _IMPORT_MESH_

#include "Module.h"
#include "Application.h"
#include "ModuleImporter.h"

struct Texture;
class ResourceMesh;

struct ImportBone
{
	struct Weight
	{
		Weight(float weight, uint vertex_id) : weight(weight), vertex_id(vertex_id)
		{}

		float weight;
		uint vertex_id;
	};

	uint name_length;
	char* name;
	float4x4 offset;
	uint num_weights;
	std::vector<Weight> weights;
};

class ImportMesh
{
public:
	ImportMesh();
	~ImportMesh();

	//bool Import(const char* file, std::string& output_file);
	bool Load(const char* exported_file, Texture* resource);

	bool Import(const aiScene * scene, const aiMesh* mesh, GameObject* obj, const char* name, const char* file, uint uuid = 0);
	void Import(uint num_vertices, uint num_indices, uint num_normals, std::vector<uint> indices, std::vector<float3> vertices, uint uid = 0);
	bool LoadResource(const char * file, ResourceMesh* resourceMesh);

private:
};

#endif
