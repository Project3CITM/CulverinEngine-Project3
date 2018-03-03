#ifndef COMP_JOINT_H__
#define COMP_JOINT_H__

#include "Component.h"

class jpJoint;

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
	void SyncComponent();


private:

	jpJoint* joint = nullptr;
};
#endif // !COMP_JOINT_H__
