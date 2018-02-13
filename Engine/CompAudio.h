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
	FX,
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

	//void SetResource(ResourceAnimation * resource_animation, bool isImport = false);
	void CopyValues(const CompAudio * component);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------

	uint GetEmitterID() const;
	void ResetAudio();

private:

	void ShowEventsInfo();
	void CreateEmitter();
	void CreateAudioEvent(std::string audio_event, int gameplay_event);
	
private:
	CompTransform * transf = nullptr;
	Wwished::SoundEmitter* emitter = nullptr;
	AUDIO_TYPE audio_type = FX;

	std::vector<std::pair<int, AudioEvent>> audio_events;
};
#endif