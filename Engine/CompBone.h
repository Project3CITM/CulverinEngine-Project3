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

	struct Weight
	{
		Weight(float weight, uint vertex_id) : weight(weight), vertex_id(vertex_id)
		{}

		float weight;
		uint vertex_id;		
	};

	float4x4 offset;
	std::vector<Weight> weights;
	ResourceMesh* resource_mesh;
};

