#ifndef COMP_RIGIDBODY_H
#define COMP_RIGIDBODY_H

#include "Component.h"

class jpPhysicsRigidBody;
class CompCollider;

class CompRigidBody : public Component
{
public:
	CompRigidBody(Comp_Type t, GameObject* parent);
	CompRigidBody(const CompRigidBody& copy, GameObject* parent);
	~CompRigidBody();

	void Clear();

	//EDITOR METHODS ---------------
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompRigidBody* component);
	//------------------------------

	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);

	//Setters ----------------------
	void SetColliderComp(CompCollider* new_comp);

	//Getters ----------------------
	jpPhysicsRigidBody* GetPhysicsBody() const;

private:

	jpPhysicsRigidBody* body = nullptr;
	CompCollider* collider_comp = nullptr;

	bool kinematic = false;

};

#endif // !COMP_RIGIDBODY_H
