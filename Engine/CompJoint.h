#ifndef COMP_JOINT_H__
#define COMP_JOINT_H__

#include "Component.h"

class jpJoint;
class CompRigidBody;

class CompJoint : public Component
{
public:

	CompJoint(Comp_Type t, GameObject* parent);
	CompJoint(const CompJoint& copy, GameObject* parent);
	~CompJoint();

	void Clear();

	//EDITOR METHODS ---------------
	void ShowOptions();
	void ShowInspectorInfo();
	//------------------------------

	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	void SyncComponent(GameObject* sync_parent);

	// Joint Methods ---------------
	void CreateJoint();

	// Setters ---------------------
	void SetMinMaxDistance();
	void SetSecondActorPose();


	void RemoveActors(CompRigidBody* body);
	void RemoveActors();

private:
	CompRigidBody*			second = nullptr;
	uint					second_uid = 0;
	std::string				second_name;

	jpJoint*				joint = nullptr;

	float					min_dist = 2.0f;
	float					max_dist = 10.0f;
};
#endif // !COMP_JOINT_H__
