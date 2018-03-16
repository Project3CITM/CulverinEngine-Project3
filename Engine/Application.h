#ifndef APPLICATION_H
#define APPLICATION_H

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
class ModuleEventSystem;
class ModuleEventSystemV2;
class ModuleShaders;
class JSONSerialization;
class ModuleRenderGui;
class ModulePhysics;
class ModuleMap;
class ModuleLightning;
class ModuleKeyBinding;
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
	float game_start_time = 0.0f; // To count the time in 
	float timePlay = -1.0f; // To count the time in 
	float time_scale = 1.0f; // Time multiplier
	uint64 frame_count = 0;  // Total Updates since Game Mode started

	// Variables to play only for 1 update
	bool prepare_frame = false;
	bool play_frame = false;
};

// Editor Mode structure to manage the Real Time
struct RealTimeClock
{
	Timer engine_start_time;
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

	void SetState(EngineState state);

	void WantToSave();
	void WantToLoad(bool in_game = false);

	void ChangeCamera(const char* window);

	std::string GetActualScene();
	void SetActualScene(std::string);

	const std::vector<Module*>* GetModuleList() const;

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
	ModuleEventSystem* event_system = nullptr;
	ModuleEventSystemV2* event_system_v2 = nullptr;
	ModuleShaders* module_shaders = nullptr;
	ModuleRenderGui* render_gui = nullptr;
	ModulePhysics* physics = nullptr;
	ModuleMap* map = nullptr;
	ModuleLightning* module_lightning = nullptr;
	ModuleKeyBinding* module_key_binding = nullptr;
private:
	std::vector<Module*> list_modules;
	// ----------------------------------

	// TIME -----------------------------
public:
	
	RealTimeClock real_time;
	GameClock game_time;
	EngineState engine_state = EngineState::STOP; // To manage if we are in Game Mode or not

	/* Performance Variables */
	int		frame_index = 0;
	int		ms_index = 0;
	float	fps_log[70] = { 0 };
	float	ms_log[70] = { 0 };

	uint maxFPS = 0;
	bool vsync = true;
	// ----------------------------------

	bool mode_game = false;

public:
	// Variables for enable different windows ---
	LCG* random = nullptr;
	bool show_config = false;
	bool show_performance = false;
	bool show_camera_popup = false;
	// -------------------------------------------

	float4 scene_dock = { 0, 0, 0, 0 };

	//Use for Serialization scene or prefabs 
	JSONSerialization* json_seria = nullptr;

private:

	std::string app_name;
	std::string org_name;

	std::string actual_scene;

	bool want_to_save = false;
	bool want_to_load = false;
	bool load_in_game = false;

	bool change_to_scene = false;
	bool change_to_game = false;

	DockContext* configuration = nullptr;
};

#endif

extern Application* App;