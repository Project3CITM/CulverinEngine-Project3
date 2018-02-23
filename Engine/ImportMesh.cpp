#include "ModuleImporter.h"

#include "ImportMesh.h"
#include "ModuleFS.h"
#include "GameObject.h"
#include "ImportMaterial.h"
#include "CompMesh.h"
#include "ModuleResourceManager.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "CompMaterial.h"
#include "CompTransform.h"
#include "ModuleTextures.h"
#include "ModuleRenderer3D.h"
#include <filesystem>
#include <iostream>
#include <experimental/filesystem>
#include <fstream>
#include <queue>
#include <algorithm>

ImportMesh::ImportMesh()
{}

ImportMesh::~ImportMesh()
{}

bool ImportMesh::Load(const char* exported_file, Texture* resource)
{
	return false;
}

// Import Mesh -----------------------------------------------------------------------------------------------------------------------------
bool ImportMesh::Import(const aiScene* scene, const aiMesh* mesh, GameObject* obj, const char* name, const char* file, uint uuid)
{
	bool ret = true;
	uint num_vertices = 0;
	uint num_indices = 0;
	uint num_normals = 0;
	uint num_textures = 0;

	float3* vertices = nullptr;
	uint* indices = nullptr;
	float3* vert_normals = nullptr;
	float2* tex_coords = nullptr;

	//Skeleton
	uint num_bones = 0;
	uint total_names_size = 0;
	uint total_weights = 0;

	aiMatrix4x4* skeleton_transform = nullptr;

	float4x4* bone_offsets = nullptr;
	char** bone_names = nullptr;
	uint* bone_name_sizes = nullptr;

	ImportBone::Weight** weights = nullptr;
	uint* num_weights = nullptr;
	aiMatrix4x4* bone_hirarchy_local_transforms = nullptr;
	char* bone_hirarchy_names = nullptr;
	uint* bone_hirarchy_num_childs = nullptr;
	std::vector<std::vector<std::pair<uint,float>>> vertex_weights;
	//--

	for (uint i = 0; i < mesh->mNumFaces; i++)
	{
		if (mesh->mFaces[i].mNumIndices != 3)
		{
			return false;
		}
	}

	CompMesh* meshComp = (CompMesh*)obj->AddComponent(C_MESH);
	//ResourceMesh* resourceMesh = App->resource_manager->CreateNewResource(Resource::Type::MESH);

	if (mesh != nullptr)
	{
		LOG("Importing Mesh %s", name);
		
		// SET VERTEX DATA -------------------------------
		num_vertices = mesh->mNumVertices;
		vertices = new float3[num_vertices];
		memcpy(vertices, mesh->mVertices, sizeof(float3) * num_vertices);
		LOG("- Imported all vertex from data, total vertex: %i", num_vertices);

		// SET INDEX DATA -----------------------------------------
		if (mesh->HasFaces())
		{
			num_indices = mesh->mNumFaces * 3;
			indices = new uint[num_indices];
			for (uint i = 0; i < mesh->mNumFaces; i++)
			{
				if (mesh->mFaces[i].mNumIndices != 3)
				{
					LOG("WARNING, Geometry face with != 3 indices!");
					return false;
				}
				else
				{
					memcpy(&indices[i * 3], mesh->mFaces[i].mIndices, sizeof(uint) * 3);
				}
			}
			LOG("- Imported all index from data, total indices: %i", num_indices);
		}
		else
		{
			LOG("- Mesh %s does not have Faces", mesh->mName.C_Str());
		}

		// SET NORMAL DATA -------------------------------
		if (mesh->HasNormals())
		{
			num_normals = num_vertices;
			vert_normals = new float3[num_normals];
			memcpy(vert_normals, mesh->mNormals, sizeof(float3) * num_normals);
			LOG("- Imported all Normals from data");
		}
		else
		{
			num_normals = 0;
			LOG("- Mesh %s does not have Normals", mesh->mName.C_Str());
		}

		// SET TEX COORD DATA -------------------------------
		//num_tex_coords = num_vertices;
		tex_coords = new float2[num_vertices];
		if (mesh->mTextureCoords[0])
		{
			for (uint i = 0; i < num_vertices; i++)
			{
				tex_coords[i].x = mesh->mTextureCoords[0][i].x;
				tex_coords[i].y = mesh->mTextureCoords[0][i].y;
			}
			//memcpy(tex_coords, mesh->mTextureCoords[0], sizeof(float2) * num_tex_coords);
			LOG("- Imported all Tex Coords from data");
		}
		else
		{
			for (uint i = 0; i < num_vertices; i++)
			{
				tex_coords[i] = float2(0, 0);//TODO-> change default value
			}

			LOG("- Mesh %s does not have Tex Coords", mesh->mName.C_Str());
		}

		// Skeleton
		if (mesh->HasBones())
		{
			//must change into list and hirarchy separately
			num_bones = mesh->mNumBones;
			bone_offsets = new float4x4[num_bones];
			bone_names = new char* [num_bones];
			bone_name_sizes = new uint[num_bones];
			num_weights = new uint[num_bones];
			weights = new ImportBone::Weight*[num_bones];
			vertex_weights.resize(num_vertices * 4);

			for(int i = 0; i < num_bones; i++)
			{
				bone_name_sizes[i] = mesh->mBones[i]->mName.length + 1;
				total_names_size += bone_name_sizes[i];
				bone_names[i] = new char[bone_name_sizes[i]];
				memcpy(bone_names[i], mesh->mBones[i]->mName.C_Str(), bone_name_sizes[i]);

				memcpy(&bone_offsets[i][0][0], &mesh->mBones[i]->mOffsetMatrix.a1, sizeof(float) * 4 * 4);

				num_weights[i] = mesh->mBones[i]->mNumWeights;

				weights[i] = new ImportBone::Weight[num_weights[i]];
				total_weights += num_weights[i];

				for (int r = 0; r < mesh->mBones[i]->mNumWeights; ++r)
				{
					weights[i][r].weight = mesh->mBones[i]->mWeights[r].mWeight;
					weights[i][r].vertex_id = mesh->mBones[i]->mWeights[r].mVertexId;

					vertex_weights[mesh->mBones[i]->mWeights[r].mVertexId].push_back(std::make_pair(i, mesh->mBones[i]->mWeights[r].mWeight));
				}				
			}

			aiNode* skeleton_root = FindSkeletonRootNode(scene, mesh);
			aiNode* mesh_node = FindMeshNode(scene, mesh);

			aiMatrix4x4 mesh_global = GetGlobalTransform(mesh_node);
			aiMatrix4x4 skeleton_root_global = GetGlobalTransform(skeleton_root->mParent);
			
			skeleton_transform = new aiMatrix4x4(mesh_global.Inverse() * skeleton_root_global);
			
			bone_hirarchy_names = new char[total_names_size];
			bone_hirarchy_num_childs = new uint[num_bones];
			bone_hirarchy_local_transforms = new aiMatrix4x4[num_bones];

			uint name_iterator = 0;
			uint joints_saved = 0;

			ImportBoneHirarchy(scene->mRootNode, mesh, bone_hirarchy_local_transforms, bone_hirarchy_names, bone_hirarchy_num_childs, name_iterator, joints_saved);

			for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
			{
				if (IsInSkeletonBranch(scene->mRootNode->mChildren[i], mesh, skeleton_root))
				{
					scene->mRootNode->mChildren[i] = nullptr;
					scene->mRootNode->mNumChildren--;

					for(int j = i; j < scene->mRootNode->mNumChildren; j++)
						scene->mRootNode->mChildren[i] = scene->mRootNode->mChildren[j + 1];
				}
			}

			if (joints_saved != num_bones)
			{
				LOG("Imported joints and Num joints do not mach");
			}
			else
			{
				LOG("Imported joints correctly");
			}
		}
		else
		{
			LOG("- Mesh %s does not have Bones", mesh->mName.C_Str());
		}

		LOG("Imported all data");
	}
	else
	{
		LOG("Can't Import Mesh");
		ret = false;
	}
	
	// SET MATERIAL DATA -----------------------------------------
	if (mesh->mMaterialIndex >= 0)
	{
		CompMaterial* materialComp = (CompMaterial*)obj->AddComponent(C_MATERIAL);
		//
		//std::vector<Texture> text_t;
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

		for (uint i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++)
		{
			aiString str;
			mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
			std::string normalPath = str.C_Str();
			normalPath = App->fs->FixName_directory(normalPath);
			ResourceMaterial* resource_mat = (ResourceMaterial*)App->resource_manager->GetResource(normalPath.c_str());
			if (resource_mat != nullptr)
			{
				if (resource_mat->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					std::string temp = std::to_string(resource_mat->GetUUID());
					App->importer->iMaterial->LoadResource(temp.c_str(), resource_mat);
				}
				std::string name = App->fs->GetOnlyName(normalPath);

				bool exists = false;
				for (auto item = App->module_shaders->materials.begin(); item < App->module_shaders->materials.end(); item++)
				{
					if (strcmp((*item)->name.c_str(), name.c_str()) == 0)
					{
						materialComp->material = (*item);
						exists = true;
						break;
					}
				}
				if (!exists)
				{
					Material* new_mat = new Material();
					new_mat->name = name;
					
					App->module_shaders->materials.push_back(new_mat);
					new_mat->material_shader = App->renderer3D->default_shader;
					new_mat->GetProgramVariables();
					new_mat->path = normalPath;
					if (new_mat->textures.size() > 0)
						new_mat->textures[0].value = resource_mat;
					materialComp->material = new_mat;
					
				}

			
				resource_mat->path_assets = normalPath;
			}
		}
	}
	
	meshComp->Enable();
	// Create Resource ----------------------
	uint uuid_mesh = 0;
	if (uuid == 0)
	{
		uuid_mesh = App->random->Int();
	}
	else
	{
		uuid_mesh = uuid;
	}
	ResourceMesh* res_mesh = (ResourceMesh*)App->resource_manager->CreateNewResource(Resource::Type::MESH, uuid_mesh);
	meshComp->SetResource(res_mesh, true);

	// ALLOCATING DATA INTO BUFFER ------------------------
	uint ranges[4] = { num_vertices, num_indices, num_normals, num_bones}; //,num_tex_coords };

	uint size = sizeof(ranges) + sizeof(float3) *  num_vertices + sizeof(uint) * num_indices + sizeof(float3) *  num_normals + sizeof(float2) *  num_vertices;
	
	if (mesh->HasBones())
	{
		//Skeleton
		size += sizeof(float4x4) * num_bones; //bone offsets
		size += total_names_size; //bone names
		size += sizeof(uint)* num_bones; //num name sizes
		size += sizeof(uint) * num_bones; // num_weights
		size += sizeof(ImportBone::Weight) * total_weights; //weights
		size += sizeof(float) * 4 * 4; //skeleton transform
		size += sizeof(float) * 4 * 4 * num_bones; //bone hyrarchy local transforms
		size += total_names_size; //bone hyrarchy names
		size += sizeof(uint); //names_sizes
		size += sizeof(std::pair<uint,float>) * total_weights; //weights per vertex
		size += sizeof(uint) * num_vertices; //weight vec sizes
		size += sizeof(uint) * num_bones; //bone hyrarchy name sizes
		size += sizeof(uint) * num_bones; //bone num childs
		//--
	}

	// Allocating all data 
	char* data = new char[size];
	char* cursor = data;

	// Storing Ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);

	// Storing Vertices
	cursor += bytes;
	bytes = sizeof(float3) * num_vertices;
	memcpy(cursor, vertices, bytes);

	// Storing Indices
	cursor += bytes;
	bytes = sizeof(uint) * num_indices;
	memcpy(cursor, indices, bytes);

	// Storing Normals
	cursor += bytes;
	bytes = sizeof(float3) * num_normals;
	memcpy(cursor, vert_normals, bytes);

	// Storing Tex Coords
	cursor += bytes;
	bytes = sizeof(float2) * num_vertices; //num_tex_coords;
	memcpy(cursor, tex_coords, bytes);

	//Skeleton
	if (mesh->HasBones())
	{
		//Bone offsets
		cursor += bytes;
		bytes = sizeof(float4x4) * num_bones;
		memcpy(cursor, &bone_offsets[0], bytes);

		//Bone name sizes
		cursor += bytes;
		bytes = sizeof(uint) * num_bones;
		memcpy(cursor, bone_name_sizes, bytes);

		//Bone names
		for (int i = 0; i < num_bones; i++)
		{
			cursor += bytes;
			bytes = bone_name_sizes[i];
			memcpy(cursor, bone_names[i], bytes);
		}

		//Bone weight num
		cursor += bytes;
		bytes = sizeof(uint) * num_bones;
		memcpy(cursor, num_weights, bytes);

		//Weights per vertex
		for (int i = 0; i < num_vertices; i++)
		{
			cursor += bytes;
			bytes = sizeof(uint);
			uint size = vertex_weights[i].size();
			memcpy(cursor, &size, bytes);

			cursor += bytes;
			bytes = sizeof(std::pair<uint,float>) * vertex_weights[i].size();
			memcpy(cursor, vertex_weights[i].data(), bytes);
		}

		//Bone weights
		for (int i = 0; i < num_bones; i++)
		{
			cursor += bytes;
			bytes = sizeof(ImportBone::Weight) * num_weights[i];
			memcpy(cursor, weights[i], bytes);
		}

		//Skeleton transform
		cursor += bytes;
		bytes = sizeof(float) * 4 * 4;
		memcpy(cursor, &skeleton_transform->a1, bytes);

		//Bone local transforms
		cursor += bytes;
		bytes = sizeof(float) * 4 * 4 * num_bones;
		memcpy(cursor, bone_hirarchy_local_transforms, bytes);

		//total bone_names_size
		cursor += bytes;
		bytes = sizeof(uint);
		memcpy(cursor, &total_names_size, bytes);

		//Bone num childs
		cursor += bytes;
		bytes = sizeof(uint) * num_bones;
		memcpy(cursor, bone_hirarchy_num_childs, bytes);

		//Bone hirarchy names
		cursor += bytes;
		bytes = total_names_size;
		memcpy(cursor, bone_hirarchy_names, bytes);

		RELEASE_ARRAY(bone_offsets);
		RELEASE_ARRAY(bone_name_sizes);
		RELEASE_ARRAY(bone_names);
		RELEASE(skeleton_transform);
		RELEASE_ARRAY(bone_hirarchy_names);
		RELEASE_ARRAY(bone_hirarchy_num_childs);
	}
	//--

	// Release all pointers
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(vert_normals);
	RELEASE_ARRAY(tex_coords);
	//RELEASE(texture);

	// Set Info ResoruceMesh
	std::string fileName = std::to_string(uuid_mesh);
	res_mesh->InitInfo(name, file);
	//std::string path_assets = name;
	//res_mesh->path_assets = App->fs->GetFullPath(path_assets);

	//Save Mesh
	App->fs->SaveFile(data, fileName, size, IMPORT_DIRECTORY_LIBRARY_MESHES);

	RELEASE_ARRAY(data);

	return ret;
}

// Import Primitive -----------------------------------------------------------------------------------------------------------------------------
void ImportMesh::Import(uint num_vertices, uint num_indices, uint num_normals, uint num_bones, std::vector<uint> indices, std::vector<float3> vertices, const char* name, uint uuid)
{
	// ALLOCATING DATA INTO BUFFER ------------------------
	uint ranges[4] = { num_vertices, num_indices, num_normals, num_bones}; //,num_tex_coords };

	uint size = sizeof(ranges) + sizeof(float3) *  num_vertices + sizeof(uint) * num_indices + sizeof(float3) *  num_normals + sizeof(float2) *  num_vertices + sizeof(uint) * num_bones;

	float3* vert_normals = nullptr;
	// Allocating all data 
	char* data = new char[size];
	char* cursor = data;

	// Storing Ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);

	// Storing Vertices
	float3* vertices_ = new float3[num_vertices];
	memcpy(vertices_, vertices.data(), sizeof(float3) * num_vertices);
	cursor += bytes;
	bytes = sizeof(float3) * num_vertices;
	memcpy(cursor, vertices_, bytes);

	// Storing Indices
	uint* indices_ = new uint[num_indices];
	memcpy(indices_, indices.data(), sizeof(uint) * num_indices);
	cursor += bytes;
	bytes = sizeof(uint) * num_indices;
	memcpy(cursor, indices_, bytes);

	// Release all pointers
	RELEASE_ARRAY(vertices_);
	RELEASE_ARRAY(indices_);
	RELEASE_ARRAY(vert_normals);

	// Create Resource ----------------------
	ResourceMesh* res_mesh = (ResourceMesh*)App->resource_manager->CreateNewResource(Resource::Type::MESH, uuid);
	// Set info
	res_mesh->num_indices = num_indices;
	res_mesh->num_vertices = num_vertices;
	if (vertices.size()) {

		for (int i = 0; i < num_vertices; i++) {
			Vertex vert;
			vert.pos = vertices[i];
			res_mesh->vertices.push_back(vert);
		}
		res_mesh->indices = indices;
	}
	std::string fileName = std::to_string(uuid);
	res_mesh->InitInfo(name, "");

	// Save Mesh
	App->fs->SaveFile(data, fileName, size, IMPORT_DIRECTORY_LIBRARY_MESHES);

	indices.clear();
	vertices.clear();

	RELEASE_ARRAY(data);
}

bool ImportMesh::LoadResource(const char* file, ResourceMesh* resourceMesh)
{
	char* buffer = nullptr;
	uint num_vertices = 0;
	uint num_indices = 0;
	uint num_normals = 0;
	//uint num_tex_coords = 0;

	float3* vertices = nullptr;
	uint* indices = nullptr;
	float3* vert_normals = nullptr;
	float2* tex_coords = nullptr;
	//Texture* texture = nullptr;

	//Skeleton
	uint num_bones;
	//--

	// Loading File
	uint size = App->fs->LoadFile(file, &buffer, IMPORT_DIRECTORY_LIBRARY_MESHES);

	if (buffer != nullptr && size > 0)
	{
		char* cursor = buffer;

		// Amount vertices, amount indices, amount normals
		uint ranges[4];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);

		// Set Amounts
		num_vertices = ranges[0];
		num_indices = ranges[1];
		num_normals = ranges[2];
		//num_tex_coords = ranges[3];
		num_bones = ranges[3];

		//Load Vertices
		cursor += bytes;
		bytes = sizeof(float3) * num_vertices;
		vertices = new float3[num_vertices];
		memcpy(vertices, cursor, bytes);

		//Load Indices
		cursor += bytes;
		bytes = sizeof(uint) * num_indices;
		indices = new uint[num_indices];
		memcpy(indices, cursor, bytes);

		//Load Normals
		if (num_normals > 0)
		{
			cursor += bytes;
			bytes = sizeof(float3) * num_normals;
			vert_normals = new float3[num_normals];
			memcpy(vert_normals, cursor, bytes);
		}

		//Load Tex Coords
		cursor += bytes;
		bytes = sizeof(float2) * num_vertices; //num_tex_coords;
		tex_coords = new float2[num_vertices];
		memcpy(tex_coords, cursor, bytes);

		//Skeleton
		if (num_bones > 0)
		{
			SkeletonSource* skeleton_source = new SkeletonSource;

			skeleton_source->num_bones = num_bones;
			skeleton_source->bones.resize(num_bones);

			//Bone names
			cursor += bytes;
			bytes = sizeof(float4x4) * num_bones;
			float4x4* bone_offsets = new float4x4[num_bones];
			memcpy(&bone_offsets[0], cursor, bytes);

			//Bone name sizes
			cursor += bytes;
			bytes = sizeof(uint) * num_bones;
			uint* bone_name_sizes = new uint[num_bones];
			memcpy(bone_name_sizes, cursor, bytes);

			//Bone names
			for (int i = 0; i < num_bones; i++)
			{
				cursor += bytes;
				skeleton_source->bones[i].name = cursor;
				bytes = bone_name_sizes[i];

				skeleton_source->bones[i].offset = bone_offsets[i];
			}		

			//Bone weight num
			cursor += bytes;
			bytes = sizeof(uint) * num_bones;
			uint* num_weights = new uint[num_bones];
			memcpy(num_weights, cursor, bytes);

			//Weights per vertex
			skeleton_source->vertex_weights.resize(num_vertices);
			for (int i = 0; i < num_vertices; i++)
			{
				cursor += bytes;
				bytes = sizeof(uint);
				uint size;
				memcpy(&size, cursor, bytes);

				skeleton_source->vertex_weights[i].resize(size);
				cursor += bytes;
				bytes = sizeof(std::pair<uint, float>) * size;
				memcpy(skeleton_source->vertex_weights[i].data(), cursor, bytes);
			}

			//Bone weights
			for (int i = 0; i < num_bones; i++)
			{
				cursor += bytes;
				bytes = sizeof(ImportBone::Weight) * num_weights[i];
				skeleton_source->bones[i].num_weights = num_weights[i];
				skeleton_source->bones[i].weights = new ImportBone::Weight [num_weights[i]];
				memcpy(skeleton_source->bones[i].weights, cursor, bytes);
			}

			//Skeleton transform
			cursor += bytes;
			bytes = sizeof(float) * 4;
			for (int i = 0; i < 4; i++)
			{
				memcpy(&skeleton_source->transform[i][0], cursor, bytes);
				cursor += bytes;
			}

			//Bone local transforms
			bytes = sizeof(float) * 4 * 4 * num_bones;
			skeleton_source->bone_hirarchy_local_transforms.resize(num_bones);
			memcpy(skeleton_source->bone_hirarchy_local_transforms.data(), cursor, bytes);

			//Bone hirarchy names total_size
			cursor += bytes;
			bytes = sizeof(uint);
			uint bone_names_size;
			memcpy(&bone_names_size, cursor, bytes);

			//Bone num childs
			cursor += bytes;
			bytes = sizeof(uint) * num_bones;
			skeleton_source->bone_hirarchy_num_childs.resize(num_bones);
			memcpy(skeleton_source->bone_hirarchy_num_childs.data(), cursor, bytes);

			//Bone hirarchy names
			cursor += bytes;
			bytes = bone_names_size;
			skeleton_source->bone_hirarchy_names = new char[bone_names_size];
			memcpy(skeleton_source->bone_hirarchy_names, cursor, bytes);

			resourceMesh->SetSkeleton(skeleton_source);
		}
		//--

		resourceMesh->InitRanges(num_vertices, num_indices, num_normals);
		resourceMesh->Init(vertices, indices, vert_normals, tex_coords);
		resourceMesh->LoadToMemory();
		RELEASE_ARRAY(vertices);
		RELEASE_ARRAY(indices);
		RELEASE_ARRAY(vert_normals);
		RELEASE_ARRAY(tex_coords);
		//RELEASE_ARRAY(cursor);
		LOG("Mesh %s Loaded!", file);
	}
	RELEASE_ARRAY(buffer);
	return true;
}

void ImportMesh::ImportBoneHirarchy(aiNode* node, const aiMesh* mesh, aiMatrix4x4* bone_hirarchy_local_transforms, char* bone_hirarchy_names, uint* bone_hirarchy_num_childs, uint& name_iterator, uint& joints_saved)
{
	bool is_bone = false;

	for (int i = 0; i < mesh->mNumBones; i++)
	{
		if (node->mName == mesh->mBones[i]->mName)
			is_bone = true;
	}

	if (is_bone)
	{
		memcpy(&bone_hirarchy_names[name_iterator], node->mName.C_Str(), node->mName.length + 1);
		name_iterator += node->mName.length + 1;
		memcpy(&bone_hirarchy_local_transforms[joints_saved].a1, &node->mTransformation.a1, sizeof(float) * 4 * 4);

		uint num_childs = 0;

		for (int i = 0; i < node->mNumChildren; i++)
		{
			for (int i = 0; i < mesh->mNumBones; i++)
			{
				if (node->mName == mesh->mBones[i]->mName)
					num_childs++;
			}
		}

		bone_hirarchy_num_childs[joints_saved] = num_childs;

		joints_saved++;
	}

	for (int i = 0; i < node->mNumChildren; i++)
		ImportBoneHirarchy(node->mChildren[i], mesh, bone_hirarchy_local_transforms, bone_hirarchy_names, bone_hirarchy_num_childs, name_iterator, joints_saved);
}

aiNode * ImportMesh::FindSkeletonRootNode(const aiScene * scene, const aiMesh* mesh)
{
	aiNode* ret = nullptr;

	for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
	{
		ret = GetSkeletonRoot(scene->mRootNode->mChildren[i], mesh);
		if (ret != nullptr)
			return ret;
	}

	return ret;
}

aiNode * ImportMesh::FindMeshNode(const aiScene * scene, const aiMesh * mesh)
{
	aiNode* ret = nullptr;

	for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
	{
		ret = GetMeshNode(scene->mRootNode->mChildren[i], mesh, scene);
		if (ret != nullptr)
			return ret;
	}

	return ret;
}

aiNode * ImportMesh::GetSkeletonRoot(aiNode * node, const aiMesh * mesh)
{
	for (int i = 0; i < mesh->mNumBones; i++)
		if (node->mName == mesh->mBones[i]->mName)
			return node;

	aiNode* ret = nullptr;

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ret = GetSkeletonRoot(node->mChildren[i], mesh);
		if (ret != nullptr)
			break;
	}

	return ret;
}

aiNode * ImportMesh::GetMeshNode(aiNode * node, const aiMesh * mesh, const aiScene * scene)
{
	bool is_joint = false;

	for (int i = 0; i < node->mNumMeshes; i++)
		if (scene->mMeshes[node->mMeshes[i]] == mesh)
			return node;

	aiNode* ret = nullptr;

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ret = GetMeshNode(node->mChildren[i], mesh, scene);
		if (ret != nullptr)
			break;
	}

	return ret;
}

bool ImportMesh::IsInSkeletonBranch(const aiNode * node, const aiMesh* mesh, const aiNode * skeleton_root)
{
	bool ret = false;

	for (int i = 0; i < node->mNumChildren; i++)
		if(node->mChildren[i] == skeleton_root)

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ret = IsInSkeletonBranch(node->mChildren[i], mesh, skeleton_root);
		if (ret == true)
			break;
	}
	
	return ret;
}

aiMatrix4x4 ImportMesh::GetGlobalTransform(const aiNode * node)
{
	if (node->mParent == nullptr)
		return node->mTransformation;

	aiMatrix4x4 transform = node->mTransformation;
	const aiNode* iterator = node->mParent;

	while (iterator->mParent != nullptr)
	{
		transform = iterator->mTransformation * transform;
		iterator = iterator->mParent;
	}

	transform = iterator->mTransformation * transform;

	return transform;
}
