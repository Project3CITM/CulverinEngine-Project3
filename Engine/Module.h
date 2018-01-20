#ifndef _MODULE_
#define _MODULE_
#include "Globals.h"
#include "parson.h"
#include "PerfTimer.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl_gl3.h"
#include <string>

class Application;
struct PhysBody3D;

class Module
{
public:

	Module(bool start_enabled = true){}

	virtual ~Module(){}

	virtual bool Init(JSON_Object* node) 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status UpdateConfig(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool SaveConfig(JSON_Object* node)
	{
		return true;
	}

	void ShowPerformance(int ms_index)
	{
		if (ImGui::TreeNodeEx(name.c_str()))
		{
			if (Awake_enabled)
			{
				ImGui::TextColored(ImVec4(0.883f, 0.721f, 0.00f, 1.00f), "AWAKE - %.4f ms", Awake_t);
			}
			if (Start_enabled)
			{
				ImGui::TextColored(ImVec4(0.207f, 0.947f, 0.958f, 1.00f), "START - %.4f ms", Start_t);
			}
			if (preUpdate_enabled)
			{
				ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "PREUPDATE - %.4f ms", pre_log[ms_index - 1]);
			}
			if (Update_enabled)
			{
				ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "UPDATE - %.4f ms", up_log[ms_index - 1]);
			}
			if (postUpdate_enabled)
			{
				ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "POSTUPDATE - %.4f ms", post_log[ms_index - 1]);
			}
			ImGui::TreePop();
		}
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2){}

	bool IsEnabled() const { return enabled; }

	void Enable()
	{
		if (enabled == false)
		{
			enabled = true;
			Start();
		}
	}

	void Disable()
	{
		if (enabled == true)
		{
			enabled = false;
			CleanUp();
		}
	}
	void Disable_whitout()
	{
		if (enabled == true)
		{
			enabled = false;
		}
	}

public:

	std::string	name;
	bool enabled = true;
	bool haveConfig = false;

	// PERFORMANCE VARIABLES -------------
	PerfTimer perf_timer;

	float ms_index = 0;
	float pre_log[70] = { 0 };
	float up_log[70] = { 0 };
	float post_log[70] = { 0 };

	float Awake_t = 0;
	float Start_t = 0;
	float preUpdate_t = 0;
	float Update_t = 0;
	float postUpdate_t = 0;

	bool Awake_enabled = false;
	bool Start_enabled = false;
	bool preUpdate_enabled = false;
	bool Update_enabled = false;
	bool postUpdate_enabled = false;
	// ----------------------------------

	ImVec4 color;
};

#endif