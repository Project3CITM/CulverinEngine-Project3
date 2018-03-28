#ifndef _MODULE_ANIMATION_H_
#define _MODULE_ANIMATION_H_
#include "Globals.h"
#include "Module.h"
class ModuleAnimation : public Module
{
public:
	ModuleAnimation();
	virtual ~ModuleAnimation();

	void ShowAnimationWindow(bool& active);
	void FindAnimComponent();
	void SaveAnimation();
	void LoadAnimation();

private:
	
};

#endif //_MODULE_ANIMATION_H_