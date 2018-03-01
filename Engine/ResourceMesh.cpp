#include "ResourceMesh.h"
#include "Application.h"
#include "Globals.h"


ResourceMesh::ResourceMesh(uint uid) : Resource(uid, Resource::Type::MESH, Resource::State::UNLOADED)
{
	num_game_objects_use_me = 0;
	LOG("Resource Mesh Created!");
}

ResourceMesh::~ResourceMesh()
{
	vertices.clear();
	indices.clear();
	vertices_normals.clear();

	delete skeleton;
}

void ResourceMesh::Init(const float3* vert, const uint* ind, const float3* vert_normals, const float2* texCoord, const float3* tangs, const float3* bitangs)
{
	// SET VERTEX DATA -------------------------------
	for (uint i = 0; i < num_vertices; i++)
	{
		Vertex ver;
		// Vertex Positions ------------------
		ver.pos = vert[i];

		// Vertex Normals --------------------
		if (has_normals)
		{
			ver.norm = vert_normals[i];
		}
		else
		{
			ver.norm.Set(0, 0, 0);
		}

		// Vertex Tex Coords ------------------
		ver.texCoords = texCoord[i];

		vertices.push_back(ver);
	}

	// SET INDEX DATA -----------------------------------------
	for (uint i = 0; i < num_indices; i++)
	{
		indices.push_back(ind[i]);
	}

	//NORMALS ARRAY ---------
	for (int i = 0; i < num_vertices; i++)
	{
		vertices_normals.push_back(float3(vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z));
		vertices_normals.push_back(float3(vertices[i].pos.x + vertices[i].norm.x, vertices[i].pos.y + vertices[i].norm.y, vertices[i].pos.z + vertices[i].norm.z));
	}
	//TANGENTS / BITANGENTS ARRAY -----------------
	for (int i = 0; i < num_vertices; i++)
	{
		tangents.push_back(tangs[i]);
		bitangents.push_back(bitangs[i]);
	}
}

void ResourceMesh::InitRanges(uint num_vert, uint num_ind, uint num_normals)
{
	num_vertices = num_vert;
	num_indices = num_ind;

	if (num_normals > 0)
	{
		has_normals = true;
	}
}

void ResourceMesh::InitInfo(const char* nameResource, const char* path)
{
	name = nameResource;
	path_assets = path;
}

void ResourceMesh::DeleteToMemory()
{
	state = Resource::State::UNLOADED;

	if (vertices_id != NULL)		glDeleteBuffers(1, &vertices_id);
	if (indices_id != NULL)			glDeleteBuffers(1, &indices_id);
	if (vertices_norm_id != NULL)	glDeleteBuffers(1, &vertices_norm_id);

	num_vertices = 0;
	num_indices = 0;
	has_normals = false;

	vertices.clear();
	indices.clear();
	vertices_normals.clear();
	LOG("UnLoaded Resource Mesh");
}

bool ResourceMesh::LoadToMemory()
{
	LOG("Resources: %s, Loaded in Memory!", this->name.c_str());

	//----------Starting to change to one buffer

	//char* total_buffer_mesh = nullptr;
	char* total_buffer_mesh = nullptr;

	int vertex_size_in_buffer = 0;

	if (vertices.size() > 0)
	{
		vertex_size_in_buffer += 3 * sizeof(float);
		vertex_size_in_buffer += 2 * sizeof(float);
	}

	if (vertices_normals.size() > 0)
	{
		vertex_size_in_buffer += 3 * sizeof(float);
		vertex_size_in_buffer += 3 * sizeof(float);
		vertex_size_in_buffer += 3 * sizeof(float);
	}

	if (skeleton != nullptr)
	{
		vertex_size_in_buffer += 4 * sizeof(float);// 4 weights
		vertex_size_in_buffer += 4 * sizeof(float);
	}

	uint total_byte_size = vertex_size_in_buffer * num_vertices;
	total_buffer_mesh = new char[total_byte_size];
	char* cursor = total_buffer_mesh;


	

	for (int i = 0; i < num_vertices; i++)
	{
		if (vertices.size() > 0)
		{
			memcpy(cursor, &vertices[i].pos, 3 * sizeof(float));
			cursor += 3 * sizeof(float);

			memcpy(cursor, &vertices[i].texCoords, 2 * sizeof(float));
			cursor += 2 * sizeof(float);
		}

		if (vertices_normals.size() > 0)
		{
			memcpy(cursor, &vertices[i].norm, 3 * sizeof(float));
			cursor += 3 * sizeof(float);

			memcpy(cursor, &tangents[i], 3 * sizeof(float));
			cursor += 3 * sizeof(float);

			memcpy(cursor, &bitangents[i], 3 * sizeof(float));
			cursor += 3 * sizeof(float);
		}

		if (skeleton != nullptr)
		{
			//weights
			for (std::vector<std::pair<uint, float>>::const_iterator it = skeleton->vertex_weights[i].begin(); it != skeleton->vertex_weights[i].end(); ++it)
			{
				memcpy(cursor, &it->second, sizeof(float));
				cursor += sizeof(float);
			}

			if (skeleton->vertex_weights[i].size() < 4)
			{
				float zero = 0.0f;
				for (int j = 0; j < 4 - skeleton->vertex_weights[i].size(); j++)
				{
					memcpy(cursor, &zero, sizeof(float));
					cursor += sizeof(float);
				}
			}

			//bone IDs
			for (std::vector<std::pair<uint, float>>::const_iterator it = skeleton->vertex_weights[i].begin(); it != skeleton->vertex_weights[i].end(); ++it)
			{
				float tmp = it->first;
				memcpy(cursor, &tmp, sizeof(float));
				cursor += sizeof(float);
			}

			if (skeleton->vertex_weights[i].size() < 4)
			{
				float tmp = -1.00000000;
				for (int j = 0; j < 4 - skeleton->vertex_weights[i].size(); j++)
				{
					memcpy(cursor, &tmp, sizeof(float));
					cursor += sizeof(float);
				}
			}
		}
	}

	glGenBuffers(1, &id_total_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, id_total_buffer);
	glBufferData(GL_ARRAY_BUFFER, total_byte_size, &total_buffer_mesh[0], GL_STATIC_DRAW);

	delete[] total_buffer_mesh;	

	glGenBuffers(1, &indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	state = Resource::State::LOADED;
	return true;
}

Resource::State ResourceMesh::IsLoadedToMemory()
{
	return state;
}

bool ResourceMesh::HasSkeleton() const
{
	return skeleton != nullptr;
}

void ResourceMesh::SetSkeleton(SkeletonSource * skeleton)
{
	this->skeleton = skeleton;
}
