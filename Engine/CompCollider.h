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


};