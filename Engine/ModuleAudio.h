#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "parson.h"
#include <vector>
#include <string>

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

	void DrawOnEditor();

	update_status UpdateConfig(float dt);


private:
	
	std::vector <std::string> loaded_banks;

	int volume = 0;
	bool mute = false;

	bool init_bank_not_loaded = false;
	bool load_bank_window_opened = false;
	bool error_window_opened = false;

	std::string banks_full_path;
};

#endif // __ModuleAudio_H__