#ifndef _IMPORT_MESH_
#define _IMPORT_MESH_

#include "Module.h"
#include "Application.h"
#include "ModuleImporter.h"

struct Texture;
class ResourceMesh;

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
	void ImportBoneHirarchy(aiNode* node, const aiMesh* mesh, aiMatrix4x4* bone_hirarchy_local_transforms, char* bone_hirarchy_names, uint* bone_hirarchy_num_childs, uint& name_iterator, uint& joints_saved);
	aiNode* FindSkeletonRootNode(const aiScene* scene, const aiMesh* mesh);
	aiNode* FindMeshNode(const aiScene* scene, const aiMesh* mesh);
	aiNode* GetSkeletonRoot(aiNode* node, const aiMesh* mesh);
	aiNode* GetMeshNode(aiNode* node, const aiMesh* mesh, const aiScene * scene);
	bool IsInSkeletonBranch(const aiNode* node, const aiMesh* mesh, const aiNode * skeleton_root);

	aiMatrix4x4 GetGlobalTransform(const aiNode* node);
};

#endif
