#ifndef __MODULEHARDWARE_H__
#define __MODULEHARDWARE_H__

#include "Module.h"
#include "Globals.h"
#include "ImGui\imgui.h"
#include "WindowManager.h"


class Hardware : public WindowManager
{
public:

	Hardware();
	virtual ~Hardware();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	//void OpenClose();
	//bool IsOpen();


private:


};


#endif