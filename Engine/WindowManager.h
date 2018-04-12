#ifndef _WINDOWMANAGER_
#define _WINDOWMANAGER_

#include "Globals.h"
#include "ImGui\imgui.h"
#include <list>
#include <vector>
#include "MathGeoLib.h"


struct Active
{
	bool active = false;

	void OpenClose()
	{
		active = !active;
	}
	bool IsActive()
	{
		return active;
	}

};

class WindowManager
{
public:
	WindowManager() {}

	virtual ~WindowManager() {}

	virtual bool Start()
	{
		return true;
	}
	virtual update_status Update(float dt)
	{
		return update_status();
	}
	virtual void Draw() {}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual void SpecialFunction(const std::string name) {}


	float2 GetPos() {}

	/*virtual void OpenClose()
	{
		active = !active;
	}*/

	/*virtual bool IsActive()
	{
		return active;
	}*/

public:
	std::vector<Active> active;
	std::string name;
	ImVec2 position;
	// or std::vector<bool> active; YES
	//ModuleObjects* obj = nullptr;

};

#endif