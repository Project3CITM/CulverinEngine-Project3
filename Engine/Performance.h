#ifndef __ModuleGUI_H__
#define __ModuleGUI_H__

#include "Module.h"
#include "Globals.h"
#include "ImGui\imgui.h"
#include <vector>

struct EventPerformance
{
	std::string name;
	uint line;
	std::string file;
};

class Performance : public Module
{
public:
	Performance();
	virtual ~Performance();





	void Create(std::string name, uint line, std::string file);

private:

	std::vector<EventPerformance> perf;
	//std::multimap<EventPerformance> perf;
	// Use C# or not?

};


#endif
