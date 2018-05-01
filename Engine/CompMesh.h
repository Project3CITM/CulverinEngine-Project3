#ifndef _COMPONENT_MESH_
#define _COMPONENT_MESH_

#include "GL3W\include\glew.h"
#include "Component.h"
#include "Math/float3.h"
#include "Math/float2.h"

#include <vector>

class ResourceMesh;
class GameObject;
class CompMaterial;
class ShaderProgram;
struct Vertex;
class SkeletonSource;
class Material;
class GameObject;

struct FaceCenter
{
	float3 pos;
	float3 norm;
};

enum Type_Primitive
{
	TYPE_NON,
	TYPE_SPHERE,
	TYPE_CUBE,
	TYPE_PLANE
};

struct Skeleton
{
	std::vector<GameObject*> bones;
	std::vector<uint> bone_uids;

	GLuint buffer_id;
	GLuint skinning_mats_id;
	uint root_uid;
	GameObject* root_bone = nullptr;
	uint root_bone_uid;
	GLfloat* skinning_mats = nullptr;

	~Skeleton()
	{
		bones.clear();
		bone_uids.clear();
		delete [] skinning_mats;
	}

	void GenSkinningTexture(const GameObject* mesh_go);
	void DebugDraw() const;
	void Save(JSON_Object* object, std::string name) const;
	void Load(const JSON_Object* object, std::string name);
	void Link(const SkeletonSource* source);
};

class CompMesh: public Component
{
public:

	CompMesh(Comp_Type t, GameObject* parent);
	CompMesh(const CompMesh& copy, GameObject* parent);
	~CompMesh();

	void Draw(bool alpha = false);
	void Draw2(uint ID);
	void Clear();
	void PreUpdate(float dt);
	void Update(float dt);
	void Render(bool render);
	bool IsRendering() const;

	// EDITOR METHODS ---------
	void ShowOptions();
	void ShowInspectorInfo();
	// ------------------------

	void LinkMaterial(const CompMaterial* mat);
	void SetResource(ResourceMesh * resourse_mesh, bool isImport = false);

	//Copy Values -----
	void CopyValues(const CompMesh * component);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	void SyncComponent(GameObject* sync_parent);
	void LinkSkeleton();
	// -------------------------------------



	bool HasSkeleton() const;
	void SetSkeleton(bool has_skeleton);
	void GenSkeleton();
	Skeleton* GetSkeleton()const;
	GameObject* GenBone(char** name_iterator, const SkeletonSource* source, uint& generated_bones, Skeleton* skeleton);

	CompMaterial* GetMaterial() const;

public:

	char* name = "MESH NAME";
	//bool has_normals = false;

	ResourceMesh* resource_mesh = nullptr;

private:

	bool render = true;
	bool select_mesh = false;
	const CompMaterial* comp_material = nullptr;
	uint uuid_resource_reimported = 0;

	bool has_skeleton = false;
	Skeleton* skeleton = nullptr;
	bool debug_skeleton = false;
	bool generated_skeleton = false;
};
#endif