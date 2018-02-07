#ifndef COMP_AUDIO_H
#define COMP_AUDIO_H


#include "Component.h"

class GameObject;

class CompAudio : public Component
{
public:

	CompAudio(Comp_Type t, GameObject* parent);
	CompAudio(const CompAudio& copy, GameObject* parent);
	~CompAudio();


	
	// EDITOR METHODS ---------
	void ShowOptions();
	void ShowInspectorInfo();
	// ------------------------

	//void SetResource(ResourceAnimation * resource_animation, bool isImport = false);
	void CopyValues(const CompAudio * component);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------

private:

};
#endif