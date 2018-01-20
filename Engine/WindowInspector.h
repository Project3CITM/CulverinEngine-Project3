#ifndef _WINDOWINSPECTOR_
#define _WINDOWINSPECTOR_

#include "Module.h"
#include "Globals.h"
#include "ImGui\imgui.h"
#include "WindowManager.h"
#include <string>

class GameObject;

class Inspector : public WindowManager
{
public:

	Inspector();
	virtual ~Inspector();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void ShowInspector();

	void LinkObject(GameObject* obj);
	void SetLinkObjectNull();

	GameObject* GetSelected() const;

public:

	bool model_loaded = false;

	int mesh_index = -1;
	int tex_index = -1;
	int tex_id = -1;

private:
	GameObject* selected_object = nullptr;

	ImVec4 color_picker = { 255, 255, 255, 255 };
};

#endif