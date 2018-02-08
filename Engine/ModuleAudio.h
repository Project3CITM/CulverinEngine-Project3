#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "parson.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

class ModuleAudio : public Module
{
public:

	ModuleAudio(bool start_enabled = true);
	~ModuleAudio();

	bool Init(JSON_Object* node);
	//bool Start();
	//update_status PreUpdate(float dt);
	//update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool SaveConfig(JSON_Object* node);
	bool CleanUp();

	update_status UpdateConfig(float dt);

private:

	int volume = 0;
	bool mute = false;
};

#endif // __ModuleAudio_H__