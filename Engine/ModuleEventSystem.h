#ifndef _EVENTSYSTEM_
#define _EVENTSYSTEM_

#include <queue>
#include <map>

#include "Module.h"

void PushEvent(Event& event);
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
	void AddListener(EventType type, Module* listener);

private:
	std::multimap<EventType, Event> MMNormalEvent;
	std::multimap<float, Event> MM3DDrawEvent;
	std::multimap<float, Event> MM3DADrawEvent;
	std::multimap<float, Event> MM2DDrawEvent;
	std::map<EventType, std::vector<Module*>> MEventListeners;
};

#endif //_EVENTSYSTEM_