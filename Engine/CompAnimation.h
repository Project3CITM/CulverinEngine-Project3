#ifndef COMPONENT_ANIMATION_H
#define COMPONENT_ANIMATION_H

#include "Component.h"

class GameObject;

class CompAnimation : public Component
{
public:

	CompAnimation(Comp_Type t, GameObject* parent);
	CompAnimation(const CompAnimation& copy, GameObject* parent);
	~CompAnimation();


	void DrawBones();
	void Clear();
	void PreUpdate(float dt);
	void Update(float dt);

	// EDITOR METHODS ---------
	void ShowOptions();
	void ShowInspectorInfo();
	// ------------------------

	//void SetResource(ResourceAnimation * resource_animation, bool isImport = false);
	void CopyValues(const CompAnimation * component);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------

private:

};
#endif
