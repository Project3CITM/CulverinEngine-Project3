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
	//------------------------------

	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
};