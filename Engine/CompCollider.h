#ifndef COMP_COLLIDER_H
#define COMP_COLLIDER_H

#include "Component.h"

class CompCollider : public Component
{
public:
	CompCollider(Comp_Type t, GameObject* parent);
	CompCollider(const CompCollider& copy, GameObject* parent);
	~CompCollider();

	//EDITOR METHODS ---------------
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompCollider* component);

	// Collision Events ------------
	void OnTriggerEnter(Component* actor);
	void OnTriggerLost(Component* actor);

	//------------------------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
};

#endif // !COMP_COLLIDER_H