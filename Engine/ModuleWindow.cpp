#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "SDL\include\SDL.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"
#include "parson.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	haveConfig = true;

	window = NULL;
	screen_surface = NULL;

	name = "Window";
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init(JSON_Object* node)
{
	perf_timer.Start();

	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		width = json_object_get_number(node, "Width");
		height = json_object_get_number(node, "Height");
		brightness = json_object_get_number(node, "Brightness");
		scale = json_object_get_number(node, "Scale");
		window_name = json_object_get_string(node, "Window Name");

		if (fullscreen = json_object_get_boolean(node, "Fullscreen"))
		{
			selected_op = 0;
		}
		if (fullscreen_d = json_object_get_boolean(node, "Full Desktop"))
		{
			selected_op = 1;
		}
		if (resizable = json_object_get_boolean(node, "Resizable"))
		{
			selected_op = 2;
		}
		if (borderless = json_object_get_boolean(node, "Borderless"))
		{
			selected_op = 3;
		}


		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if (fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (WIN_OPENGL == true)
		{
			flags |= SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
		}

		if (borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (fullscreen_d == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(window_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if (window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}

		displaymode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
		int display_count = 0;
		if ((display_count = SDL_GetNumVideoDisplays()) < 1) {
			SDL_Log("SDL_GetNumVideoDisplays returned: %i", display_count);
			return 1;
		}

		if (SDL_GetDisplayMode(displayIndex, modeIndex, &displaymode) != 0) {
			SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
			return 1;
		}

		if (borderless)
		{
			SDL_SetWindowBordered(window, SDL_FALSE);
		}
		else
		{
			SDL_SetWindowBordered(window, SDL_TRUE);
		}
		SDL_Surface*image;
		image = SDL_LoadBMP("Images/UI/icon_Culverin.bmp");
		// The icon is attached to the window pointer
		SDL_SetWindowIcon(window, image);
		// ...and the surface containing the icon pixel data is no longer required.
		SDL_FreeSurface(image);
	}

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

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if (window != NULL)
	{
		SDL_DestroyWindow(window);
		ImGui_ImplSdlGL3_Shutdown();
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

update_status ModuleWindow::UpdateConfig(float dt)
{
	static int refresh = displaymode.refresh_rate;
	ImGui::PushItemWidth(ImGui::GetWindowWidth() / 4);
	ImGui::InputText("Window Name", textBuf, IM_ARRAYSIZE(textBuf));
	ImGui::SameLine();
	if (ImGui::Button("APPLY##window_name"))
	{
		window_name = textBuf;
		SetTitle(window_name.c_str());
	}
	ImGui::Text("Refresh rate:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", displaymode.refresh_rate);
	ImGui::SliderInt("Brightness", &brightness, 0, 100); ImGui::SameLine();
	if (ImGui::Button("APPLY##brightness"))
	{
		float value = (float)brightness * 0.01;
		SDL_SetWindowBrightness(window, value);
	}
	ImGui::Separator();
	ImGui::Text("Window Resolution");
	const char* win_res[] = { "640x480", "800x600", "1024x768","1280x1024","1365x768","1920x1080" };
	ImGui::Combo("Select Resolution", &selected_res, win_res, IM_ARRAYSIZE(win_res)); ImGui::SameLine();
	if (ImGui::Button("APPLY##win_resolution"))
	{
		SetWindowRes(selected_res);
	}
	const char* win_opt[] = { "Fullscreen", "Full Desktop", "Windowed" };
	ImGui::Combo("Window Options", &selected_op, win_opt, IM_ARRAYSIZE(win_opt)); ImGui::SameLine();
	if (ImGui::Button("APPLY##win_options"))
	{
		SetWindowOption(selected_op);
	}
	if (ImGui::Checkbox("Borderless", &borderless))
	{
		if (borderless)
		{
			SDL_SetWindowBordered(window, SDL_FALSE);
		}
		else
		{
			SDL_SetWindowBordered(window, SDL_TRUE);
		}
	}
	ImGui::PopItemWidth();
	return UPDATE_CONTINUE;
}

bool ModuleWindow::SaveConfig(JSON_Object* node)
{
	json_object_set_string(node, "Window Name", window_name.c_str());
	json_object_set_number(node, "Width", width);
	json_object_set_number(node, "Height", height);
	json_object_set_number(node, "Brightness", brightness);
	json_object_set_number(node, "Scale", scale);
	json_object_set_boolean(node, "Fullscreen", fullscreen);
	json_object_set_boolean(node, "Full Desktop", fullscreen_d);
	json_object_set_boolean(node, "Resizable", resizable);
	json_object_set_boolean(node, "Borderless", borderless);

	return true;
}

void ModuleWindow::SetWindowOption(int i)
{
	switch (i)
	{
	case 0:
	{
		//SetFullscreen -------------
		fullscreen = true;
		fullscreen_d = false;
		resizable = false;
		borderless = false;
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		break;
	}
	case 1:
	{
		//SetFullDesktop -------------
		fullscreen_d = true;
		fullscreen = false;
		resizable = false;
		borderless = false;
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

		break;
	}
	case 2:
	{
		//SetResizable ----------
		resizable = true;
		fullscreen_d = false;
		fullscreen = false;
		borderless = false;
		SDL_SetWindowFullscreen(window, SDL_WINDOW_RESIZABLE);
		break;
	}
	default:
	{
		break;
	}
	}
}

void ModuleWindow::SetWindowRes(int i)
{
	//"640x480", "800x600", "1024x768", "1280x1024", "1365x768", "1920x1080"
	switch (i)
	{
	case 0:
	{
		width = 640;
		height = 480;

		break;
	}
	case 1:
	{
		width = 800;
		height = 600;

		break;
	}
	case 2:
	{
		width = 1024;
		height = 768;

		break;
	}
	case 3:
	{
		width = 1280;
		height = 1024;

		break;
	}
	case 4:
	{
		width = 1365;
		height = 768;

		break;
	}
	case 5:
	{
		width = 1920;
		height = 1080;

		break;
	}
	default:
	{
		width = 500;
		height = 500;
		break;
	}
	}

	SDL_SetWindowSize(window, width, height);
}

uint ModuleWindow::GetScale() const
{
	return scale;
}

uint ModuleWindow::GetWidth() const
{
	return width;
}

uint ModuleWindow::GetHeight() const
{
	return height;
}