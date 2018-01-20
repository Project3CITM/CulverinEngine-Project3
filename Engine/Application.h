#ifndef _APPLICATION_
#define _APPLICATION_

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "parson.h"

#include "GL3W/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include "ImGui/imgui_dock_v2.h"
#include "Algorithm/Random/LCG.h"
#include "Math/float4.h"

#include <list>
#include <vector>
#include <string>
#include <gl/GL.h>
#include <gl/GLU.h>

class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleRenderer3D;
class ModuleCamera3D;
class Scene;
class Console;
class ModuleGUI;
class ModuleImporter;
class ModuleFS;
class ModuleTextures;
class ModuleResourceManager;

class JSONSerialization;

enum EngineState
{
	PLAY = 0,
	PAUSE,
	STOP,
	PLAYFRAME
};

// Game Mode structure to manage the Game Time
struct GameClock
{
	float gameStart_time = 0.0f; // To count the time in 
	float timeScale = 1.0f; // Time multiplier
	uint64 frame_count = 0;  // Total Updates since Game Mode started

	// Variables to play only for 1 update
	bool prepare_frame = false;
	bool play_frame = false;
};

// Editor Mode structure to manage the Real Time
struct RealTimeClock
{
	Timer engineStart_time;
	Timer last_sec_frame_time;
	Timer frame_time;
	Timer ms_timer;

	uint64 frame_count = 0;
	uint32 last_sec_frame_count = 0;
	uint32 prev_last_sec_frame_count = 0;
	uint32 last_frame_ms = 0;
	float dt = 0.0f;
	int capped_ms = -1;
};

class Application
{
public:
	Application();
	~Application();

	bool Init();
	update_status Update();
	void Config();
	bool CleanUp();

	bool GetVSYNC();
	void SetFpsCap(uint);

	bool SaveConfig();

	static void ShowHelpMarker(const char* desc, const char* icon = "(?)");
	char* GetCharfromConstChar(const char* name);

	void SetState(EngineState state);

	void WantToSave();
	void WantToLoad();

	void ChangeCamera(const char* window);

private:
	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	// MODULES --------------------------
public:
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleAudio* audio = nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;
	Scene* scene = nullptr;
	Console* console = nullptr;
	ModuleGUI* gui = nullptr;
	ModuleImporter* importer = nullptr;
	ModuleFS* fs = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleResourceManager* resource_manager = nullptr;

private:
	std::list<Module*> list_modules;
	// ----------------------------------

	// TIME -----------------------------
public:
	RealTimeClock realTime;
	GameClock gameTime;
	EngineState engineState = EngineState::STOP; // To manage if we are in Game Mode or not

	/* Performance Variables */
	int		frame_index = 0;
	int		ms_index = 0;
	float	fps_log[70] = { 0 };
	float	ms_log[70] = { 0 };

	uint maxFPS = 0;
	bool vsync = true;
	// ----------------------------------

public:
	// Variables for enable different windows ---
	LCG* random = nullptr;
	bool showconfig = false;
	bool showperformance = false;
	bool showCameraPopup = false;
	// -------------------------------------------

	float4 SceneDock = { 0, 0, 0, 0 };

	//Use for Serialization scene or prefabs 
	JSONSerialization* Json_seria = nullptr;

private:
	std::string appName;
	std::string orgName;

	bool want_to_save = false;
	bool want_to_load = false;

	bool change_to_scene = false;
	bool change_to_game = false;

	DockContext* configuration = nullptr;
};

#endif

extern Application* App;