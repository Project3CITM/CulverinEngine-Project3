#ifndef COMP_RIGIDBODY_H
#define COMP_RIGIDBODY_H

#include "Component.h"

class jpPhysicsRigidBody;
class CompCollider;
class CompJoint;
class CompTransform;
struct CollisionData;

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
	void SyncComponent(GameObject* sync_parent);

	bool IsKinematic();
	bool HaveBodyShape();

	//Setters ----------------------
	void SetColliderPosition();
	void SetMomentumToZero();
	void SetColliderComp(CompCollider* new_comp);
	void SetJointActor(CompJoint* joint);

	//Getters ----------------------
	jpPhysicsRigidBody* GetPhysicsBody() const;

	//Parent Methods ---------------
	void UpdateParentPosition();

	//Joint Methods ----------------
	void SetMaxJointPose();
	void RemoveJoint();

	// Collision Events ------------
	void OnTriggerEnter(Component* actor1);
	void OnTriggerLost(Component* actor1);

	void OnContact(CollisionData new_data);

	//PhysX Methods
	// Dynamic and kinematic -------
	void MoveKinematic(float3 pos, Quat rot);

	// Dynamic not kinematic -------	
	void ApplyForce(float3 force);
	void ApplyImpulse(float3 impulse);
	void ApplyTorqueForce(float3 force);
	void ApplyTorqueImpulse(float3 impulse);

	void LockTransform();
	void UnLockTransform();

	void WakeUp();

private:

	jpPhysicsRigidBody * body = nullptr;
	CompCollider*			collider_comp = nullptr;
	CompTransform*			transform = nullptr;
	CompJoint*				joint = nullptr;

	bool					kinematic = false;
	bool					own_update = false;

	uint					lock_move = 0;
	float					sleep_time = 0.4f;

};

#endif // !COMP_RIGIDBODY_H
