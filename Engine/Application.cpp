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
#include "CompCamera.h"
#include "ModuleTextures.h"
#include "ModuleFS.h"
#include "ModuleResourceManager.h"
#include "ModuleEventSystem.h"
#include "ModuleRenderGui.h"
#include "ModulePhysics.h"
#include "ModuleLightning.h"
#include "ModuleParticles.h"
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
#include "ModuleEventSystemV2.h"
#include "ModuleAnimation.h"
#include "ImportScript.h"
#include "ModuleInput.h"
#include "PlayerActions.h"

#include <shlobj.h>

static int malloc_count;
static void *counted_malloc(size_t size);
static void counted_free(void *ptr);

Application::Application()
{
	BROFILER_CATEGORY("Application Creation", Profiler::Color::PaleVioletRed);
	window = new ModuleWindow();
	input = new ModuleInput();
	audio = new ModuleAudio(true);
	renderer3D = new ModuleRenderer3D();
	render_gui = new ModuleRenderGui();

	camera = new ModuleCamera3D();
	scene = new Scene();
	console = new Console();
	gui = new ModuleGUI();
	animation_ui = new ModuleAnimation();

	importer = new ModuleImporter();
	fs = new ModuleFS();
	textures = new ModuleTextures();
	resource_manager = new ModuleResourceManager();
	event_system_v2 = new ModuleEventSystemV2();

	random = new math::LCG();
	json_seria = new JSONSerialization();

	module_lightning = new ModuleLightning();
	module_shaders = new ModuleShaders();
	physics = new ModulePhysics();
	map = new ModuleMap();
	particles = new ModuleParticles();

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
	AddModule(console);
	AddModule(scene);
	AddModule(particles);
	AddModule(module_lightning); // Ask this module creators before changing the order, possible dependencies with scene and shaders module.
	AddModule(module_shaders);
	
	AddModule(gui);
	AddModule(animation_ui);

	AddModule(map); // Possible change position
	AddModule(importer);
	AddModule(textures);
	AddModule(audio);
	AddModule(event_system_v2); //keep event system down and before render, we have events to draw, so we need to update everrything before, draw with events and render
	// Renderer last!
	AddModule(renderer3D);
	AddModule(render_gui);
}

Application::Application(bool make_Build)
{
	fs = new ModuleFS();

	json_seria = new JSONSerialization();
	random = new math::LCG();

	importer = new ModuleImporter();
	resource_manager = new ModuleResourceManager();
	textures = new ModuleTextures();
	module_shaders = new ModuleShaders();
	renderer3D = new ModuleRenderer3D();
	//window = new ModuleWindow();

	build_mode = true;
	//AddModule(window);
	AddModule(fs);
	AddModule(resource_manager);
	AddModule(module_shaders);
	AddModule(importer);
	AddModule(textures);
	//AddModule(renderer3D);

	// ^^
}

Application::~Application()
{
	std::vector<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end())
	{
		delete *item;
		item++;
	}
	RELEASE(json_seria);
	RELEASE(random);
	if (build_mode == false)
	{
		RELEASE(configuration);
	}
}

bool Application::Init()
{
	BROFILER_CATEGORY("Application Init", Profiler::Color::PaleVioletRed);
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
				json_object_clear(config_node);
			}
			item++;
		}
		json_value_free(config_file);

		// After all Init calls we call Start() in all modules
		LOG("Application Start --------------");
		BROFILER_CATEGORY("Application Start", Profiler::Color::PaleVioletRed);
		item = list_modules.begin();

		while (item != list_modules.end() && ret == true)
		{
			if ((*item)->IsEnabled())
			{
				ret = (*item)->Start();
			}
			item++;
		}
		ImGui_ImplSdlGL3_Init(App->window->window);
		real_time.engine_start_time.Start();
		real_time.ms_timer.Start();
	}
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	BROFILER_CATEGORY("Application PrepareUpdate", Profiler::Color::PaleVioletRed);
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
		if (game_time.timePlay != -1.0f)
		{
			game_time.timePlay += real_time.dt * game_time.time_scale;
		}
	}

	if (change_to_game)
	{
		renderer3D->SetActiveCamera(renderer3D->game_camera);
		change_to_game = false;
	}
	else if (change_to_scene)
	{
		renderer3D->scene_camera->SetCullDynamics(false);
		renderer3D->SetActiveCamera(renderer3D->scene_camera);
		change_to_scene = false;
	}

	if (mode_game)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 0.0f;
	}
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	BROFILER_CATEGORY("Application FinishUpdate", Profiler::Color::PaleVioletRed);
	// SAVE & LOAD FUNCTIONS ------------------------
	if (want_to_save == true || want_to_save_binary == true)
	{
		if (App->mode_game == false)
		{
			if (want_to_save)
			{
				actual_scene = json_seria->SaveScene();
			}
			else if (want_to_save_binary)
			{
				actual_scene = json_seria->SaveSceneBinary();
			}
		}
		want_to_save = false;
		want_to_save_binary = false;
	}

	if (want_to_load == true || want_to_load_binary == true || want_to_load_json == true)
	{
		//Before Delete GameObjects Del Variables Scripts GameObject 
		scene->octree.Clear(false);
		scene->static_objects.clear();
		scene->dynamic_objects.clear();

		//App->importer->iScript->ClearMonoMap();
		std::string scene_binary = DIRECTORY_ASSETS;
		scene_binary += "SceneBinary/";
		scene_binary += fs->GetOnlyName(actual_scene) + ".culverinscene";

		App->scene->DeleteAllGameObjects(App->scene->root);
		scene->ClearAllTags();
		App->particles->CleanUp();

		render_gui->focus = nullptr;
		render_gui->selected = nullptr;
		render_gui->ClearInteractiveVector();

		if (want_to_load)
		{
			if (App->fs->CheckIsFileExist(scene_binary))
			{
				json_seria->LoadSceneBinary(scene_binary.c_str());
			}
			else
			{
				json_seria->LoadScene(actual_scene.c_str());
			}
		}
		else if (want_to_load_json)
		{
			json_seria->LoadScene(actual_scene.c_str());
		}
		else if (want_to_load_binary)
		{
			if (App->fs->CheckIsFileExist(scene_binary))
			{
				json_seria->LoadSceneBinary(scene_binary.c_str());
			}
			else
			{
				LOG("[error] Doesn't exist this scene in binary");
				LOG("[blue] Load scene with json!!");
				json_seria->LoadScene(actual_scene.c_str());
			}
		}

		if (load_in_game)
		{
			importer->iScript->SetMonoMap(App->scene->root, true);
			scene->root->StartScripts();
		}
		//App->resource_manager->ReImportAllScripts();
		if (engine_state != EngineState::STOP)
		{
			change_to_game = true;
		}
		want_to_load = false;
		want_to_load_binary = false;
		want_to_load_json = false;
		load_in_game = false;
	}

	if (dont_destroy_on_load)
	{
		//Before Delete GameObjects Del Variables Scripts GameObject 
		scene->octree.Clear(false);
		scene->static_objects.clear();

		//App->scene->DeleteAllGameObjects(App->scene->root);
		scene->ChangeRoot(scene->dontdestroyonload, scene->root);

		//First swap main camera
		GameObject* camera = scene->dontdestroyonload->FindGameObjectWithTag("camera");
		((CompCamera*)camera->FindComponentByType(Comp_Type::C_CAMERA))->SetMain(false);

		scene->ClearAllTags();
		importer->iScript->ClearLinkVariables();


		std::string scene_binary = DIRECTORY_ASSETS;
		scene_binary += "SceneBinary/";
		scene_binary += fs->GetOnlyName(actual_scene) + ".culverinscene";
		if (App->fs->CheckIsFileExist(scene_binary))
		{
			json_seria->LoadSceneBinary(scene_binary.c_str());
		}
		else
		{
			json_seria->LoadScene(actual_scene.c_str());
		}


		importer->iScript->SetMonoMap(App->scene->root, true);
		// Now do Start Scripts
		scene->root->StartScripts();
		input->player_action->SetInputManagerActive("GUI", true);

		//App->resource_manager->ReImportAllScripts();
		dont_destroy_on_load = false;
	}

	if (remove_dont_destroy_on_load)
	{
		render_gui->focus = nullptr;
		render_gui->selected = nullptr;
		render_gui->ClearInteractiveVector();

		scene->DeleteAllGameObjects(App->scene->dontdestroyonload);

		GameObject* camera2 = scene->FindGameObjectWithTagRecursive("camera", scene->root);
		//GameObject* camera2 = scene->root->FindGameObjectWithTagRecursive("camera");
		CompCamera* cam = (CompCamera*)camera2->FindComponentByType(Comp_Type::C_CAMERA);
		cam->SetMain(true);

		if (engine_state != EngineState::STOP)
		{
			change_to_game = true;
		}

		//App->importer->iScript->ClearMonoMap();
		//importer->iScript->SetMonoMap(App->scene->root, true);
		//// Now do Start Scripts
		//scene->root->StartScripts();

		remove_dont_destroy_on_load = false;
	}

	if (load_multi_scene)
	{
		/**/
		//Before Delete GameObjects Del Variables Scripts GameObject 
		scene->octree.Clear(false);
		scene->static_objects.clear();

		scene->ChangeRoot(scene->dontdestroyonload, scene->root);

		//First swap main camera
		GameObject* camera = scene->dontdestroyonload->FindGameObjectWithTag("camera");
		((CompCamera*)camera->FindComponentByType(Comp_Type::C_CAMERA))->SetMain(false);

		scene->ClearAllTags();
		importer->iScript->ClearLinkVariables();

		// Load SecondaryScene and swap
		std::string scene_binary = DIRECTORY_ASSETS;
		scene_binary += "SceneBinary/";
		scene_binary += fs->GetOnlyName(secondary_scene) + ".culverinscene";
		if (App->fs->CheckIsFileExist(scene_binary))
		{
			json_seria->LoadSceneBinary(scene_binary.c_str());
		}
		else
		{
			json_seria->LoadScene(secondary_scene.c_str());
		}
		scene->ChangeRoot(scene->secondary_root, scene->root);

		camera = scene->secondary_root->FindGameObjectWithTag("camera");
		((CompCamera*)camera->FindComponentByType(Comp_Type::C_CAMERA))->SetMain(false);

		// Load Root 
		scene_binary = DIRECTORY_ASSETS;
		scene_binary += "SceneBinary/";
		scene_binary += fs->GetOnlyName(actual_scene) + ".culverinscene";
		if (App->fs->CheckIsFileExist(scene_binary))
		{
			json_seria->LoadSceneBinary(scene_binary.c_str());
		}
		else
		{
			json_seria->LoadScene(actual_scene.c_str());
		}

		camera = scene->root->FindGameObjectWithTag("camera");
		((CompCamera*)camera->FindComponentByType(Comp_Type::C_CAMERA))->SetMain(true);
		if (engine_state != EngineState::STOP)
		{
			change_to_game = true;
		}

		// Set Scripting MonoMaps
		importer->iScript->SetMonoMap(App->scene->root, true);
		importer->iScript->SetMonoMap(App->scene->secondary_root, true);

		// Now do Start Scripts
		scene->root->StartScripts();
		scene->secondary_root->StartScripts();

		input->player_action->SetInputManagerActive("GUI", true);

		load_multi_scene = false;
	}

	if (remove_secondary_scene)
	{
		render_gui->focus = nullptr;
		render_gui->selected = nullptr;
		render_gui->ClearInteractiveVector();

		scene->DeleteAllGameObjects(scene->secondary_root,false);

		if (engine_state != EngineState::STOP)
		{
			change_to_game = true;
		}

		remove_secondary_scene = false;
	}

	if (change_to_secondary_scene)
	{
		if (scene->secondary_root->GetChildsVec().size() > 0)
		{
			// Reset gui inputs?
			render_gui->focus = nullptr;
			render_gui->selected = nullptr;
			render_gui->ClearInteractiveVector();

			//First swap main camera
			//GameObject* camera = scene->root->FindGameObjectWithTag("camera");
			//((CompCamera*)camera->FindComponentByType(Comp_Type::C_CAMERA))->SetMain(false);
			GameObject* camera2 = scene->FindGameObjectWithTagRecursive("camera", scene->root);
			CompCamera* cam = (CompCamera*)camera2->FindComponentByType(Comp_Type::C_CAMERA);
			cam->SetMain(false);

			//Swap Scenes
			GameObject* temp = new GameObject();
			scene->ChangeRoot(temp, scene->root);
			scene->ChangeRoot(scene->root, scene->secondary_root);
			scene->ChangeRoot(scene->secondary_root, temp);
			RELEASE(temp);

			//Set new main camera
			//camera = scene->root->FindGameObjectWithTag("camera");
			//((CompCamera*)camera->FindComponentByType(Comp_Type::C_CAMERA))->SetMain(true);
			GameObject* camera3 = scene->FindGameObjectWithTagRecursive("camera", scene->root);
			CompCamera* cam3 = (CompCamera*)camera3->FindComponentByType(Comp_Type::C_CAMERA);
			cam3->SetMain(true);

			if (engine_state != EngineState::STOP)
			{
				change_to_game = true;
			}
			//Block or not in Game InputEvents 
			input->player_action->SetInputManagerActive("GUI", activate_gui_input);
		}
		activate_gui_input = false;
		change_to_secondary_scene = false;
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
	BROFILER_CATEGORY("Application PreUpdate", Profiler::Color::PaleVioletRed);
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
			if (((*item) == camera) || ((*item) == event_system_v2))
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

	if (mode_game)
	{
		//if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		//{
		//	active_fps = !active_fps;
		//}
		if (active_fps)
		{
			ImGui::SetNextWindowPos(ImVec2(20, 20));
			ImGui::Begin("Info", NULL, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoInputs);
			ImGui::Text("FPS: %.0f", App->fps_log[App->frame_index - 1]);
			ImGui::End();
		}
	}

	item = list_modules.begin();
	BROFILER_CATEGORY("Application Update", Profiler::Color::PaleVioletRed);
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->IsEnabled())
		{
			if (((*item) == camera) || ((*item) == event_system_v2))
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
	BROFILER_CATEGORY("Application PostUpdate", Profiler::Color::PaleVioletRed);
	item = list_modules.begin();
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->IsEnabled())
		{
			if (((*item) == camera) || ((*item) == event_system_v2) || ((*item) == renderer3D))
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
	if (App->mode_game == false)
	{
		SaveConfig();
	}

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
		json_object_set_string(config_node, "ActualScene", fs->GetToAsstes(actual_scene).c_str()); //TODO ELLIOT


		//Iterate all modules to save each respective info
		std::vector<Module*>::iterator item = list_modules.begin();

		while (item != list_modules.end() && ret == true)
		{
			config_node = json_object_get_object(config, (*item)->name.c_str());
			ret = (*item)->SaveConfig(config_node);
			item++;
		}

		json_serialize_to_file(config_file, "config.json");
		json_object_clear(config_node);
		json_object_clear(config);
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
				play_event.Set_event_data(EventType::EVENT_TIME_MANAGER);
				play_event.time.time = play_event.time.TIME_PLAY;
				PushEvent(play_event);
			}
			else if (engine_state == EngineState::PAUSE)
			{
				//Send unpause Event
				Event play_event;
				play_event.Set_event_data(EventType::EVENT_TIME_MANAGER);
				play_event.time.time = play_event.time.TIME_UNPAUSE;
				PushEvent(play_event);				
			}
			else if (engine_state == EngineState::PLAY)
			{
				//Send unpause Event
				Event play_event;
				play_event.Set_event_data(EventType::EVENT_TIME_MANAGER);
				play_event.time.time = play_event.time.TIME_STOP;
				PushEvent(play_event);
			}
			break;
		}
	case EngineState::PAUSE:
	{
		//Send Pause Event
		Event play_event;
		play_event.Set_event_data(EventType::EVENT_TIME_MANAGER);
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
			game_time.timePlay = -1.0f;
			game_time.frame_count = 0.0f;
			ChangeCamera("Scene"); // To notice renderer3D to change to Scene Camera

			// Clear static objects vector
			//scene->FillStaticObjectsVector(false); 

			//To Load all elements saved before entering Game Mode
			WantToLoad();
		}
		else
		{
			if (App->renderer3D->game_camera != nullptr)
			{	
				// Fill static objects vector when play
				//scene->FillStaticObjectsVector(true); 

				if (engine_state != EngineState::PAUSE)
				{
					// To notice renderer3D to change to Gcene Camera
					ChangeCamera("Game");

					if (mode_game == false)
					{
						scene->scene_buff->WantRefreshRatio();

						//To Save all elements in the scene to load them correctly when exiting Game Mode
						//actual_scene = json_seria->SaveScene();
					}
				}

				//PLAY ENGINE ------
				engine_state = EngineState::PLAY;
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

void Application::WantToSave(bool binary)
{
	if (binary == false)
	{
		want_to_save = true;
	}
	else
	{
		want_to_save_binary = true;
	}
}

void Application::WantToLoad(bool in_game, bool binary, bool force_json)
{
	if (binary == false)
	{
		if (force_json)
		{
			want_to_load_json = true;
		}
		else
		{
			want_to_load = true;
		}
	}
	else
	{
		want_to_load_binary = true;
	}
	load_in_game = in_game;
}

void Application::DontDestroyOnLoad()
{
	dont_destroy_on_load = true;
}

void Application::LoadMultiScene()
{
	load_multi_scene = true;
}

void Application::ChangeToSecondary()
{
	change_to_secondary_scene = true;
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

void Application::SetSecondaryScene(std::string scene)
{
	secondary_scene = scene;
}

const std::vector<Module*>* Application::GetModuleList() const
{
	return &list_modules;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::MakeBuild(std::string build_name, std::string Initial_scene, std::string destination, bool game_mode_)
{
	static std::string initial_scene;
	std::vector<std::string> scenes_build;
	App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), scenes_build, "scene.json");
	if (scenes_build.size() > 0)
	{
		initial_scene = "UnKnown";
		for (int i = 0; i < scenes_build.size(); i++)
		{
			if (strcmp(App->fs->GetOnlyName(scenes_build[i]).c_str(), Initial_scene.c_str()) == 0)
			{
				initial_scene = Initial_scene;
				initial_scene += ".scene.json";
				LOG("Initial Scene: %s", initial_scene.c_str());
			}
		}
	}

	LOG("Save Initial Scene");
	App->SaveLogs();

	// Settings
	static std::string game_name;
	game_name = build_name;

	static bool native_resolution = false;
	static bool run_in_background = false;
	static bool full_desktop = true;
	static bool game_mode = true;
	static bool use_release = true;

	// First Create a new folder in desktop
	// Destination: "/MyBuilds"
	std::string desktop = App->fs->GetGameDirectory() + destination; 
	App->fs->CreateFolder(desktop.c_str());
	LOG("%s", desktop.c_str());
	//desktop = "C:/Users/Administrador/Desktop";
	App->fs->NormalitzatePath(desktop);
	desktop += "/" + game_name;
	App->fs->CreateFolder(desktop.c_str());

	LOG("Create Main Folders");
	App->SaveLogs();

	// Now Create a folder Assets and copy-paste all metas/cs
	std::string desktop_assets = desktop + "/Assets";
	App->fs->CreateFolder(desktop_assets.c_str());
	std::vector<std::string> files_meta;
	App->fs->GetAllMetas(App->fs->GetMainDirectory(), files_meta);
	for (int i = 0; i < files_meta.size(); i++)
	{
		std::string desktop_assets_temp = desktop_assets + "/";
		desktop_assets_temp += App->fs->FixName_directory(files_meta[i]);
		App->fs->CopyPasteFile(files_meta[i].c_str(), desktop_assets_temp.c_str());
	}
	files_meta.clear();

	LOG("Copy all metas");
	App->SaveLogs();

	// Now copy-paste all files .cs
	std::vector<std::string> files_cs;
	App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), files_cs, "cs");
	for (int i = 0; i < files_cs.size(); i++)
	{
		std::string desktop_assets_temp = desktop_assets + "/";
		desktop_assets_temp += App->fs->FixName_directory(files_cs[i]);
		App->fs->CopyPasteFile(files_cs[i].c_str(), desktop_assets_temp.c_str());
	}
	files_cs.clear();

	LOG("Copy all cs");
	App->SaveLogs();

	std::vector<std::string> prefabs;
	App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), prefabs, "prefab.json");
	for (int i = 0; i < prefabs.size(); i++)
	{
		std::string desktop_assets_temp = desktop_assets + "/";
		desktop_assets_temp += App->fs->FixName_directory(prefabs[i]);
		App->fs->CopyPasteFile(prefabs[i].c_str(), desktop_assets_temp.c_str());
	}
	prefabs.clear();

	LOG("Copy all prefabs");
	App->SaveLogs();

	// Now copy all scenes
	for (int i = 0; i < scenes_build.size(); i++)
	{
		std::string desktop_assets_temp = desktop_assets + "/";
		desktop_assets_temp += App->fs->GetOnlyName(scenes_build[i]);
		desktop_assets_temp += ".scene.json";
		App->fs->CopyPasteFile(scenes_build[i].c_str(), desktop_assets_temp.c_str());
	}
	scenes_build.clear();

	LOG("Copy all scenes");
	App->SaveLogs();

	// Copy Folder Maps -------------------------------------------
	std::string path_copy = App->fs->GetMainDirectory() + "/Maps";
	std::string path_paste = desktop_assets + "/Maps";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy Map folder");
	App->SaveLogs();

	// Copy Folder Maps -------------------------------------------
	path_copy = App->fs->GetMainDirectory() + "/SceneBinary";
	path_paste = desktop_assets + "/SceneBinary";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy SceneBinary folder");
	App->SaveLogs();


	// Copy Folder ParticleSystem -------------------------------------------
	path_copy = App->fs->GetMainDirectory() + "/ParticleSystem";
	path_paste = desktop_assets + "/ParticleSystem";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy ParticleSystem folder");
	App->SaveLogs();

	// Copy Folder Shaders -------------------------------------------
	path_copy = App->fs->GetMainDirectory() + "/Shaders";
	path_paste = desktop_assets + "/Shaders";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy Shaders folder");
	App->SaveLogs();

	// After, copy folders (Library, AudioBanks, Images, Mono, ScriptManager, Fonts)
	// Library -----------------------------------------------------
	path_copy = App->fs->GetGameDirectory() + "/Library";
	path_paste = desktop + "/Library";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy Library folder");
	App->SaveLogs();

	// AudioBanks -----------------------------------------------------
	path_copy = App->fs->GetGameDirectory() + "/AudioBanks";
	path_paste = desktop + "/AudioBanks";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy AudioBanks folder");
	App->SaveLogs();

	// Images -----------------------------------------------------
	path_copy = App->fs->GetGameDirectory() + "/Images";
	path_paste = desktop + "/Images";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy Images folder");
	App->SaveLogs();

	// Mono -----------------------------------------------------
	path_copy = App->fs->GetGameDirectory() + "/Mono";
	path_paste = desktop + "/Mono";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy Mono folder");
	App->SaveLogs();

	// ScriptManager -----------------------------------------------------
	path_copy = App->fs->GetGameDirectory() + "/ScriptManager";
	path_paste = desktop + "/ScriptManager";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy ScriptManager folder");
	App->SaveLogs();

	// Fonts -----------------------------------------------------
	path_copy = App->fs->GetGameDirectory() + "/Fonts";
	path_paste = desktop + "/Fonts";
	App->fs->CopyPasteFolder(path_copy.c_str(), path_paste.c_str());

	LOG("Copy Fonts folder");
	App->SaveLogs();

	// Player Actions
	std::string desktop_library = desktop + "/";
	desktop_library += "Library/JSON/player_action.json";
	std::string temp_player = App->fs->GetMainDirectory();
	temp_player += "/player_action.json";
	App->fs->CopyPasteFile(temp_player.c_str(), desktop_library.c_str());

	// Then all files (no Folders) in Game.
	std::vector<std::string> files_game;
	App->fs->GetOnlyFilesFromFolder(App->fs->GetGameDirectory(), files_game);
	for (int i = 0; i < files_game.size(); i++)
	{
		std::string files_game_new = desktop + "/";
		files_game_new += App->fs->FixName_directory(files_game[i]);
		App->fs->CopyPasteFile(files_game[i].c_str(), files_game_new.c_str());
	}

	LOG("Copy all files (Game)");
	App->SaveLogs();

	// Then Edit config file with new settings
	std::string new_config = desktop;
	new_config += "/config.json";
	App->json_seria->SaveConfig(new_config, game_name, initial_scene, game_mode, full_screen, resizable_window, borderless, full_desktop);

	LOG("Save New Config");
	App->SaveLogs();

		// Finnaly .exe from Release
	std::string folder = App->fs->GetGameDirectory();
	folder += "/CulverinEngine.exe";
	std::string executable = desktop;
	executable += "/" + game_name + ".exe";
	App->fs->CopyPasteFile(folder.c_str(), executable.c_str(), true);
	LOG("Copy .exe");

}

void Application::SaveLogs(const char* path)
{
	// Create a file Logs.txt to saw all errors.
	if (path != nullptr)
	{
		std::ofstream outfile(path);
		for (int i = 0; i < savelogs.size(); i++)
		{
			outfile << savelogs[i].c_str() << std::endl;
		}
		outfile.close();
	}
	else
	{
		std::ofstream outfile("Logs.txt");
		for (int i = 0; i < savelogs.size(); i++)
		{
			outfile << savelogs[i].c_str() << std::endl;
		}
		outfile.close();
	}
}

bool Application::InitBuild()
{
	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file("config.json");
	config = json_value_get_object(config_file);
	std::vector<Module*>::iterator item = list_modules.begin();
	LOG("Application Init --------------");
	App->SaveLogs();
	while (item != list_modules.end())
	{
		if ((*item)->IsEnabled())
		{
			config_node = json_object_get_object(config, (*item)->name.c_str());
			(*item)->Init(config_node);
		}
		item++;
	}
	LOG("Application Start --------------");
	App->SaveLogs();
	item = list_modules.begin();

	while (item != list_modules.end())
	{
		if ((*item)->IsEnabled())
		{
			(*item)->Start();
		}
		item++;
	}
	return true;
}

void Application::SetFullScreen()
{
	full_screen = !full_screen;
}

void Application::SetWindowed()
{
	resizable_window = !resizable_window;
}

void Application::SetBordeless()
{
	borderless = !borderless;
}

void Application::SetVSync()
{
	vsync = !vsync;
}

void Application::ShowFPS()
{
	active_fps = !active_fps;
}

void Application::SwapControllerVibration()
{
	App->input->SwapVibration();
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