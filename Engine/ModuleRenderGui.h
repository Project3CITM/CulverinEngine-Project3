#ifndef _MODULERENDERERGUI_
#define _MODULERENDERERGUI_
#include "Module.h"
#include "Globals.h"


class CompCanvas;
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
	bool SetEventListenrs();
	
	void OnEvent(Event& event);
	void WorldSpaceDraw();

	void ScreenSpaceDraw();

	bool CleanUp();
	std::vector<CompCanvas*> screen_space_canvas;



	void OnResize(int width, int height);
private:
	int total_width = 0;
	int total_height = 0;
	int window_width = 0;
	int window_height = 0;
	int window_scale = 1;
	//std::vector<ComponentCanvas*> world_space_canvas;

};
#endif //_MODULERENDERERGUI_
