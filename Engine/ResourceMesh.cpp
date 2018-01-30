#include "ResourceMesh.h"
#include "Application.h"
#include "Globals.h"

ResourceMesh::ResourceMesh(uint uid) : Resource(uid, Resource::Type::MESH, Resource::State::UNLOADED)
{
	NumGameObjectsUseMe = 0;
	LOG("Resource Mesh Created!");
}

ResourceMesh::~ResourceMesh()
{
	vertices.clear();
	indices.clear();
	vertices_normals.clear();
}

void ResourceMesh::Init(const float3* vert, const uint* ind, const float3* vert_normals, const float2* texCoord)
{
	// SET VERTEX DATA -------------------------------
	for (uint i = 0; i < num_vertices; i++)
	{
		Vertex ver;
		// Vertex Positions ------------------
		ver.pos = vert[i];

		// Vertex Normals --------------------
		if (hasNormals)
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
}

void ResourceMesh::InitRanges(uint num_vert, uint num_ind, uint num_normals)
{
	num_vertices = num_vert;
	num_indices = num_ind;

	if (num_normals > 0)
	{
		hasNormals = true;
	}
}

void ResourceMesh::InitInfo(const char* nameResource)
{
	name = App->GetCharfromConstChar(nameResource);
}



void ResourceMesh::DeleteToMemory()
{
	state = Resource::State::UNLOADED;

	if (vertices_id != NULL)		glDeleteBuffers(1, &vertices_id);
	if (indices_id != NULL)			glDeleteBuffers(1, &indices_id);
	if (vertices_norm_id != NULL)	glDeleteBuffers(1, &vertices_norm_id);

	num_vertices = 0;
	num_indices = 0;
	hasNormals = false;

	vertices.clear();
	indices.clear();
	vertices_normals.clear();
	LOG("UnLoaded Resource Mesh");
}

bool ResourceMesh::LoadToMemory()
{
	LOG("Resources: %s, Loaded in Memory!", this->name);
	//glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertices_id);
	glGenBuffers(1, &indices_id);
	glGenBuffers(1, &vertices_norm_id);

	//glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

	if (hasNormals)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices_norm_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices_normals.size() * sizeof(float3), &vertices_normals[0], GL_STATIC_DRAW);
	}

	//// Vertex Positions -------------
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (void*)0);

	//// Vertex Normals ----------------
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (void*)offsetof(_Vertex, norm));

	//glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	state = Resource::State::LOADED;
	return true;
}

Resource::State ResourceMesh::IsLoadedToMemory()
{
	return state;
}
