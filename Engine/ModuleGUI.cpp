#include "Globals.h"
#include "ModuleGUI.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleConsole.h"
#include "Scene.h"
#include "ModuleCamera3D.h"
#include "ModuleTextures.h"
#include "ModuleWindow.h"
#include "WindowHardware.h"
#include "WindowInspector.h"
#include "WindowHierarchy.h"
#include "WindowSceneWorld.h"
#include "WindowProject.h"
#include "ModuleImporter.h"
#include "ImportScript.h"
#include "GameObject.h"
#include "ModuleAudio.h"
#include "ModuleMap.h"
#include "ModuleEventSystem.h"
#include "ShadersLib.h"
#include "EventDef.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl_gl3.h"
#include "ImGui/imgui_dock_v2.h"
#include "Gl3W/include/glew.h"
#include "Algorithm/Random/LCG.h"
#include "SDL/include/SDL.h"
#include "ModuleShaders.h"

ModuleGUI::ModuleGUI(bool start_enabled): Module(start_enabled)
{
	Start_enabled = true;
	Update_enabled = true;
	have_config = true;

	name = "Gui";
}

ModuleGUI::~ModuleGUI()
{
	for (int i = 0; i < win_manager.size(); i++)
	{
		delete win_manager[i];
	}
	win_manager.clear();
}


bool ModuleGUI::Init(JSON_Object* node)
{
	win_manager.push_back(new Hardware());		//0---- HARDWARE
	win_manager.push_back(new Inspector());		//1---- INSPECTOR
	win_manager.push_back(new Hierarchy());		//2---- Hierarchy
	win_manager.push_back(new SceneWorld());		//3---- SceneWorld
	win_manager.push_back(new Project());		//4---- Project

												//TODO ELLIOT NEED ACTIVE bye JSON, Also Console
	win_manager[INSPECTOR]->active[0].active = true;
	win_manager[HIERARCHY]->active[0].active = true;
	win_manager[SCENEWORLD]->active[0].active = true;
	win_manager[PROJECT]->active[0].active = true;

	return true;
}

bool ModuleGUI::Start()
{
	perf_timer.Start();
	
	LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	ImGuiIO& io{ ImGui::GetIO() };

	io.Fonts->AddFontFromFileTTF("Fonts\\Ruda-Bold.ttf", 15);
	io.Fonts->AddFontDefault();

	App->scene->scene_buff = new FrameBuffer();
	App->scene->scene_buff->Create(App->window->GetWidth(), App->window->GetHeight());

	std::vector<WindowManager*>::iterator window = win_manager.begin();
	for (int i = 0; i < win_manager.size(); i++)
	{
		window[i]->Start();
	}

	LoadDocks();

	Start_t = perf_timer.ReadMs();
	return true;
}

//update_status ModuleGUI::PreUpdate(float dt)
//{
//	perf_timer.Start();
//
//	preUpdate_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}

update_status ModuleGUI::Update(float dt)
{
	perf_timer.Start();

	//ShowTest -----------------------
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		show_test = !show_test;
	}
	if (show_test)
	{
		ImGui::ShowTestWindow();
	}

	// Main Menu --------------------------------
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", "Ctrl + N"))
			{
				if (App->scene->scene_saved)
				{
					App->scene->DeleteAllGameObjects(App->scene->root);
					App->event_system->ClearEvents(EventType::EVENT_DRAW);
					App->scene->root->SetName("NewScene");
					App->scene->CreateMainCamera(nullptr);
					App->resource_manager->ReImportAllScripts();
				}
			}
			if (ImGui::MenuItem("Open Scene", "Ctrl + O", false, false))
			{

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
			{
				App->WantToSave();
			}
			if (ImGui::MenuItem("Save Scene as...", false, false))
			{
			}
			if (ImGui::MenuItem("Load Scene"))
			{
				App->WantToLoad();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", NULL, &window_exit))
			{
				if (is_saved)
				{
					ImGui::EndMenu();
					ImGui::EndMainMenuBar();
					return UPDATE_STOP;
				}
				else
				{
					reposition_exit = true;
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			//if (ImGui::MenuItem("Undo", "Ctrl + Z"))
			//{

			//}
			//if (ImGui::MenuItem("Redo", "Ctrl + Y"))
			//{

			//}
			//ImGui::Separator();
			//if (ImGui::MenuItem("Cut", "Ctrl + X"))
			//{

			//}
			//if (ImGui::MenuItem("Copy", "Ctrl + C"))
			//{

			//}
			//if (ImGui::MenuItem("Paste", "Ctrl + V"))
			//{

			//}
			//ImGui::Separator();
			if (ImGui::MenuItem("Look View to Selected", "F"))
			{
				App->camera->CenterToObject();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Create Empty", "Ctrl+Shift+N"))
			{
				// Link inspector to the GameObject Created to call its Inspector window.
				GameObject* empty = App->scene->CreateGameObject();
				App->gui->SetLinkInspector(empty);
				App->camera->SetFocus(empty);
			}
			ImGui::MenuItem("Create Empty Child", "Alt+Shift+N", false, false);
			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					// Link inspector to the GameObject Created to call its Inspector window.
					GameObject* cube = App->scene->CreateCube();
					App->gui->SetLinkInspector(cube);
					App->camera->SetFocus(cube);
				}
				ImGui::MenuItem("Create Sphere", NULL, false, false);
				ImGui::MenuItem("Create Capsule", NULL, false, false);
				ImGui::MenuItem("Create Cylinder", NULL, false, false);
				if (ImGui::MenuItem("Create Plane")) {
					GameObject* plane = App->scene->CreatePlane();
					App->gui->SetLinkInspector(plane);
					App->camera->SetFocus(plane);
				}
				// CreateNewScript
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("UI"))
			{
				if (ImGui::MenuItem("Text", NULL, false, false))
				{
					//GameObject* cube = App->scene->CreateCube();
					//App->gui->SetLinkInspector(cube);
				}
				ImGui::MenuItem("Image", NULL, false, false);
				ImGui::MenuItem("Button", NULL, false, false);
				ImGui::MenuItem("Canvas", NULL, false, false);
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("C# Script"))
			{
				window_create_new_script = !window_create_new_script;
			}

			if (ImGui::BeginMenu("Shaders"))
			{
				if (ImGui::MenuItem("Create Shader Object"))
				{
					shader_obj_creation = true;
				}
				if (ImGui::MenuItem("Create Shader Program"))
				{
					shader_program_creation_UI = true;
					shader_program_creation = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Material"))
			{
				material_creation = true;
				
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Component", false))
		{
			ImGui::MenuItem("Add...", NULL, false, false);
			ImGui::MenuItem("Mesh", NULL, false, false);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Inspector"))
			{
				win_manager[INSPECTOR]->active[0].OpenClose();
				//LogOpenCloseWindow(winManager[INSPECTOR]->active[0].active, winManager[INSPECTOR]->name);
			}
			if (ImGui::MenuItem("Hardware"))
			{
				win_manager[HARDWARE]->active[0].OpenClose();
				//LogOpenCloseWindow(winManager[HARDWARE]->active[0].active, winManager[HARDWARE]->name);
			}
			if (ImGui::MenuItem("Console", "�"))
			{
				App->console->OpenClose();
				//LogOpenCloseWindow(App->console->console_activated, App->console->name);
			}
			if (ImGui::MenuItem("Scene"))
			{
				win_manager[SCENEWORLD]->active[0].OpenClose();
				//LogOpenCloseWindow(winManager[SCENEWORLD]->active[0].active, winManager[SCENEWORLD]->name);
			}
			if (ImGui::MenuItem("Hierarchy"))
			{
				win_manager[HIERARCHY]->active[0].OpenClose();
				//LogOpenCloseWindow(winManager[HIERARCHY]->active[0].active, winManager[HIERARCHY]->name);
			}
			if (ImGui::MenuItem("Audio Banks"))
			{
				window_audio = !window_audio;
			}
			if (ImGui::MenuItem("Project"))
			{
				win_manager[PROJECT]->active[0].OpenClose();
				//LogOpenCloseWindow(winManager[PROJECT]->active[0].active, winManager[PROJECT]->name);
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Info Mouse", NULL, &window_infoMouse))
			{
				//LogOpenCloseWindow(window_infoMouse, std::string("Info Mouse"));
			}
			if (ImGui::MenuItem("Style Editor"))
			{
				window_style = !window_style;
				//LogOpenCloseWindow(window_style, std::string("Style Editor"));
			}
			if (ImGui::MenuItem("Random Generator"))
			{
				window_random_generator = !window_random_generator;
				//LogOpenCloseWindow(window_Random_generator, std::string("Random Generator"));
			}
			if (ImGui::MenuItem("Edit Map Tiles"))
			{
				window_create_map = !window_create_map;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Resources"))
			{
				window_show_all_resources = !window_show_all_resources;
			}
			if (ImGui::MenuItem("Performance"))
			{
				App->show_performance = !App->show_performance;
				//LogOpenCloseWindow(App->showperformance, std::string("Performance"));
			}
			if (ImGui::MenuItem("Configuration"))
			{
				App->show_config = !App->show_config;
				//LogOpenCloseWindow(App->showconfig, std::string("Configuration"));
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About Culverin..."))
			{
				window_about_us = !window_about_us;
				LogOpenCloseWindow(window_about_us, std::string("About Culverin"));
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Documentation 'GitHub'"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/elliotjb/3D-Engine/", NULL, NULL, SW_SHOWNORMAL);
			}
			if (ImGui::MenuItem("Download latest"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/elliotjb/3D-Engine/releases", NULL, NULL, SW_SHOWNORMAL);
			}
			if (ImGui::MenuItem("Report a bug"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/elliotjb/3D-Engine/issues/new", NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::Separator();

			if (ImGui::MenuItem("README"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/elliotjb/3D-Engine/blob/master/README.md", NULL, NULL, SW_SHOWNORMAL);
			}
			if (ImGui::BeginMenu("Revert Style"))
			{
				if (ImGui::MenuItem("Revert Style Docking"))
				{
					RevertStyleDocks();
				}
				if (ImGui::MenuItem("Revert Style Colors and format"))
				{
					
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		menu_height = ImGui::GetWindowSize().y;
		ImGui::EndMainMenuBar();
	}

	//Machine Generator -------------------
	if (window_random_generator)
	{
		static LCG random_generator;
		ImGui::Begin("Random Numbers Generator", &window_random_generator, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Spacing();
		ImGui::PushItemWidth(60);
		static int numbers_f = 0;
		static int numbers_i = 0;
		if (ImGui::Button("FLOAT"))
			numbers_f++;
		if (ImGui::Button("INT"))
			numbers_i++; ImGui::SameLine();

		static int n1 = 0;
		static int n2 = 0;
		ImGui::PushItemWidth(100);
		ImGui::InputInt("Min", &n1, 1, 0, 1); ImGui::SameLine();
		ImGui::InputInt("Max", &n2, 1, 0, 1);

		if (numbers_f & 1)
		{
			numbers_f++;
			floats_list.push_back(random_generator.Float());
			//ImGui::Text("%.4f", random_generator.Float())
		}

		if (numbers_i & 1)
		{
			if (n1 <= n2)
			{
				numbers_i++;
				ints_list.push_back(random_generator.Int(n1, n2));
			}
		}

		static int clear = 0;
		if (ImGui::Button("CLEAR"))
			clear++;
		if (clear & 1)
		{
			clear++;
			if (floats_list.size() > 0)
			{
				floats_list.clear();
			}

			if (ints_list.size() > 0)
			{
				ints_list.clear();
			}
		}

		ImGui::PopItemWidth();
		ImGui::Spacing();

		ImGui::Text("FLOATS --------------------");
		for (uint n = 0; n < floats_list.size(); n++)
		{
			ImGui::Text("%i - %.4f", n, floats_list[n]);
		}

		ImGui::Text("INTS --------------------");
		for (uint n = 0; n < ints_list.size(); n++)
		{
			ImGui::Text("%i - %i", n, ints_list[n]);
		}
		ImGui::End();
	}
	
	// Create New Script ------------------
	if (window_create_new_script)
	{
		App->importer->iScript->CreateNewScript(window_create_new_script);
	}
	//----------------------------------------------
	//MAP ----------------------
	if (window_create_map)
	{
		App->map->ShowEditorMap(window_create_map);
	}
	//----------------------------------------------

	// Window Creating Shader Object --------------------------------

	if (shader_obj_creation) {
		ImGui::Begin("Shader Object Definition", &shader_obj_creation);

		ImGui::InputText("Name of the Shader", str_shad_temp, 64);

		ImGui::Combo("Shaders Mode", &combo_shaders, "Vertex Shader\0Fragment Shader\0Geometry Shader");

		if (ImGui::Button("Create")) {

			Event shader_event;
			shader_event.shader_editor.type = EventType::EVENT_OPEN_SHADER_EDITOR;
			shader_event.shader_editor.name = str_shad_temp;

			switch (combo_shaders) {
			case 0:
				shader_event.shader_editor.shader_type = ShaderType::vertex;
				break;
			case 1:
				shader_event.shader_editor.shader_type = ShaderType::fragment;
				break;
			case 2:
				shader_event.shader_editor.shader_type = ShaderType::geometry;
				break;
			}
			shader_event.shader_editor.open_editor = true;
			PushEvent(shader_event);

			shader_obj_creation = false;
		}
			ImGui::End();
	
	}
	
	if (shader_program_creation_UI) 
	{

		ImGui::Begin("Shader Program Definition", &shader_program_creation_UI);

		ImGui::InputText("Name of the Shader", str_shad_prg_temp, 64);

		std::string shader_options;
		std::string name;

		for (int i = 0; i < vec_temp_shader.size(); i++) 
		{
			name = vec_temp_shader[i]->name;
			shader_options += name;
			shader_options += '\0';
		}

	

		ImGui::Combo("Shader 1:", &combo_shaders_obj, shader_options.c_str());

		ImGui::Combo("Shader 2:", &combo_shaders_obj2, shader_options.c_str());

		if (ImGui::Button("Create")) 
		{

			Event shader_event_prg;
			shader_event_prg.shader_program.type = EventType::EVENT_CREATE_SHADER_PROGRAM;
			shader_event_prg.shader_program.name = str_shad_prg_temp;

			if (combo_shaders_obj != -1)
			{
				shader_event_prg.shader_program.Shader1 = vec_temp_shader[combo_shaders_obj];
			}

			if (combo_shaders_obj2 != -1) {
				shader_event_prg.shader_program.Shader2 = vec_temp_shader[combo_shaders_obj2];
			}

			PushEvent(shader_event_prg);

			vec_temp_shader.clear();

			shader_program_creation_UI = false;
		}

		ImGui::End();
	}
	
	if (material_creation) {
		if (ImGui::Begin("Shader Program Definition", &material_creation))
		{
			ImGui::InputText("Name of the Material", str_mat_temp, 64);

			std::string material_options;
			std::string name;

			for (int i = 0; i < App->module_shaders->programs.size(); i++)
			{
				name = App->module_shaders->programs[i]->name;
				material_options += name;
				material_options += '\0';
			}

			static ShaderProgram* selected_shader_program = nullptr;

			if (ImGui::Combo("Program 1:", &combo_material_obj, material_options.c_str())) {
				selected_shader_program = App->module_shaders->programs[combo_material_obj];

			}

		

			if (ImGui::Button("Create") && selected_shader_program != nullptr)
			{

				Material* new_mat = new Material();
				new_mat->name = str_mat_temp;
				new_mat->material_shader = selected_shader_program;
				App->module_shaders->materials.push_back(new_mat);
				selected_shader_program = nullptr;
				new_mat->GetProgramVariables();
				new_mat->Save();
				material_creation = false;
			}

			

		}
		ImGui::End();

	}

	//-----------------------------------------------------------

	// Window About Us... ---------------------------------
	if (window_about_us)
	{
		if (!ImGui::Begin("About Culverin", &window_about_us, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::End();
			return UPDATE_CONTINUE;
		}
		//Name of your Engine
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Culverin Engine");

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		// Description
		ImGui::Text("This is a project consistent of creating a Game Engine,\nthat allows you to use tools to create video games.\nAt this moment, this engine works with a similar structure of Unity:\nGameObjects are containers of Components, and they\nhave a parent-children relation.\nYou can add/remove/edit these type of components:\n -Transform,\n -Material,\n -Mesh\n -Material.");
		ImGui::Text("SCRIPTING SYSTEM: this engine has integrated this high level system to create your own scripts and play with GameObects in the scene [info in the README]");
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		// Libraries
		ImGui::Text("Libraries used:");
		ImGui::Spacing();
		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("SDL - v2.0.7"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "http://www.sdl.com/", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("ImGui - v1.51"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("MathGeoLib - v1.5"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "http://clb.demon.fi/MathGeoLib/nightly/", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("Parson, JSON library"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "https://github.com/kgabis/parson", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("Open Asset Import Library (ASSIMP) - v3.1.1"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "http://assimp.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("DevIL - v1.7.8"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "http://openil.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("Mono"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "http://www.mono-project.com/", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("PhysX - v3.4.1"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "https://developer.nvidia.com/physx-sdk", NULL, NULL, SW_SHOWNORMAL);
			}
		}
		static GLuint icon_physics = App->textures->LoadTexture("Images/UI/nvidia_physX_logo.png");
		ImGui::Image((ImTextureID)icon_physics, ImVec2(256.f, 128.f), ImVec2(0,1),ImVec2(1,0));


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();		
		// Name of the Author
		ImGui::Text("Authors:");
		ImGui::Spacing();

		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("Elliot Jimenez Bosch"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "https://github.com/elliotjb", NULL, NULL, SW_SHOWNORMAL);
			}
		}

		ImGui::Bullet(); ImGui::SameLine();
		if (ImGui::MenuItem("Jordi Ona Rufi"))
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				ShellExecuteA(NULL, "open", "https://github.com/Jordior97", NULL, NULL, SW_SHOWNORMAL);
			}
		}

		ImGui::End();
	}

	// Show All Resources ------------------
	if (window_show_all_resources)
	{
		App->resource_manager->ShowAllResources(window_show_all_resources);
	}

	if (window_audio)
	{
		ImGui::Begin("Audio Banks", &window_audio);
		App->audio->DrawOnEditor();
		ImGui::End();
	}

	//Window Style -----------------------
	if (window_style)
	{
		ImGui::Begin("Style Editor", &window_style);
		ShowStyleEditor();
		ImGui::End();
	}

	// Windows EXIT ---------------------------------
	if (window_exit)
	{
		return UPDATE_STOP;
	}

	// ----------------------------------------------
	//Window Info Mouse inse scenworld
	if (window_infoMouse)
	{
		ShowInfoMouse(&window_infoMouse);
	}

	//Update All Modules ----------------------------------
	UpdateWindows(dt);

	Update_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

//Update Window Configuration -----------------------------
update_status ModuleGUI::UpdateConfig(float dt)
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));
	ImGui::Text("Windows:");
	ImGui::Checkbox("##SceneActive", &win_manager[SCENEWORLD]->active[0].active); ImGui::SameLine();
	ImGui::Text("Scene");
	ImGui::Checkbox("##InspectorActive", &win_manager[INSPECTOR]->active[0].active); ImGui::SameLine();
	ImGui::Text("Inspector");
	ImGui::Checkbox("##HierarchyActive", &win_manager[HIERARCHY]->active[0].active); ImGui::SameLine();
	ImGui::Text("Hierarchy");
	ImGui::Bullet();  ImGui::SameLine();
	ImGui::Text("Show Window Confirm Delete");  ImGui::SameLine(); 
	if(ImGui::Checkbox("##HierarchyConfirmDelete", &((Hierarchy*)win_manager[HIERARCHY])->show_confirm_delete))
	{
		((Hierarchy*)win_manager[HIERARCHY])->ChangeShowConfirmDelete();
	}
	ImGui::Checkbox("##ConsoleActive", &App->console->console_activated); ImGui::SameLine();
	ImGui::Text("Console");
	ImGui::Checkbox("##HardwareActive", &win_manager[HARDWARE]->active[0].active); ImGui::SameLine();
	ImGui::Text("Hardware");
	ImGui::Checkbox("##RandomActive", &window_random_generator); ImGui::SameLine();
	ImGui::Text("Random Generation");
	ImGui::Checkbox("##AboutActive", &window_about_us); ImGui::SameLine();
	ImGui::Text("About Culverin");
	ImGui::Checkbox("##PerformanceActive", &App->show_performance); ImGui::SameLine();
	ImGui::Text("Performance");
	ImGui::Checkbox("##StyleEsditorActive", &window_style); ImGui::SameLine();
	ImGui::Text("Style Editor");
	ImGui::Checkbox("##WindowInfoMouse", &window_infoMouse); ImGui::SameLine();
	ImGui::Text("Info Mouse");

	ImGui::PopStyleVar();


	return UPDATE_CONTINUE;
}

void ModuleGUI::LoadDocks()
{
	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file("ImGuiDock.json");

	config = json_value_get_object(config_file);
	config_node = json_object_get_object(config, "Docking");
	getDockContext()->LoadDock(config_node);
	json_value_free(config_file);
}

void ModuleGUI::RevertStyleDocks()
{
	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file("ImGuiDock_style.json");

	config = json_value_get_object(config_file);
	config_node = json_object_get_object(config, "Docking");
	getDockContext()->LoadDock(config_node);
	json_value_free(config_file);
}

bool ModuleGUI::CleanUp()
{
	//Save config ImGui Docks ------------------
	//JSON_Value* config_file;
	//JSON_Object* config;
	//JSON_Object* config_node;

	//config_file = json_parse_file("ImGuiDock.json"); 

	//config = json_value_get_object(config_file);
	//config_node = json_object_get_object(config, "Docking");
	//getDockContext()->SaveDock(config_node);

	//json_serialize_to_file(config_file, "ImGuiDock.json");


	return true;
}

void ModuleGUI::ShowStyleEditor(ImGuiStyle* ref) //TODO need reposition
{
	ImGuiStyle& style = ImGui::GetStyle();

	// You can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it compares to the default style)
	const ImGuiStyle default_style; // Default style
	if (ImGui::Button("Revert Style"))
	{
		style = ref ? *ref : default_style;
	}
	if (ref)
	{
		ImGui::SameLine();
		if (ImGui::Button("Save Style"))
			*ref = style;
	}

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.55f);

	if (ImGui::TreeNode("Rendering"))
	{
		ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines); ImGui::SameLine(); App->ShowHelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
		ImGui::Checkbox("Anti-aliased shapes", &style.AntiAliasedFill);
		ImGui::PushItemWidth(100);
		ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, NULL, 2.0f);
		if (style.CurveTessellationTol < 0.0f) style.CurveTessellationTol = 0.10f;
		ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Settings"))
	{
		ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 16.0f, "%.0f");
		ImGui::SliderFloat("ChildWindowRounding", &style.ChildRounding, 0.0f, 16.0f, "%.0f");
		ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 16.0f, "%.0f");
		ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
		ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
		ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
		ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 16.0f, "%.0f");
		ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
		ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 16.0f, "%.0f");
		ImGui::Text("Alignment");
		ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); App->ShowHelpMarker("Alignment applies when a button is larger than its text content.");
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Colors"))
	{
		static int output_dest = 0;
		static bool output_only_modified = false;
		if (ImGui::Button("Copy Colors"))
		{
			if (output_dest == 0)
				ImGui::LogToClipboard();
			else
				ImGui::LogToTTY();
			ImGui::LogText("ImGuiStyle& style = ImGui::GetStyle();" IM_NEWLINE);
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const ImVec4& col = style.Colors[i];
				const char* name = ImGui::GetStyleColorName(i);
				if (!output_only_modified || memcmp(&col, (ref ? &ref->Colors[i] : &default_style.Colors[i]), sizeof(ImVec4)) != 0)
					ImGui::LogText("style.Colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE, name, 22 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
			}
			ImGui::LogFinish();
		}
		ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); ImGui::PopItemWidth();
		ImGui::SameLine(); ImGui::Checkbox("Only Modified Fields", &output_only_modified);

		ImGui::Text("Tip: Left-click on colored square to open color picker,\nRight-click to open edit options menu.");

		static ImGuiTextFilter filter;
		filter.Draw("Filter colors", 200);

		static ImGuiColorEditFlags alpha_flags = 0;
		ImGui::RadioButton("Opaque", &alpha_flags, 0); ImGui::SameLine();
		ImGui::RadioButton("Alpha", &alpha_flags, ImGuiColorEditFlags_AlphaPreview); ImGui::SameLine();
		ImGui::RadioButton("Both", &alpha_flags, ImGuiColorEditFlags_AlphaPreviewHalf);

		ImGui::BeginChild("#colors", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		ImGui::PushItemWidth(-160);
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName(i);
			if (!filter.PassFilter(name))
				continue;
			ImGui::PushID(i);
			ImGui::ColorEdit4(name, (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
			if (memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &default_style.Colors[i]), sizeof(ImVec4)) != 0)
			{
				ImGui::SameLine(); if (ImGui::Button("Revert")) style.Colors[i] = ref ? ref->Colors[i] : default_style.Colors[i];
				if (ref) { ImGui::SameLine(); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i]; }
			}
			ImGui::PopID();
		}
		ImGui::PopItemWidth();
		ImGui::EndChild();

		ImGui::TreePop();
	}

	bool fonts_opened = ImGui::TreeNode("Fonts", "Fonts (%d)", ImGui::GetIO().Fonts->Fonts.Size);
	ImGui::SameLine(); App->ShowHelpMarker("Tip: Load fonts with io.Fonts->AddFontFromFileTTF()\nbefore calling io.Fonts->GetTex* functions.");
	if (fonts_opened)
	{
		ImFontAtlas* atlas = ImGui::GetIO().Fonts;
		if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
		{
			ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			ImGui::TreePop();
		}
		ImGui::PushItemWidth(100);
		for (int i = 0; i < atlas->Fonts.Size; i++)
		{
			ImFont* font = atlas->Fonts[i];
			bool font_details_opened = ImGui::TreeNode(font, "Font %d: \'%s\', %.2f px, %d glyphs", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size);
			ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) ImGui::GetIO().FontDefault = font;
			if (font_details_opened)
			{
				ImGui::PushFont(font);
				ImGui::Text("The quick brown fox jumps over the lazy dog");
				ImGui::PopFont();
				ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
				ImGui::SameLine(); App->ShowHelpMarker("Note than the default embedded font is NOT meant to be scaled.\n\nFont are currently rendered into bitmaps at a given size at the time of building the atlas. You may oversample them to get some flexibility with scaling. You can also render at multiple sizes and select which one to use at runtime.\n\n(Glimmer of hope: the atlas system should hopefully be rewritten in the future to make scaling more natural and automatic.)");
				ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
				ImGui::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
				ImGui::Text("Texture surface: %d pixels (approx) ~ %dx%d", font->MetricsTotalSurface, (int)sqrtf((float)font->MetricsTotalSurface), (int)sqrtf((float)font->MetricsTotalSurface));
				for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
				{
					ImFontConfig* cfg = &font->ConfigData[config_i];
					ImGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
				}
				if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
				{
					// Display all glyphs of the fonts in separate pages of 256 characters
					const ImFont::Glyph* glyph_fallback = font->FallbackGlyph; // Forcefully/dodgily make FindGlyph() return NULL on fallback, which isn't the default behavior.
					font->FallbackGlyph = NULL;
					for (int base = 0; base < 0x10000; base += 256)
					{
						int count = 0;
						for (int n = 0; n < 256; n++)
							count += font->FindGlyph((ImWchar)(base + n)) ? 1 : 0;
						if (count > 0 && ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
						{
							float cell_spacing = style.ItemSpacing.y;
							ImVec2 cell_size(font->FontSize * 1, font->FontSize * 1);
							ImVec2 base_pos = ImGui::GetCursorScreenPos();
							ImDrawList* draw_list = ImGui::GetWindowDrawList();
							for (int n = 0; n < 256; n++)
							{
								ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size.x + cell_spacing), base_pos.y + (n / 16) * (cell_size.y + cell_spacing));
								ImVec2 cell_p2(cell_p1.x + cell_size.x, cell_p1.y + cell_size.y);
								const ImFont::Glyph* glyph = font->FindGlyph((ImWchar)(base + n));;
								draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
								font->RenderChar(draw_list, cell_size.x, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
								if (glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2))
								{
									ImGui::BeginTooltip();
									ImGui::Text("Codepoint: U+%04X", base + n);
									ImGui::Separator();
									ImGui::Text("XAdvance+1: %.1f", glyph->AdvanceX);
									ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
									ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
									ImGui::EndTooltip();
								}
							}
							ImGui::Dummy(ImVec2((cell_size.x + cell_spacing) * 16, (cell_size.y + cell_spacing) * 16));
							ImGui::TreePop();
						}
					}
					font->FallbackGlyph = glyph_fallback;
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		static float window_scale = 1.0f;
		ImGui::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.1f");              // scale only this window
		ImGui::DragFloat("global scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.1f"); // scale everything
		ImGui::PopItemWidth();
		ImGui::SetWindowFontScale(window_scale);
		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

void ModuleGUI::LogOpenCloseWindow(bool active, std::string name)
{
	if (active)
	{
		LOG("Opened %s Window...", name.c_str());
	}
	else
	{
		LOG("Closed %s Window...", name.c_str());
	}
}

void ModuleGUI::SetLinkInspector(GameObject* obj)
{
	((Inspector*)win_manager[INSPECTOR])->LinkObject(obj);
}

void ModuleGUI::UpdateWindows(float dt)
{
	static bool show_scene3 = true;
	//DockContext::Dock::setPosSize(ImVec2(0, 0), ImVec2(100,100));
	static int width;
	static int height;
	SDL_GetWindowSize(App->window->window, &width, &height);
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(ImVec2(width, height - 20));


	ImGui::Begin("MasterWindow", &show_scene3, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoFocusOnAppearing);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.211f, 0.211f, 0.211f, 1.00f));
	if (ImGui::BeginChild(ImGui::GetID("MasterWindow"), ImVec2(ImGui::GetWindowWidth(), 30), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		static GLuint icon_play = App->textures->LoadTexture("Images/UI/IconPlay.png");
		static GLuint icon_pause = App->textures->LoadTexture("Images/UI/IconPause.png");
		static GLuint icon_playframe = App->textures->LoadTexture("Images/UI/IconPlayFrame.png");
		//ImTextureID temp = play;
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(150, 150, 150, 0));
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(255, 0, 0, 255));
		//ImGui::Image((ImTextureID*)play, ImVec2(300, 60));
		static ImVec2 pos_icon(20, 20);
		ImGui::SameLine(width / 2 - 40);
		if (ImGui::ImageButton((ImTextureID*)icon_play, pos_icon))
		{
			if (App->scene->CheckNoFails())
			{
				int exc = App->engine_state;
				App->SetState(EngineState::PLAY); // OR STOP
				// -------------
				//App->importer->iScript->ClearMonoMap();

				if (exc == EngineState::STOP)
				{
					// First add call
					App->importer->iScript->SetMonoMap(App->scene->root, true);
					//Start all scripts
					App->scene->StartScripts();
				}
			}
		}
		ImGui::SameLine(width / 2 - 10);
		if (ImGui::ImageButton((ImTextureID*)icon_pause, pos_icon))
		{
			App->SetState(EngineState::PAUSE);
		}
		ImGui::SameLine(width / 2 + 20);
		if (ImGui::ImageButton((ImTextureID*)icon_playframe, pos_icon))
		{
			App->SetState(EngineState::PLAYFRAME);
			App->game_time.prepare_frame = true;
		}
		ImGui::PopStyleColor(1);

		//Text that indicates the state of the engine (PLAY/PAUSE/PLAYFRAME)
		if (App->engine_state != EngineState::STOP)
		{
			ImGui::SameLine(width * 0.5 + 60);
			ShowEngineState();
		}
		if (App->show_camera_popup)
		{
			ShowCameraMissing();
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();


	BeginWorkspace();

	App->Config();

	std::vector<WindowManager*>::iterator window = win_manager.begin();
	for (int i = 0; i < win_manager.size(); i++)
	{
		window[i]->Update(dt);
	}

	if (App->console->IsOpen())
	{
		ShowExampleAppConsole();
	}

	EndWorkspace();
	ImGui::End();
}

void ModuleGUI::ShowExampleAppConsole()
{
	App->console->Draw("Console");
}


void ModuleGUI::ShowHardware()
{
}

void ModuleGUI::ShowInfoMouse(bool* active)
{
	
	ImGui::SetNextWindowPos(ImVec2(GetPositionDock("Scene").x + 20, GetPositionDock("Scene").y + 50));
	if (!ImGui::Begin("Info", active, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | 
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	ImGui::Text("FPS: %.0f", App->fps_log[App->frame_index - 1]);
	ImGui::End();
}

void ModuleGUI::ShowWindowExit(bool* active)
{
	static int width = 0;
	static int height = 0;

	if (reposition_exit)
	{
		SDL_GetWindowSize(App->window->window, &width, &height);
		ImGui::SetNextWindowPos(ImVec2(width / 2 - 180, height / 2 - 80));
		reposition_exit = false;
	}
	if (!ImGui::Begin("Scene(s) Have Been Modified", active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Do you want to save the changes...");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Your changes will be lost if you don't save them.");
	ImGui::Spacing();
	if (ImGui::Button("Save"))
	{
		reposition_exit = true;
		is_saved = true;
		game_save = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Don't Save"))
	{
		reposition_exit = true;
		is_saved = true;
		game_save = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
	{
		reposition_exit = true;
		*active = false;
	}
	if (*active == false)
	{
		reposition_exit = true;
	}
	ImGui::End();
}

void ModuleGUI::ShowCameraMissing()
{
	ImGui::OpenPopup("Active Game Camera Missing!");
	if (ImGui::BeginPopupModal("Active Game Camera Missing!", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("There isn't any Game Camera Activated! Make sure that one\nof them is active before clicking 'Play' Button.");
		float width = ImGui::GetItemRectSize().x * 0.5;
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text(""); ImGui::SameLine(width - 20);
		if (ImGui::Button("OK", ImVec2(40, 20)))
		{
			App->show_camera_popup = false;
		}
	}
	ImGui::EndPopup();
}

void ModuleGUI::ShowEngineState()
{
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.25f, 1.00f, 0.00f, 1.00f));
	if (App->engine_state == EngineState::PLAY)
	{
		ImGui::Text("PLAY");
	}
	else if (App->engine_state == EngineState::PAUSE)
	{
		ImGui::Text("PAUSE");
	}
	else if (App->engine_state == EngineState::PLAYFRAME)
	{
		ImGui::Text("PLAYFRAME");
	}
	ImGui::PopStyleColor();
}

void ModuleGUI::ShowCreateNewScriptWindow()
{
	window_create_new_script = true;
}

bool ModuleGUI::SetEventListenrs()
{
	AddListener(EventType::EVENT_SEND_ALL_SHADER_OBJECTS, this);
	return true;
}

void ModuleGUI::OnEvent(Event& event)
{
	switch (event.type)
	{
	case EventType::EVENT_SEND_ALL_SHADER_OBJECTS:
		//need to fix std::pair
		vec_temp_shader = *event.send_shader_object.shaders;
		break;
	}
}