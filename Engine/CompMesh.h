#ifndef _COMPONENT_MESH_
#define _COMPONENT_MESH_

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

struct FaceCenter
{
	float3 pos;
	float3 norm;
};

enum Type_Primitive
{
	TYPE_NON,
	TYPE_SPHERE,
	TYPE_CUBE
};

struct Skeleton
{
	std::vector<GameObject*> bones;
};

class CompMesh: public Component
{
public:

	CompMesh(Comp_Type t, GameObject* parent);
	CompMesh(const CompMesh& copy, GameObject* parent);
	~CompMesh();

	void Draw();
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
	// -------------------------------------

	void SetUniformVariables(ShaderProgram* shader);

	bool HasSkeleton() const;
	void GenSkeleton();
	GameObject* GenBone(char** name_iterator, const SkeletonSource* source, uint& generated_bones);

public:

	char* name = "MESH NAME";
	bool has_normals = false;

	ResourceMesh* resource_mesh = nullptr;

private:

	bool render = true;
	bool select_mesh = false;
	const CompMaterial* material = nullptr;
	uint uuid_resource_reimported = 0;

	Skeleton* skeleton = nullptr;

};
#endif