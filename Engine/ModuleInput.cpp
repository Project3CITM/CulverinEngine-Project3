#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleFS.h"
#include "ModuleGUI.h"
#include "WindowProject.h"
#include "ModuleResourceManager.h"

#include "ImGui/imgui_impl_sdl_gl3.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	preUpdate_enabled = true;

	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);

	name = "Input";
	haveConfig = true;
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init(JSON_Object* node)
{
	perf_timer.Start();

	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	Awake_t = perf_timer.ReadMs();
	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	perf_timer.Start();

	SDL_PumpEvents();
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	// No Restrictios With Move Camera ----------------------------------
	mouse_x_motion_global = mouse_x_global;
	mouse_y_motion_global = mouse_y_global;
	Uint32 buttons_global = SDL_GetGlobalMouseState(&mouse_x_global, &mouse_y_global);
	mouse_x_motion_global = mouse_x_global - mouse_x_motion_global;
	mouse_y_motion_global = mouse_y_global - mouse_y_motion_global;

	//Cap Error?
	if (mouse_x_motion_global > 600 || mouse_x_motion_global < -600)
	{
		mouse_x_motion_global = 0;
	}
	if (mouse_y_motion_global > 600 || mouse_y_motion_global < -600)
	{
		mouse_y_motion_global = 0;
	}

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(/*buttons*/buttons_global & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSdlGL3_ProcessEvent(&e);

		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;
			
			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;


			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;

			break;

			case SDL_DROPFILE:
			{
				if (App->resource_manager->CheckFileType(e.drop.file) == Resource::Type::MESH)
				{
					LOG("Importing file: %s", e.drop.file);
					dropedfiles.push_back(e.drop.file);
				}
				else if (App->resource_manager->CheckFileType(e.drop.file) == Resource::Type::MATERIAL)
				{
					LOG("Importing file: %s", e.drop.file);
					dropedfiles.push_front(e.drop.file);
				}
				else if (App->resource_manager->CheckFileType(e.drop.file) == Resource::Type::SCRIPT)
				{
					LOG("Importing file: %s", e.drop.file);
					dropedfiles.push_back(e.drop.file);
				}
				else if (App->resource_manager->CheckFileType(e.drop.file) == Resource::Type::FOLDER)
				{
					LOG("Importing file: %s", e.drop.file);
					dropedfiles.push_back(e.drop.file);
				}
			}
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
				if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
				{
					// Now Update All vectors "project" and "fs"
					((Project*)App->gui->winManager[PROJECT])->UpdateNow();
					App->fs->CheckNowfilesAssets();
				}
			}
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

int ModuleInput::GetMouseXMotionNormalized()
{
	int x_motion = App->input->GetMouseXMotion();
	if (x_motion > 0)
	{
		return 1;
	}
	if (x_motion < 0)
	{
		return -1;
	}
	return 0;
}

int ModuleInput::GetMouseYMotionNormalized()
{
	int y_motion = App->input->GetMouseYMotion();
	if (y_motion > 0)
	{
		return 1;
	}
	if (y_motion < 0)
	{
		return -1;
	}
	return 0;
}

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

update_status ModuleInput::UpdateConfig(float dt)
{
	ImGui::Text("Mouse Data");
	ImGui::Spacing();
	ImGui::Text("Mouse Position:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "(%i, %i)", mouse_x, mouse_y);
	ImGui::Text("Mouse Motion:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "(%i, %i)", mouse_x_motion, mouse_y_motion);
	ImGui::Separator();
	ImGui::Text("Mouse Click"); ImGui::SameLine();
	//ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "%i", SDL_GetMouseState(NULL, NULL));
	int but = SDL_GetMouseState(NULL, NULL);
	switch (but)
	{
	case 1:
		ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "Left");
		break;
	case 2:
		ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "Middle");
		break;
	case 4:
		ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "Right");
		break;
	case 5:
		ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "Left - Right");
		break;
	default:
		ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "NoN");
		break;
	}

	//ImGui::Text("Keyboard Click");
	//ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "%i", SDL_GetKeyboardState(NULL));
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

SDL_Scancode ModuleInput::GetKeyFromName(const char* name)
{
	return SDL_GetScancodeFromName(name);
}
