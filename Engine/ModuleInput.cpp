#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleFS.h"
#include "ModuleGUI.h"
#include "WindowProject.h"
#include "ModuleMap.h"
#include "ModuleResourceManager.h"
#include "SDL2_ttf/include/SDL_ttf.h"
#include "ImGui/imgui_impl_sdl_gl3.h"
#include "ModuleEventSystem.h"
#include "PlayerActions.h"
#include "InputManager.h"
#include "JSONSerialization.h"
#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	preUpdate_enabled = true;

	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);

	name = "Input";
	have_config = true;
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
	TTF_Init();

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
		LOG("Error on SDL_Init: GameController");

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	//SDL_SetWindowGrab(App->window->window, SDL_TRUE); // "Get relative moution"
	//SDL_SetRelativeMouseMode(SDL_TRUE);


	//SDL_Cursor* cursor;
	//cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	//SDL_SetCursor(cursor);
	//SDL_ShowCursor(SDL_ENABLE);

	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			controller = SDL_GameControllerOpen(i);
			if (controller == nullptr)
				LOG("Gamepad not opened %s", SDL_GetError());

		}

	}
	ui_input_manager = json_object_get_string(node, "UI InputManager");
	submit = json_object_get_string(node, "Submit");
	cancel = json_object_get_string(node, "Cancel");
	vertical = json_object_get_string(node, "Vertical");
	horizontal = json_object_get_string(node, "Horizontal");

	player_action = new PlayerActions(this);
	std::string name = App->fs->GetMainDirectory();
	name += "/";
	name += "player_action.json";
	if (App->fs->CheckIsFileExist(name.c_str()))
		App->json_seria->LoadPlayerAction(&player_action, name.c_str());
	else
	{
		std::string name = App->fs->GetMainDirectory();
		name += "/";
		name += "player_action_default.json";
		App->json_seria->LoadPlayerAction(&player_action, name.c_str());
	}

	ui_manager = player_action->GetInputManager(ui_input_manager.c_str());
	if (ui_manager != nullptr)
	{
		//ui_conected = t;
			ui_conected=ui_manager->ActionExist(submit.c_str());
		if (ui_conected == true)
			ui_conected = ui_manager->ActionExist(cancel.c_str());
		if (ui_conected == true)
			ui_conected = ui_manager->ActionExist(vertical.c_str());
		if (ui_conected == true)
			ui_conected = ui_manager->ActionExist(horizontal.c_str());

	}

	
	Awake_t = perf_timer.ReadMs();
	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	perf_timer.Start();

	SDL_PumpEvents();
	player_action->UpdateInputsManager();

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
	ImGuiIO& io = ImGui::GetIO();
	mouse_y = io.DisplaySize.y - mouse_y;
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
		//if (App->mode_game) {
			player_action->ReceiveEvent(&e);
		//}
		switch(e.type)
		{

			case SDL_MOUSEBUTTONDOWN:
			{

				//LOG("mouse down");
				mouse_x = e.motion.x / SCREEN_SIZE;
				mouse_y = e.motion.y / SCREEN_SIZE;

				Event mouse_event;
				mouse_event.pointer.type = EventType::EVENT_BUTTON_DOWN;
				if (e.button.button == SDL_BUTTON_LEFT)
					mouse_event.pointer.button = EPoint::InputButton::INPUT_MOUSE_LEFT;
				if (e.button.button == SDL_BUTTON_MIDDLE)
					mouse_event.pointer.button = EPoint::InputButton::INPUT_MOUSE_MIDDLE;
				if (e.button.button == SDL_BUTTON_RIGHT)
					mouse_event.pointer.button = EPoint::InputButton::INPUT_MOUSE_RIGHT;
				mouse_event.pointer.position.x = mouse_x;
				mouse_event.pointer.position.y = mouse_y;
				mouse_event.pointer.focus = nullptr;
				PushEvent(mouse_event);
			}
			break;
			case SDL_MOUSEBUTTONUP:
			{
				mouse_x = e.motion.x / SCREEN_SIZE;
				mouse_y = e.motion.y / SCREEN_SIZE;
				Event mouse_event;
				mouse_event.pointer.type = EventType::EVENT_BUTTON_UP;
				if (e.button.button == SDL_BUTTON_LEFT)
					mouse_event.pointer.button = EPoint::InputButton::INPUT_MOUSE_LEFT;
				if (e.button.button == SDL_BUTTON_MIDDLE)
					mouse_event.pointer.button = EPoint::InputButton::INPUT_MOUSE_MIDDLE;
				if (e.button.button == SDL_BUTTON_RIGHT)
					mouse_event.pointer.button = EPoint::InputButton::INPUT_MOUSE_RIGHT;
				mouse_event.pointer.position.x = mouse_x;
				mouse_event.pointer.position.y = mouse_y;
				mouse_event.pointer.focus = nullptr;

				PushEvent(mouse_event);
			}
			break;
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;
			
			case SDL_MOUSEMOTION:
			
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;


			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			{
				Event mouse_event;
				mouse_event.pointer.type = EventType::EVENT_MOUSE_MOTION;
				mouse_event.pointer.position.x = mouse_x;
				mouse_event.pointer.position.y = mouse_y;
				mouse_event.pointer.motion.x = mouse_x_motion;
				mouse_event.pointer.motion.y = mouse_y_motion;
				mouse_event.pointer.focus = nullptr;

				PushEvent(mouse_event);
			}
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
				else if (App->resource_manager->CheckFileType(e.drop.file) == Resource::Type::FONT)
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
				else if (App->map->CheckTypeMap(e.drop.file) != TypeMap::MAP_NON)
				{
					App->map->imported_map = e.drop.file;
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
					if (App->mode_game == false)
					{
						((Project*)App->gui->win_manager[PROJECT])->UpdateNow();
						App->fs->CheckNowfilesAssets();
					}
				}
			}
		}
	}
	UIInputManagerUpdate();
	//LOG(std::to_string(mouse_x_motion).c_str());
	
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
	ImGui::Separator();
	ImGui::Text("Player Actions");
	ImGui::Separator();
	player_action->UpdateConfig(dt);
	ImGui::Separator();
	ImVec4 color;
	if (ui_conected)
	{
		color = ImVec4(0.4f, 1.0f, 0.4f, 1.00f);
		ImGui::TextColored(color, "Conection Success");

	}
	else
	{
		color = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
		ImGui::TextColored(color, "Conection Fail");

	}
	ImGui::TextColored(color,"UI InputManager: %s", ui_input_manager.c_str());
	ImGui::TextColored(color,"UI Vertical: %s", vertical.c_str());
	ImGui::TextColored(color,"UI Horizontal: %s", horizontal.c_str());
	ImGui::TextColored(color,"UI Submit: %s", submit.c_str());
	ImGui::TextColored(color,"UI Cancel: %s", cancel.c_str());
	//ImGui::Text("Keyboard Click");
	//ImGui::TextColored(ImVec4(0.0f, 0.58f, 1.0f, 1.0f), "%i", SDL_GetKeyboardState(NULL));
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	App->json_seria->SavePlayerAction(player_action, App->fs->GetMainDirectory().c_str() , "player_action");
	player_action->Clear();
	RELEASE(player_action);
	player_action = nullptr;
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	TTF_Quit();
	return true;
}

void ModuleInput::UIInputManagerUpdate()
{
	if (!ui_conected|| ui_manager==nullptr)
		return;
	if (!ui_manager->GetActiveInput())
		return;

	std::vector<KeyAction*> vect_temp = ui_manager->GetKey(submit.c_str());
	for (int i = 0; i < vect_temp.size(); i++)
	{
		if (vect_temp[i]->OnClick())
		{
			Event mouse_event;
			mouse_event.gui_submit.type = EventType::EVENT_SUBMIT;
			mouse_event.gui_submit.active = true;
			PushEvent(mouse_event);

		}
	}
	vect_temp.clear();
	vect_temp = ui_manager->GetKey(cancel.c_str());
	for (int i = 0; i < vect_temp.size(); i++)
	{
		if (vect_temp[i]->OnClick())
		{

				Event mouse_event;
				mouse_event.gui_cancel.type = EventType::EVENT_CANCEL;
				mouse_event.gui_cancel.active = true;
				PushEvent(mouse_event);
		}
	}
	vect_temp.clear();

	if (ui_manager->GetAxis(vertical.c_str())->direction_axis > 0.8f)
	{
		Event mouse_event;
		mouse_event.gui_axis.type = EventType::EVENT_AXIS;
		mouse_event.gui_axis.value = ui_manager->GetAxis(vertical.c_str())->direction_axis;
		mouse_event.gui_axis.direction = mouse_event.gui_axis.DIRECTION_DOWN;
		PushEvent(mouse_event);

	}
	else if (ui_manager->GetAxis(vertical.c_str())->direction_axis < -0.8f)
	{
		Event mouse_event;
		mouse_event.gui_axis.type = EventType::EVENT_AXIS;
		mouse_event.gui_axis.value = ui_manager->GetAxis(vertical.c_str())->direction_axis;
		mouse_event.gui_axis.direction = mouse_event.gui_axis.DIRECTION_UP;
		PushEvent(mouse_event);

	}
	if (ui_manager->GetAxis(horizontal.c_str())->direction_axis > 0.8f)
	{
		Event mouse_event;
		mouse_event.gui_axis.type = EventType::EVENT_AXIS;
		mouse_event.gui_axis.value = ui_manager->GetAxis(horizontal.c_str())->direction_axis;
		mouse_event.gui_axis.direction = mouse_event.gui_axis.DIRECTION_RIGHT;
		PushEvent(mouse_event);
	}
	else if (ui_manager->GetAxis(horizontal.c_str())->direction_axis < -0.8f)
	{
		Event mouse_event;
		mouse_event.gui_axis.type = EventType::EVENT_AXIS;
		mouse_event.gui_axis.value = ui_manager->GetAxis(horizontal.c_str())->direction_axis;
		mouse_event.gui_axis.direction = mouse_event.gui_axis.DIRECTION_LEFT;
		PushEvent(mouse_event);

	}
}

SDL_Scancode ModuleInput::GetKeyFromName(const char* name)
{
	return SDL_GetScancodeFromName(name);
}
