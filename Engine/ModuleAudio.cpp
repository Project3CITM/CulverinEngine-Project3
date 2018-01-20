#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "SDL\include\SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"

#pragma comment( lib,  "SDL_mixer/libx86/SDL2_mixer.lib")

ModuleAudio::ModuleAudio(bool start_enabled) : Module(start_enabled), music(NULL)
{
	Awake_enabled = true;

	haveConfig = true;

	volume = 10;
	name = "Audio";
}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init(JSON_Object* node)
{
	perf_timer.Start();

	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		//Disable(); -> IMPLEMENT ENABLE - DISABLE
		return true; // Ugly patch for class computers without audio :(
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = true;
	}

	volume = json_object_get_number(node, "Volume");
	mute = json_object_get_boolean(node, "Mute");

	Awake_t = perf_timer.ReadMs();
	return ret;
}

//bool ModuleWindow::Start()
//{
//	perf_timer.Start();
//
//	Start_t = perf_timer.ReadMs();
//	return true;
//}
//
//update_status ModuleWindow::PreUpdate(float dt)
//{
//	perf_timer.Start();
//
//	preUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}
//
//update_status ModuleWindow::Update(float dt)
//{
//	perf_timer.Start();
//
//	Update_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}
//
//update_status ModuleWindow::postUpdate(float dt)
//{
//	perf_timer.Start();
//
//	postUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}

bool ModuleAudio::SaveConfig(JSON_Object * node)
{
	//Save audio config info --------------------------------
	json_object_set_number(node, "Volume", volume);
	json_object_set_boolean(node, "Mute", mute);
	// ------------------------------------------------------
	return true;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}

	std::list<Mix_Chunk*>::iterator item = fx.begin();

	while (item != fx.end())
	{
		Mix_FreeChunk(item._Ptr->_Myval);
		item++;
	}

	fx.clear();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool ModuleAudio::PlayMusic(const char* path, float fade_time)
{
	/*if (IsEnabled() == false) -> IMPLEMENT ENABLE - DISABLE
		return false;*/

	bool ret = true;

	if (music != NULL)
	{
		if (fade_time > 0.0f)
		{
			Mix_FadeOutMusic((int)(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}
		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if (music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if (fade_time > 0.0f)
		{
			if (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}

		if (mute == false)
		{
			VolumeMusic(volume);
		}
		else
		{
			VolumeMusic(0);
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

bool ModuleAudio::StopMusic()
{
	bool ret = true;
	if (music != NULL)
	{
		Mix_PauseMusic();
		if (Mix_PausedMusic == 0)
		{
			LOG("Music wasn't paused. Mix_GetError(): %s", Mix_GetError());
			ret = false;
		}
		else
		{
			LOG("Music Paused");
		}
	}
	return ret;
}

bool ModuleAudio::ResumeMusic()
{
	bool ret = true;
	if (music != NULL)
	{
		Mix_ResumeMusic();
		if (Mix_Playing(-1) == 0)
		{
			LOG("Music is now playing.");
			ret = false;
		}
		else
		{
			LOG("Music dosent play. Mix_GetError(): %s", Mix_GetError());
		}
	}
	return ret;
}

void ModuleAudio::VolumeMusic(int volume)
{
	if (music != NULL)
	{
		Mix_VolumeMusic(volume);
		LOG("volume is now : %d\n", Mix_VolumeMusic(-1));
	}
}

void ModuleAudio::Mute(bool mute)
{
	if (mute)
	{
		VolumeMusic(0);
	}
	else
	{
		VolumeMusic((float)volume / 100 * 128);
	}
}

void ModuleAudio::FadeMusic(int ms)
{
	if (music != NULL)
	{
		Mix_FadeOutMusic((int)(ms * 1000.0f));
	}
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path)
{
	/*if (IsEnabled() == false) -> ENABLE - DISABLE
		return 0;*/

	unsigned int ret = 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if (chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool ModuleAudio::PlayFx(unsigned int id, int repeat)
{
	/*if (IsEnabled() == false) -> IMPLEMENT ENABLE - DISABLE
		return false;*/

	bool ret = false;
	Mix_Chunk* chunk = NULL;

	// TODO -> ADAPT "at(index, &chunk)" TO STD::LIST
	//if (fx.at(id - 1, chunk) == true)
	//{
	//	Mix_PlayChannel(-1, chunk, repeat);
	//	ret = true;
	//}

	return ret;
}

update_status ModuleAudio::UpdateConfig(float dt)
{
	if (ImGui::SliderInt("Volume", &volume, 0, 100))
	{
		if (mute == false)
		{
			VolumeMusic((float)volume / 100 * 128);
		}
	}

	if (ImGui::Checkbox("Mute", &mute))
	{
		Mute(mute);
	}
	return UPDATE_CONTINUE;
}
