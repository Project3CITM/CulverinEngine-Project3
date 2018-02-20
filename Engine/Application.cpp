#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "Scene.h"
#include "ModuleConsole.h"
#include "ModuleGUI.h"
#include "ModuleImporter.h"
#include "ModuleTextures.h"
#include "ModuleFS.h"
#include "ModuleResourceManager.h"
#include "ModuleEventSystem.h"
#include "ModuleRenderGui.h"
#include "ModulePhysics.h"
#include "ModuleLightning.h"
#include "parson.h"
#include "PerfTimer.h"
#include "WindowSceneWorld.h"
#include "ModuleMap.h"
#include"ModuleShaders.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl_gl3.h"
#include "ImGui/ImGuizmo.h"
#include "SDL/include/SDL.h"
#include "JSONSerialization.h"
#include "mmgr/mmgr.h"
#include "ModuleEventSystem.h"

static int malloc_count;
static void *counted_malloc(size_t size);
static void counted_free(void *ptr);

Application::Application()
{
	window = new ModuleWindow();
	input = new ModuleInput();
	audio = new ModuleAudio(true);
	renderer3D = new ModuleRenderer3D();
	render_gui = new ModuleRenderGui();

	camera = new ModuleCamera3D();
	scene = new Scene();
	console = new Console();
	gui = new ModuleGUI();
	importer = new ModuleImporter();
	fs = new ModuleFS();
	textures = new ModuleTextures();
	resource_manager = new ModuleResourceManager();
	event_system = new ModuleEventSystem();

	random = new math::LCG();
	json_seria = new JSONSerialization();

	module_lightning = new ModuleLightning();
	module_shaders = new ModuleShaders();
	physics = new ModulePhysics();
	map = new ModuleMap();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(fs);
	AddModule(camera);
	AddModule(resource_manager);
	AddModule(physics);
	AddModule(input);
	AddModule(audio);
	AddModule(console);
	AddModule(scene);
	AddModule(module_lightning); // Ask this module creators before changing the order, possible dependencies with scene and shaders module.
	AddModule(module_shaders);
	AddModule(gui);
	AddModule(map); // Possible change position
	AddModule(importer);
	AddModule(textures);
	AddModule(event_system); //keep event system down and before render, we have events to draw, so we need to update everrything before, draw with events and render
	// Renderer last!
	AddModule(renderer3D);
	AddModule(render_gui);

}

Application::~Application()
{
	std::vector<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end())
	{
		delete *item;
		item++;
	}
	RELEASE(configuration);
	RELEASE(random);
	RELEASE(json_seria);
}

bool Application::Init()
{
	bool ret = true;

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file("config.json");

	configuration = new DockContext();
	
	//In case of error
	if (config_file != nullptr)
	{
		ret = true;

		// Get editor variables from .json document ------
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config,"Application");
		app_name = json_object_get_string(config_node, "App Name");
		org_name = json_object_get_string(config_node, "Org Name");
		maxFPS = json_object_get_number(config_node, "Max FPS");
		vsync = json_object_get_boolean(config_node, "VSYNC");
		mode_game = json_object_get_boolean(config_node, "Mode Game");

		actual_scene = json_object_get_string(config_node, "ActualScene");
		if (strcmp(actual_scene.c_str(), "") == 0)
		{
			mode_game = false;
		}
		if (mode_game)
		{
			gui->enabled = false;
		}
		SetFpsCap(maxFPS);
		// ---------------------------------------------------

		// Call Init() in all modules
		std::vector<Module*>::iterator item = list_modules.begin();

		while (item != list_modules.end() && ret == true)
		{
			if ((*item)->IsEnabled())
			{
				config_node = json_object_get_object(config, (*item)->name.c_str());
				ret = (*item)->Init(config_node);
			}
			item++;
		}
		json_value_free(config_file);

		// After all Init calls we call Start() in all modules
		LOG("Application Start --------------");
		item = list_modules.begin();

		while (item != list_modules.end() && ret == true)
		{
			if ((*item)->IsEnabled())
			{
				ret = (*item)->Start();
			}
			item++;
		}
		real_time.engine_start_time.Start();
		real_time.ms_timer.Start();
	}
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	real_time.frame_count++;
	real_time.last_sec_frame_count++;
	real_time.dt = (float)real_time.ms_timer.ReadSec();
	real_time.ms_timer.Start();
	real_time.frame_time.Start();

	if (game_time.prepare_frame)
	{
		game_time.play_frame = true;
		game_time.prepare_frame = false;
	}

	if (engine_state == EngineState::PLAY || engine_state == EngineState::PLAYFRAME)
	{
		game_time.game_start_time += real_time.dt * game_time.time_scale;
		game_time.frame_count++;
	}

	if (change_to_game)
	{
		renderer3D->SetActiveCamera(renderer3D->game_camera);
		change_to_game = false;
	}
	else if (change_to_scene)
	{
		renderer3D->SetActiveCamera(renderer3D->scene_camera);
		change_to_scene = false;
	}
}

// ---------------------------------------------
void Application::FinishUpdate()
{

	// SAVE & LOAD FUNCTIONS ------------------------
	if (want_to_save == true)
	{
		actual_scene = json_seria->SaveScene();
		want_to_save = false;
	}

	if (want_to_load == true)
	{
		//Before Delete GameObjects Del Variables Scripts GameObject 
		App->scene->ClearAllVariablesScript();
		App->scene->DeleteAllGameObjects(App->scene->root); //TODO->Elliot
		json_seria->LoadScene(actual_scene.c_str());
		//App->resource_manager->ReImportAllScripts();
		want_to_load = false;
	}
	// ---------------------------------------------

	// Framerate calculations ----------------------
	if (real_time.last_sec_frame_time.Read() > 1000)
	{
		real_time.last_sec_frame_time.Start();
		real_time.prev_last_sec_frame_count = real_time.last_sec_frame_count;

		fps_log[frame_index] = real_time.prev_last_sec_frame_count;
		frame_index = (frame_index + 1) % IM_ARRAYSIZE(fps_log);

		real_time.last_sec_frame_count = 0;
	}

	float avg_fps = float(real_time.frame_count) / real_time.engine_start_time.ReadSec();
	float seconds_since_startup = real_time.engine_start_time.ReadSec();
	real_time.last_frame_ms = real_time.frame_time.Read();
	uint32 frames_on_last_update = real_time.prev_last_sec_frame_count;
	
	ms_log[ms_index] = real_time.last_frame_ms;

	//Get all performance data-------------------
	std::vector<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		if ((*item)->IsEnabled())
		{
			(*item)->pre_log[ms_index] = (*item)->preUpdate_t;
			 (*item)->up_log[ms_index] = (*item)->Update_t;
			 (*item)->post_log[ms_index] = (*item)->postUpdate_t;
		}
		item++;
	}

	ms_index = (ms_index + 1) % IM_ARRAYSIZE(ms_log); //ms_index works for all the logs (same size)


	if (real_time.capped_ms > 0 && real_time.last_frame_ms < real_time.capped_ms)
	{
		SDL_Delay(real_time.capped_ms - real_time.last_frame_ms);
	}

	if (game_time.play_frame)
	{
		game_time.play_frame = false;
		SetState(EngineState::PAUSE);
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	// SAVE / LOAD  keys ---------------------------------------
	//if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	//{
	//	want_to_save = true;
	//}
	//if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
	//{
	//	want_to_load = true;
	//}
	// ---------------------------------------------------------
	
	std::vector<Module*>::iterator item = list_modules.begin();
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->IsEnabled())
		{
			if ((*item) == camera)
			{
				ret = (*item)->PreUpdate(real_time.dt); // Camera can't be affected by Game Time Scale (0 dt = 0 movement)
			}
			else
			{
				if (engine_state == EngineState::PLAY || engine_state == EngineState::PLAYFRAME)
				{
					ret = (*item)->PreUpdate(real_time.dt * game_time.time_scale);
				}
				else if (engine_state == EngineState::PAUSE || engine_state == EngineState::STOP)
				{
					ret = (*item)->PreUpdate(0);
				}
			}
		}
		item++;
	}

	/* ImGui + ImGuizmo Begin Frame */
	ImGui_ImplSdlGL3_NewFrame(window->window);
	ImGuizmo::BeginFrame();

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->IsEnabled())
		{
			if ((*item) == camera)
			{
				// Camera can't be affected by Game Time Scale (0 dt = 0 movement)
				ret = (*item)->Update(real_time.dt);
			}
			else
			{
				if (engine_state == EngineState::PLAY || engine_state == EngineState::PLAYFRAME)
				{
					ret = (*item)->Update(real_time.dt * game_time.time_scale);
				}
				else if (engine_state == EngineState::PAUSE || engine_state == EngineState::STOP)
				{
					ret = (*item)->Update(0);
				}
			}
		}
		item++;
	}

	//PERFORMANCE WINDOW -----------------------
	if (show_performance)
	{
		static bool stop_perf = false;
		item = list_modules.begin();

		if (!ImGui::Begin("PERFORMANCE", &show_performance, ImGuiWindowFlags_NoCollapse))
		{
			ImGui::End();
			stop_perf = true;
		}
		else if (stop_perf == false)
		{
			ImGui::Spacing();
			while (item != list_modules.end())
			{
				if ((*item)->IsEnabled())
				{
					(*item)->ShowPerformance(ms_index);
				}
				item++;
			}
			ImGui::End();
		}
		stop_perf = false;
	}
	//-----------------------------------------------

	item = list_modules.begin();
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->IsEnabled())
		{
			if ((*item) == camera)
			{
				// Camera can't be affected by Game Time Scale (0 dt = 0 movement)
				ret = (*item)->PostUpdate(real_time.dt);
			}
			else
			{
				if (engine_state == EngineState::PLAY || engine_state == EngineState::PLAYFRAME)
				{
					ret = (*item)->PostUpdate(real_time.dt * game_time.time_scale);
				}
				else if (engine_state == EngineState::PAUSE || engine_state == EngineState::STOP)
				{
					ret = (*item)->PostUpdate(0);
				}
			}
		}
		item++;
	}

	FinishUpdate();
	return ret;
}

void Application::Config()
{
	if (show_config)
	{
		static bool stop_conf = false;
		bool ret = true;
		std::vector<Module*>::iterator item = list_modules.begin();
		item = list_modules.begin();

		if (!BeginDock("CONFIGURATION", &show_config, 0))
		{
			EndDock();
			stop_conf = true;
		}
		configuration->_BeginWorkspace("ConfigurationWindow");
		if (stop_conf == false)
		{
			static bool* temp = NULL;

			// APPLICATION OPTIONS --------------------------------------------------
			if (!configuration->_BeginDock("Application", temp, 0))
			{
				configuration->_EndDock();
			}
			else
			{
				ImGui::Text("App Name:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), app_name.c_str());
				ImGui::Text("Organization Name:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), org_name.c_str());
				static int fps = maxFPS;
				ImGui::SliderInt("Max FPS", &fps, 0, 60);
				ImGui::SameLine(); ShowHelpMarker("0 = no framerate cap"); ImGui::SameLine();
				if (ImGui::Button("APPLY"))
				{
					SetFpsCap(fps);
				}
				ImGui::Text("Framerate:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.0f", fps_log[frame_index - 1]);
				ImGui::PlotHistogram("", fps_log, IM_ARRAYSIZE(fps_log), 0, NULL, 0.0f, 120.0f, ImVec2(0, 80));
				ImGui::Text("Milliseconds:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.0f", ms_log[ms_index - 1]);
				ImGui::PlotHistogram("", ms_log, IM_ARRAYSIZE(ms_log), 0, NULL, 0.0f, 50.0f, ImVec2(0, 80));
				ImGui::Checkbox("VSYNC", &vsync); ImGui::SameLine();
				ShowHelpMarker("Restart to apply changes");

				// TIME MANAGEMENT --------------------------------------
				ImGui::Separator();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
				if (ImGui::TreeNodeEx("TIME MANAGEMENT", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PopStyleColor();

					ImGui::Text("Time Since Startup:"); ImGui::SameLine();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.0f s", real_time.engine_start_time.ReadSec());
					ImGui::Text("Frames in Last Second:"); ImGui::SameLine();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%u", real_time.prev_last_sec_frame_count);
					
					ImGui::Spacing();

					ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "GAME CLOCK");
					ImGui::SliderFloat("Time Scale", &game_time.time_scale, 0.0f, 5.0f);
					ImGui::Text("Time Since Game Started:"); ImGui::SameLine();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.2f s", game_time.game_start_time);
					ImGui::Text("Total Frames:"); ImGui::SameLine();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%u", game_time.frame_count);

					ImGui::TreePop();
				}
				else
				{
					ImGui::PopStyleColor();
				}
				// ------------------------------------------------------

				configuration->_EndDock();
			}

			// MEMORY CONSUMPTION ----------------------------------------------
			sMStats stats = m_getMemoryStatistics();
			if (!configuration->_BeginDock("Memory Consumption", temp, 0))
			{
				configuration->_EndDock();
			}
			else
			{
				ImGui::BulletText("ACCUMULATED");
				ImGui::Text("- Actual Memory:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.08f, 1.0f), "%i", stats.accumulatedActualMemory);
				ImGui::Text("- Allocated memory:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.08f, 1.0f), "%i", stats.accumulatedAllocUnitCount);
				ImGui::Text("- Reported memory:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.08f, 1.0f), "%i", stats.accumulatedReportedMemory);
				ImGui::Spacing();

				ImGui::BulletText("PEAK");
				ImGui::Text("- Actual Memory:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.08f, 1.0f), "%i", stats.peakActualMemory);
				ImGui::Text("- Allocated memory:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.08f, 1.0f), "%i", stats.peakAllocUnitCount);
				ImGui::Text("- Reported memory:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.08f, 1.0f), "%i", stats.peakReportedMemory);
				ImGui::Spacing();

				ImGui::BulletText("TOTAL");
				ImGui::Text("- Actual memory:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.08f, 1.0f), "%i", stats.totalActualMemory);
				ImGui::Text("- Allocated memory:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.08f, 1.0f), "%i", stats.totalAllocUnitCount);
				ImGui::Text("- Reported memory:"); ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.08f, 1.0f), "%i", stats.totalReportedMemory);

				configuration->_EndDock();
			}

			while (item != list_modules.end() && ret == UPDATE_CONTINUE)
			{
				if ((*item)->IsEnabled() && (*item)->have_config)
				{
					static bool* closeButton = NULL;
					if (!configuration->_BeginDock((*item)->name.c_str(), closeButton, ImGuiWindowFlags_NoCollapse))
					{
						configuration->_EndDock();
					}
					else
					{
						ret = (*item)->UpdateConfig(real_time.dt);
						configuration->_EndDock();
					}
				}
				item++;
			}
			configuration->_EndWorkspace();
			EndDock();
		}
		else
		{
			configuration->_EndWorkspace();
		}
		stop_conf = false;
	}
}

bool Application::CleanUp()
{
	bool ret = true;

	//Before Cleaning, save data to config file
	SaveConfig();

	std::vector<Module*>::iterator item = list_modules.end();

	while (item != list_modules.begin() && ret == true)
	{
		item--;
		ret = (*item)->CleanUp();
	}
	return ret;
}

bool Application::GetVSYNC()
{
	return vsync;
}

void Application::SetFpsCap(uint fps)
{
	maxFPS = fps;

	if (fps > 0)
	{
		real_time.capped_ms = 1000 / fps;
	}
	else
	{
		real_time.capped_ms = 0;
	}
}

bool Application::SaveConfig()
{
	bool ret = false;

	LOG("SAVING CONFIG TO FILE -----------------------")

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file("config.json");

	if (config_file != nullptr)
	{
		ret = true;

		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Application");

		json_object_set_string(config_node, "App Name", app_name.c_str());
		json_object_set_string(config_node, "Org Name", org_name.c_str());
		json_object_set_number(config_node, "Max FPS", maxFPS);
		json_object_set_boolean(config_node, "VSYNC", vsync);
		json_object_set_boolean(config_node, "Mode Game", mode_game);
		 
		//Save ActualScene ----- 
		json_object_set_string(config_node, "ActualScene", actual_scene.c_str()); //TODO ELLIOT


		//Iterate all modules to save each respective info
		std::vector<Module*>::iterator item = list_modules.begin();

		while (item != list_modules.end() && ret == true)
		{
			config_node = json_object_get_object(config, (*item)->name.c_str());
			ret = (*item)->SaveConfig(config_node);
			item++;
		}

		json_serialize_to_file(config_file, "config.json");
	}
	json_value_free(config_file);

	return ret;
}

void Application::ShowHelpMarker(const char* desc, const char* icon)
{
	ImGui::TextDisabled(icon);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

char* Application::GetCharfromConstChar(const char* name)
{
	int length = strlen(name);
	char* temp = new char[length + 1];
	strcpy(temp, name);
	temp[length] = '\0';
	return temp;
}

void Application::SetState(EngineState state)
{

	switch (state)
	{
	case EngineState::PLAYFRAME:
	case EngineState::PLAY:
		{
			if (engine_state == EngineState::STOP)
			{
				//Send Play event
				Event play_event;
				play_event.time.type = EventType::EVENT_TIME_MANAGER;
				play_event.time.time = play_event.time.TIME_PLAY;
				PushEvent(play_event);
			}
			else if (engine_state == EngineState::PAUSE)
			{
				//Send unpause Event
				Event play_event;
				play_event.time.type = EventType::EVENT_TIME_MANAGER;
				play_event.time.time = play_event.time.TIME_UNPAUSE;
				PushEvent(play_event);				
			}
			else if (engine_state == EngineState::PLAY)
			{
				//Send unpause Event
				Event play_event;
				play_event.time.type = EventType::EVENT_TIME_MANAGER;
				play_event.time.time = play_event.time.TIME_STOP;
				PushEvent(play_event);
			}
			break;
		}


	case EngineState::PAUSE:
	{
		//Send Pause Event
		Event play_event;
		play_event.time.type = EventType::EVENT_TIME_MANAGER;
		play_event.time.time = play_event.time.TIME_PAUSE;
		PushEvent(play_event);
		break;
	}
	}



	if (state == EngineState::PLAY)
	{
		// If it's already Game Mode, exit and start again Editor Mode
		if (engine_state == EngineState::PLAY)
		{

			//STOP ENGINE ------------

			engine_state = EngineState::STOP;
			game_time.game_start_time = 0.0f;
			game_time.frame_count = 0.0f;
			ChangeCamera("Scene"); // To notice renderer3D to change to Scene Camera

			// Clear static objects vector
			scene->FillStaticObjectsVector(false); 

			//To Load all elements saved before entering Game Mode
			WantToLoad();
		}
		else
		{
			if (App->renderer3D->game_camera != nullptr)
			{

				//PLAY ENGINE -------

				engine_state = EngineState::PLAY;
				ChangeCamera("Game"); // To notice renderer3D to change to Gcene Camera

				// Fill static objects vector when play
				scene->FillStaticObjectsVector(true); 

				if (mode_game == false)
				{
					scene->scene_buff->WantRefreshRatio();
				}

				//To Save all elements in the scene to load them correctly when exiting Game Mode
				actual_scene = json_seria->SaveScene();
			}
			else
			{
				show_camera_popup = true;
			}
		}
	}
	else 
	{
		// PAUSE & PLAYFRAME buttons only work if you are in game mode
		if (engine_state != EngineState::STOP)
		{
			engine_state = state;
		}
	}
	LOG("Engine State is Now: %i", engine_state);
}

void Application::WantToSave()
{
	want_to_save = true;
}

void Application::WantToLoad()
{
	want_to_load = true;
}

void Application::ChangeCamera(const char* window)
{
	if (window == "Game")
	{
		change_to_game = true;
		change_to_scene = false;
	}
	else if (window == "Scene")
	{
		change_to_game = false;
		change_to_scene = true;
	}
}

std::string Application::GetActualScene()
{
	return actual_scene;
}

void Application::SetActualScene(std::string scene)
{
	actual_scene = scene;
}

const std::vector<Module*>* Application::GetModuleList() const
{
	return &list_modules;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

static void *counted_malloc(size_t size) {
	void *res = malloc(size);
	if (res != NULL) {
		malloc_count++;
	}
	return res;
}

static void counted_free(void *ptr) {
	if (ptr != NULL) {
		malloc_count--;
	}
	free(ptr);
}