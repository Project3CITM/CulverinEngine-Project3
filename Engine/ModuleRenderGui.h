#ifndef _MODULERENDERERGUI_
#define _MODULERENDERERGUI_
#include "Module.h"
#include "Globals.h"



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
	void WorldSpaceDraw();

	void ScreenSpaceDraw();

	bool CleanUp();



	void OnResize(int width, int height);
private:
	int total_width = 0;
	int total_height = 0;
	int window_width = 0;
	int window_height = 0;
	int window_scale = 1;
	//std::vector<ComponentCanvas*> world_space_canvas;
	//std::vector<ComponentCanvas*> screen_space_canvas;

};
#endif //_MODULERENDERERGUI_
