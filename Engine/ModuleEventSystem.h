#ifndef _EVENTSYSTEM_
#define _EVENTSYSTEM_

#include <queue>
#include <map>

#include "Module.h"
#include "EventDef.h"

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
	std::queue<Event> QNormalEvent;
	std::multimap<float, Event> Q3DDrawEvent;
	std::multimap<float, Event> Q3DADrawEvent;
	std::multimap<float, Event> Q2DDrawEvent;
	std::map<EventType, std::vector<Module*>> EListeners;
};

#endif //_EVENTSYSTEM_