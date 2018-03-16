#ifndef _MODULERENDERERGUI_
#define _MODULERENDERERGUI_
#include "Module.h"
#include "Globals.h"


class CompCanvas;
class CompInteractive;
class ModuleRenderGui : public Module
{
public:
	ModuleRenderGui(bool start_enabled = true);
	~ModuleRenderGui();

	bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	void OnEvent(Event& event);
	bool SetEventListenrs();
	void PassSelected(CompInteractive* to_pass);
	void ChangeSelected(Event& this_event);
	void WorldSpaceDraw();
	void ScreenSpaceDraw(bool debug=false);

	bool CleanUp();
	std::vector<CompCanvas*> screen_space_canvas;



	void OnResize(int width, int height);


private:
	int total_width = 0;
	int total_height = 0;
	int window_width = 0;
	int window_height = 0;
	int window_scale = 1;
	bool mouse_down = false;
	bool debug_draw = false;
	float2 last_size_dock = float2(0, 0);
	std::vector<CompInteractive*> iteractive_vector;

public:

	CompInteractive* focus = nullptr;
	CompInteractive* selected = nullptr;

	//std::vector<ComponentCanvas*> world_space_canvas;

};
#endif //_MODULERENDERERGUI_
