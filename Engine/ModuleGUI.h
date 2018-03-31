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

	bool Init(JSON_Object* node);
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
	void ShowCreateNewScriptWindow();

	void ShowStyleEditor(ImGuiStyle* ref = NULL);
	//void ShowConfig();
	void LoadDocks();
	void RevertStyleDocks();
	void LogOpenCloseWindow(bool active, std::string name);

	void SetLinkInspector(GameObject* obj);

	//Event system test
	bool SetEventListenrs();
	void OnEvent(Event& event);

public:

	/*Capsule Capsule_A;
	Capsule Capsule_B;
	Line line;
	Plane plane;*/
	bool show_test = false;
	bool is_saved = false;
	std::vector<WindowManager*> win_manager;

	//RANDOM NUMBERS TEST ----------
	std::vector<float> floats_list;
	std::vector<int> ints_list;
	//------------------------------

	// Shader Ui stuff-----------------
	int combo_shaders = 0;
	bool shader_obj_creation = false;
	bool shader_program_creation= false;
	bool shader_program_creation_UI = false;
	bool shader_obj_creation_begin = false;
	bool material_creation = false;
	char str_shad_temp[64] = "";
	char str_shad_prg_temp[64] = "";
	char str_mat_temp[64] = "";

	std::vector<Shader*> vec_temp_shader;
	int combo_shaders_obj = -1;
	int combo_shaders_obj2 = -1;
	int combo_material_obj = -1;

	// Add and remove icons Component UI Button
	uint icon_plus;
	uint icon_remove;
	//-----------------

private:

	//WINDOW EXIT -------------
	bool game_save = false;
	bool reposition_exit = true;

	// bools statics ----------------------
	bool window_create_objects = false;
	bool window_show_objects = false;
	bool window_random_generator = false;
	bool window_about_us = false;
	bool configuration = false;
	bool window_infoMouse = false;
	bool window_exit = false;
	bool window_style = false;
	bool window_create_new_script = false;
	bool window_show_all_resources = false;
	bool window_audio = false;
	bool window_create_map = false;
	bool window_create_animation = false;

	//bool window_NewScene = false;

	int menu_height = 0;

};
#endif