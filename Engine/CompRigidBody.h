#ifndef COMP_RIGIDBODY_H
#define COMP_RIGIDBODY_H

#include "Component.h"

class jpPhysicsRigidBody;
class CompCollider;
class CompJoint;
class CompTransform;

class CompRigidBody : public Component
{
public:
	CompRigidBody(Comp_Type t, GameObject* parent);
	CompRigidBody(const CompRigidBody& copy, GameObject* parent);
	~CompRigidBody();

	void PreUpdate(float dt);
	void Update(float dt);
	void Clear();

	//EDITOR METHODS ---------------
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompRigidBody* component);
	//------------------------------

	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	void SyncComponent();

	bool IsKinematic();
	bool HaveBodyShape();

	//Setters ----------------------
	void SetColliderPosition();
	void SetMomentumToZero();
	void SetColliderComp(CompCollider* new_comp);
	void SetJointActor(CompJoint* joint);

	//Getters ----------------------
	jpPhysicsRigidBody* GetPhysicsBody() const;

	void UpdateParentPosition();

	// Collision Events ------------
	void OnTriggerEnter(Component* trigger);
	void OnTriggerLost(Component* trigger);

	//PhysX Methods
	void MoveKinematic(float3 pos, Quat rot);

private:

	jpPhysicsRigidBody*		body = nullptr;
	CompCollider*			collider_comp = nullptr;
	CompTransform*			transform = nullptr;
	CompJoint*				joint = nullptr;

	bool					kinematic = false;
	bool					own_update = false;

	uint					lock_move = 0;

};

#endif // !COMP_RIGIDBODY_H
