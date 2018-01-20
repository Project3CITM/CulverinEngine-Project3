#ifndef __ModuleGUI_H__
#define __ModuleGUI_H__

#include "Module.h"
#include "Globals.h"
#include "ImGui\imgui.h"
#include <list>
#include <vector>
#include "MathGeoLib.h"
#include "WindowManager.h"
#include "SDL\include\SDL.h"

class GameObject;

#define IM_NEWLINE "\r\n"

enum WindowName
{
	//Names of Windows (Modules)
	HARDWARE,
	INSPECTOR,
	HIERARCHY,
	SCENEWORLD,
	PROJECT
};

class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = false);
	virtual ~ModuleGUI();

	//bool Init(JSON_Object* node);
	bool Start();
	//update_status PreUpdate(float dt);
	update_status Update(float dt);
	//update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool CleanUp();

	void UpdateWindows(float dt);

	void ShowExampleAppConsole();
	void ShowHardware();
	void ShowInfoMouse(bool* active);
	void ShowWindowExit(bool* active);
	void ShowCameraMissing();
	void ShowEngineState();

	void ShowStyleEditor(ImGuiStyle* ref = NULL);
	//void ShowConfig();
	void LoadDocks();
	void RevertStyleDocks();
	void LogOpenCloseWindow(bool active, std::string name);

	void SetLinkInspector(GameObject* obj);

public:

	/*Capsule Capsule_A;
	Capsule Capsule_B;
	Line line;
	Plane plane;*/
	bool ShowTest = false;
	bool isSaved = false;
	std::vector<WindowManager*> winManager;

	//RANDOM NUMBERS TEST ----------
	std::vector<float> floats_list;
	std::vector<int> ints_list;
	//------------------------------

private:
	//WINDOW EXIT -------------
	bool gameSave = false;
	bool reposition_exit = true;

	// bools statics ----------------------
	bool window_create_objects = false;
	bool window_show_objects = false;
	bool window_Random_generator = false;
	bool window_about_us = false;
	bool configuration = false;
	bool window_infoMouse = false;
	bool window_exit = false;
	bool window_style = false;
	bool window_CreateNewScript = false;
	bool window_ShowAllResources = false;

	int menu_height = 0;

};
#endif