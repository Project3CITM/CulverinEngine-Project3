#include <vector>
#include "Math\float4x4.h"
#include "Component.h"

class ResourceMesh;

class CompBone : public Component
{
public:
	CompBone(Comp_Type t, GameObject* parent);
	CompBone(const CompBone& copy, GameObject* parent);
	~CompBone();

	bool Enable();
	bool Disable();
	void Init();
	void PreUpdate(float dt);
	void Update(float dt);
	void Draw();
	void Clear();
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompBone* comp);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------

	void GenSkinningMatrix(const float4x4& parent_transform);
	const float4x4& GetSkinningMatrix();

	uint uuid_resource_reimported = 0;

	struct Weight
	{
		Weight(float weight, uint vertex_id) : weight(weight), vertex_id(vertex_id)
		{}

		float weight;
		uint vertex_id;		
	};

	float4x4 offset;
	float4x4 skinning_matrix;
	std::vector<Weight> weights;
	ResourceMesh* resource_mesh = nullptr;
};

