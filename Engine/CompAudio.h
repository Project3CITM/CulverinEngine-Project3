#ifndef COMP_AUDIO_H
#define COMP_AUDIO_H


#include "Component.h"
#include "wwished.h"
#include <string>
#include <vector>

class GameObject;
class CompTransform;

enum AUDIO_TYPE
{
	FX = 0,
	LISTENER
};

struct AudioEvent
{
	std::string name;
};

class CompAudio : public Component
{
public:

	CompAudio(Comp_Type t, GameObject* parent);
	CompAudio(const CompAudio& copy, GameObject* parent);
	~CompAudio();

	void Update(float dt);
	
	// EDITOR METHODS ---------
	void ShowOptions();
	void ShowInspectorInfo();
	
	// ------------------------

	void CopyValues(const CompAudio * component);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------

	uint GetEmitterID() const;
	void ResetAudio();
	void PlayAudioEvent(const char* event_name);
	void StopAudioEvent(const char* event_name);
	void SetAuxiliarySend(const char* target_bus, float value);

private:
	void CreateEmitter();
		
private:
	CompTransform * transf = nullptr;
	Wwished::SoundEmitter* emitter = nullptr;
	AUDIO_TYPE audio_type = FX;
};
#endif