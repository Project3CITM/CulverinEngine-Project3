#ifndef _IMPORT_MESH_
#define _IMPORT_MESH_

#include "Module.h"
#include "Application.h"
#include "ModuleImporter.h"

struct Texture;
class ResourceMesh;

struct Weight
{
	float weight;
	uint vertex_id;
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
	void ImportBone(const aiNode* node, const aiMesh* mesh, char** joint_names, uint& joint_names_size, aiMatrix4x4* bone_offsets, uint* num_weights, Weight** weights, uint& num_joints) const;

};

#endif
