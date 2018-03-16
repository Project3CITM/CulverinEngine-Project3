#ifndef EVENTSYSTEM_V2
#define EVENTSYSTEM_V2

#include <map>
#include <vector>
#include "Module.h"

/*
void PushEvent(Event& event);
void PushImmediateEvent(Event& event);
void AddListener(EventType type, Module* listener);
*/

class ModuleEventSystemV2 : public Module
{
public:
	ModuleEventSystemV2(bool start_enabled = true);
	~ModuleEventSystemV2();
	bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool SaveConfig(JSON_Object* node);
	bool CleanUp();

	void PushEvent(Event& event);
	void PushImmediateEvent(Event& event);
	void AddListener(EventType type, Module* listener);

	void ClearEvents(EventType type);
	void ClearEvents(EventType type, Component* component);

private:
	std::vector<std::multimap<float, Event>> DrawV;
	std::vector<std::multimap<float, Event>> NonDrawV;
	std::map<EventType, std::vector<Module*>> MEventListeners;
	uint MapSetToIterate = 0;
};

#endif //EVENTSYSTEM_V2