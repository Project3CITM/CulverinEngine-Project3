#ifndef __MODULESCENEWORLD_H__
#define __MODULESCENEWORLD_H__

#include "Module.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "WindowManager.h"


class SceneWorld : public WindowManager
{
public:

	SceneWorld();
	virtual ~SceneWorld();

	bool Start();
	update_status Update(float dt);
	void Draw();
	void ShowSceneWorld();
	bool CleanUp();

	void SetWindowParams();
	float4 GetWindowParams() const;
	
	//void OpenClose();
	//bool IsOpen();


private:
	ImVec2 mouse_pos;
	float4 SceneWindow = float4::zero;

};

#endif //__MODULEHIERARCHY_H__