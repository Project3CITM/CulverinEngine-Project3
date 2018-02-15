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

	float4x4* bone_offsets = nullptr;
	char** bone_names = nullptr;
	uint* bone_name_sizes = nullptr;

	ImportBone::Weight** weights = nullptr;
	uint* num_weights = nullptr;
	char* bone_hirarchy_names = nullptr;
	uint* bone_hirarchy_name_sizes = nullptr;
	uint* bone_hirarchy_num_childs = nullptr;
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
					weights[i][r].weight = mesh->mBones[i]->mWeights[r].mVertexId;
					weights[i][r].vertex_id = mesh->mBones[i]->mWeights[r].mVertexId;
				}				
			}

			std::queue<aiNode*> nodes;
			bone_hirarchy_name_sizes = new uint[num_bones];
			bone_hirarchy_names = new char[total_names_size];
			bone_hirarchy_num_childs = new uint[num_bones];

			uint joints_saved = 0;
			uint name_iterator = 0;

			nodes.push(scene->mRootNode);

			while (nodes.size() > 0)
			{
				aiNode* current_node = nodes.front();

				bool is_joint = false;

				for (int i = 0; i < mesh->mNumBones; i++)
					if (current_node->mName == mesh->mBones[i]->mName)
					{
						is_joint = true;
						break;
					}

				if (is_joint == true)
				{
					bone_hirarchy_name_sizes[joints_saved] = current_node->mName.length + 1;
					memcpy(&bone_hirarchy_names[name_iterator], current_node->mName.C_Str(), bone_hirarchy_name_sizes[joints_saved]);
					name_iterator += bone_hirarchy_name_sizes[joints_saved];
					bone_hirarchy_num_childs[joints_saved] = current_node->mNumChildren;

					joints_saved++;
				}

				for (int i = 0; i < current_node->mNumChildren; i++)
					nodes.push(current_node->mChildren[i]);

				nodes.pop();
			}

			if (joints_saved == mesh->mNumBones)
			{
				LOG("- Imported all Bones from data");
			}
			else
			{
				LOG("Imported joints and Num joints do not mach");
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
				materialComp->material_shader = *App->renderer3D->default_shader;
				materialComp->material_shader.textures[0].res_material = resource_mat;
				
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
	
	//Skeleton
	size += sizeof(float4x4) * num_bones; //bone offsets
	size += total_names_size; //bone names
	size += sizeof(uint)* num_bones; //num name sizes
	size += sizeof(uint) * num_bones; // num_weights
	size += sizeof(ImportBone::Weight) * total_weights; //weights
	size += total_names_size; //bone hyrarchy names
	size += sizeof(uint); //names_sizes
	size += sizeof(uint) * num_bones; //bone hyrarchy name sizes
	size += sizeof(uint) * num_bones; //bone num childs
	//--

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

		//Bone weights
		for (int i = 0; i < num_bones; i++)
		{
			cursor += bytes;
			bytes = sizeof(ImportBone::Weight) * num_weights[i];
			memcpy(cursor, weights[i], bytes);
		}

		//Bone hirarchy name sizes
		cursor += bytes;
		bytes = sizeof(uint) * num_bones;
		memcpy(cursor, bone_hirarchy_name_sizes, bytes);

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
		RELEASE_ARRAY(bone_hirarchy_names);
		RELEASE_ARRAY(bone_hirarchy_name_sizes);
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
void ImportMesh::Import(uint num_vertices, uint num_indices, uint num_normals, uint num_bones, std::vector<uint> indices, std::vector<float3> vertices, uint uuid)
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
	indices.clear();
	vertices.clear();

	// Create Resource ----------------------
	ResourceMesh* res_mesh = (ResourceMesh*)App->resource_manager->CreateNewResource(Resource::Type::MESH, uuid);
	// Set info
	std::string fileName = std::to_string(uuid);
	res_mesh->InitInfo("Cube", "");

	// Save Mesh
	App->fs->SaveFile(data, fileName, size, IMPORT_DIRECTORY_LIBRARY_MESHES);

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
			skeleton_source->bones = new ImportBone[num_bones];

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

			//Bone weights
			for (int i = 0; i < num_bones; i++)
			{
				cursor += bytes;
				bytes = sizeof(ImportBone::Weight) * num_weights[i];
				skeleton_source->bones[i].num_weights = num_weights[i];
				skeleton_source->bones[i].weights = new ImportBone::Weight [num_weights[i]];
				memcpy(skeleton_source->bones[i].weights, cursor, bytes);
			}

			//Bone name sizes
			cursor += bytes;
			bytes = sizeof(uint) * num_bones;
			skeleton_source->bone_hirarchy_name_sizes = new uint[num_bones];
			memcpy(skeleton_source->bone_hirarchy_name_sizes, cursor, bytes);

			//Bone hirarchy names total_size
			cursor += bytes;
			bytes = sizeof(uint);
			uint bone_names_size = *((uint*)cursor);

			//Bone num childs
			cursor += bytes;
			bytes = sizeof(uint) * num_bones;
			skeleton_source->bone_hirarchy_num_childs = new uint[num_bones];
			memcpy(skeleton_source->bone_hirarchy_num_childs, cursor, bytes);

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