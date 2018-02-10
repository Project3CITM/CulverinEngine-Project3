#ifndef COMP_AUDIO_H
#define COMP_AUDIO_H


#include "Component.h"
#include "wwished.h"

class GameObject;

enum AUDIO_TYPE
{
	FX,
	LISTENER
};

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

	uint GetEmitterID() const;
	void ResetAudio();

private:

	void CreateEmitter();
	
private:
	Wwished::SoundEmitter* emitter = nullptr;
	AUDIO_TYPE audio_type = FX;
};
#endif