#ifndef COMP_COLLIDER_H
#define COMP_COLLIDER_H

#include "Component.h"
#include "ModulePhysics.h"

class jpPhysicsRigidBody;
class CompRigidBody;
class CompTransform;
class CompScript;

enum JP_COLLIDER_TYPE;

struct CollisionData
{
	Component* actor1 = nullptr;

	bool is_contact = false;
	float3 impact_point = float3::zero;
	float3 impact_normal = float3::zero;

public:

	void ResetData();

	GameObject* GetCollidedParent() const;

};

class CompCollider : public Component
{
public:
	CompCollider(Comp_Type t, GameObject* parent);
	CompCollider(const CompCollider& copy, GameObject* parent);
	~CompCollider();

	void Update(float dt);
	void Clear();
	void ClearCompCollider();

	//EDITOR METHODS ---------------
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCollider* component);
	//------------------------------

	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	void SyncComponent(GameObject* sync_parent);


	// Collision Events ------------
	void OnTriggerEnter(Component* actor1);
	void OnTriggerLost(Component* actor1);

	void OnContact(CollisionData new_data);

	void ChangeCollider();
	void UpdateCollider();
	void CollisionActive(bool active);

	//Meant to only be used by rigidbody to get the phsyics body
	jpPhysicsRigidBody* GivePhysicsBody(CompRigidBody* new_rigid_body);

	//Setters -------------------
	void SetColliderPosition();
	void SetSizeFromBoundingBox();
	void SetFilterFlags();

	void SetRigidBodyComp(CompRigidBody* new_comp);

	//Getters -------------------
	float3 GetPosition() const;
	float3 GetGlobalPosition() const;
	Quat GetLocalQuat() const;
	Quat GetGlobalQuat() const;
	GameObject* GetCollidedObject()const;
	float3 GetContactPoint() const;
	float3 GetContactNormal() const;

	//Scripting Method ----------
	void MoveStaticTo(float3 pos);

private:

	jpPhysicsRigidBody * body = nullptr;
	CompRigidBody* rigid_body_comp = nullptr;
	CompTransform* transform = nullptr;

	// Two defintions of the type are needed to avoid conflict when changing types
	JP_COLLIDER_TYPE	collider_type = (JP_COLLIDER_TYPE)3;
	JP_COLLIDER_TYPE	curr_type = (JP_COLLIDER_TYPE)3;

	// Collider Data
	Quat				local_quat = Quat(0, 0, 0, 1);
	float3				position = float3(0.f, 0.f, 0.f);
	float3				angle = float3(0.f, 0.f, 0.0f);

	float3				material = float3(0.5f, 0.5f, 0.0f);
	float3				size = float3(1.f, 1.f, 1.f);
	float				rad = 0.5f;

	// Collision Data
	bool				trigger = false;
	bool				on_move = false;
	bool				select_flags = false;
	uint				collision_flags = 0;
	uint				uid_script_asigned = 0;
	std::string			script_name;
	CompScript*			listener = nullptr;
	CollisionData		collision_data;

};

#endif // !COMP_COLLIDER_H