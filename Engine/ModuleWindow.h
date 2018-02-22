#ifndef MODULE_WINDOW_H
#define MODULE_WINDOW_H

#include "Module.h"
#include "SDL/include/SDL.h"
#include "parson.h"

class Application;

class ModuleWindow : public Module
{
public:
	
	ModuleWindow(bool start_enabled = true);
	virtual ~ModuleWindow();

	bool Init(JSON_Object* node);
	//bool Start();
	//update_status PreUpdate(float dt);
	//update_status Update(float dt);
	//update_status postUpdate(float dt);
	bool CleanUp();

	void SetTitle(const char* title);
	update_status UpdateConfig(float dt);
	bool SaveConfig(JSON_Object* node);

	void SetWindowOption(int i);
	void SetWindowRes(int i);
	void SetWindowSize(int width,int height);

	// Retrieve window scale
	uint GetScale() const;
	uint GetWidth() const;
	uint GetHeight() const;
	uint GetDesktopWidth() const;
	uint GetDesktopHeight() const;

public:

	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	bool writing = false; // To not overlap camera controls when writting

private:

	SDL_DisplayMode displaymode;
	int display_index = 0;
	int mode_index = 0;

	// Window Parameters -------------
	int width = SCREEN_WIDTH * SCREEN_SIZE;
	int height = SCREEN_HEIGHT * SCREEN_SIZE;
	int scale = SCREEN_SIZE;
	int brightness = 100;

	int selected_op = 2;
	int selected_res = 4;

	bool fullscreen = false;
	bool fullscreen_d = false;
	bool resizable = false;
	bool borderless = false;

	std::string window_name;
	char text_buf[256] = "Change Name...";
	// ------------------------------------

};

#endif // MODULE_WINDOW_H