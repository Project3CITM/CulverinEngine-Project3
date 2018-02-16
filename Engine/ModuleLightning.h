#ifndef __MODULELIGHTNING_H__
#define __MODULELIGHTNING_H__

#include "Module.h"

class ModuleLightning : public Module
{
public:
	ModuleLightning(bool start_enabled = true);
	virtual ~ModuleLightning();
};

#endif // __MODULELIGHTNING_H__