#ifndef COMP_RIGIDBODY_H
#define COMP_RIGIDBODY_H

#include "Component.h"

class jpPhysicsRigidBody;
class CompCollider;
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

	//Setters ----------------------
	void SetColliderPosition();
	void SetColliderComp(CompCollider* new_comp);

	//Getters ----------------------
	jpPhysicsRigidBody* GetPhysicsBody() const;
	void UpdateParentPosition();

private:

	jpPhysicsRigidBody* body = nullptr;
	CompCollider* collider_comp = nullptr;
	CompTransform* transform = nullptr;

	bool kinematic = false;
	bool own_update = false;

};

#endif // !COMP_RIGIDBODY_H
