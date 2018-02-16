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
		Weight() : weight(0.0f), vertex_id(0)
		{}

		float weight;
		uint vertex_id;
	};

	std::string name;
	float4x4 offset;
	uint num_weights;
	Weight* weights;
};

class ImportMesh
{
public:
	ImportMesh();
	~ImportMesh();

	//bool Import(const char* file, std::string& output_file);
	bool Load(const char* exported_file, Texture* resource);

	bool Import(const aiScene * scene, const aiMesh* mesh, GameObject* obj, const char* name, const char* file, uint uuid = 0);
	void Import(uint num_vertices, uint num_indices, uint num_normals, uint num_bones, std::vector<uint> indices, std::vector<float3> vertices, const char* name, uint uid = 0);
	bool LoadResource(const char * file, ResourceMesh* resourceMesh);

private:
	void ImportBoneHirarchy(aiNode* node, const aiMesh* mesh, char* bone_hirarchy_names, uint* bone_hirarchy_num_childs, uint& name_iterator, uint& joints_saved);
};

#endif
