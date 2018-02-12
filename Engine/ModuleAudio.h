#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "parson.h"
#include <vector>
#include <string>


class CompAudio;

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
	void UnloadAllBanks();

	update_status UpdateConfig(float dt);
	

	void SaveAudioBanks(JSON_Object* config_node);
	void LoadAudioBanksFromScene(int number_of_banks, JSON_Object* config_node);


	void ChangeVolume(float volume);
	void Mute();

	void StopSounds();
	void PauseSounds();
	void ResumeSounds();

	void SetListener(CompAudio* c);


private:

	int LoadBank(const char* name);


private:
	
	std::vector <std::string> loaded_banks;

	float volume = 0;
	bool muted = false;
	float last_volume = 0;

	bool init_bank_not_loaded = false;
	bool load_bank_window_opened = false;


	std::string banks_full_path;

	CompAudio* current_listener = nullptr;
};

#endif // __ModuleAudio_H__