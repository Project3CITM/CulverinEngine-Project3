#ifndef EVENTSYSTEM_V2
#define EVENTSYSTEM_V2

#include <map>
#include <vector>
#include "Module.h"
#include "EventDefV2.h"

enum EventValidation;

void PushEvent(Event& event);
void PushImmediateEvent(Event& event);
void AddListener(EventType type, Module* listener);

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
	EventValidation ValidEvent(Event& event, float dt);

private:
	std::multimap<uint, Event> DrawV;							//Draw events are stored here ordered by resource number (faster draw, less bind/unbind)
	std::multimap<float, Event> DrawAlphaV;						//Draw events are stored here ordered by distance to active camera
	std::multimap<EventType, Event> NoDrawV;					//No-Draw events are stored here ordered by EventType enum, less eventvector change when iterating
	std::map<EventType, std::vector<Module*>> MEventListeners;
	uint MapSetToIterate = 0;
	bool IteratingMaps = false;
	bool EventPushedWhileIteratingMaps_DrawV = false;
	bool EventPushedWhileIteratingMaps_DrawAlphaV = false;
	bool EventPushedWhileIteratingMaps_NoDrawV = false;
};

enum EventValidation
{
	EVENT_VALIDATION_VALID,
	EVENT_VALIDATION_ACTIVE_DELAY,
	EVENT_VALIDATION_ADD_CONTINUE,
	EVENT_VALIDATION_ERASE_CONTINUE,
	EVENT_VALIDATION_ERROR
};

#endif //EVENTSYSTEM_V2