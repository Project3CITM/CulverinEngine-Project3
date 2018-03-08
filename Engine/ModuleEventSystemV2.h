/*
#ifndef EVENTSYSTEM_V2
#define EVENTSYSTEM_V2

#include <map>
#include <vector>
#include "Module.h"

void PushEvent(Event& event);
void PushImmediateEvent(Event& event);
void AddListener(EventType type, Module* listener);

class ModuleEventSystem : public Module
{
public:
	ModuleEventSystem(bool start_enabled = true);
	~ModuleEventSystem();
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
*/