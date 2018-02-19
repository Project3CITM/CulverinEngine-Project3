#ifndef COMP_COLLIDER_H
#define COMP_COLLIDER_H

#include "Component.h"

class jpPhysicsRigidBody;
class CompTransform;
class CompScript;

enum JP_COLLIDER_TYPE;

class CompCollider : public Component
{
public:
	CompCollider(Comp_Type t, GameObject* parent);
	CompCollider(const CompCollider& copy, GameObject* parent);
	~CompCollider();

	void Update(float dt);
	void Clear();

	//EDITOR METHODS ---------------
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCollider* component);
	//------------------------------

	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);


	// Collision Events ------------
	void OnTriggerEnter(Component* actor);
	void OnTriggerLost(Component* actor);
	
	void ChangeCollider();
	void UpdateCollider();

	//Setters -------------------
	void SetColliderPosition();
	void SetSizeFromBoundingBox();

	//Getters
	float3 GetPosition() const;
	Quat GetLocalQuat() const;

private:
	
	jpPhysicsRigidBody * body;
	
	CompTransform* transform;

	// Two defintions of the type are needed to avoid conflict when changing types
	JP_COLLIDER_TYPE	collider_type = (JP_COLLIDER_TYPE)3;	//Defines the next type of collider when changing it via editor
	JP_COLLIDER_TYPE	curr_type = (JP_COLLIDER_TYPE)3;		//Defines the current type of the collider

	// Collider Data
	Quat				local_quat = Quat(0, 0, 0, 1);
	float3				position = float3(0.f, 0.f, 0.f);
	float3				angle = float3(0.f, 0.f, 0.0f);

	float3				material = float3(0.5f, 0.5f, 0.0f);
	float3				size = float3(1.f, 1.f, 1.f);
	float				rad = 0.5f;

	bool				trigger = false;	
	CompScript*			listener = nullptr;

};

#endif // !COMP_COLLIDER_H