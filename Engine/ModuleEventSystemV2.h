#ifndef EVENTSYSTEM_V2
#define EVENTSYSTEM_V2

#include <map>
#include <vector>
#include "Module.h"
#include "EventDefV2.h"

/*-----------------------------------------------------------------------------------------------------------*/
/*---Event System v2.0---------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/
/*---How to create new events:-------------------------------------------------------------------------------*/
/*-->If you want a new event type, go to the "EventDefV2.h" file and follow the instructions-----------------*/
/*-->If you want to use a normal event keep reading this file------------------------------------------------*/
/*-->First you must include this file and create a Event union instance like this (DeleteGo event example):--*/
/*------Event new_event;-------------------------------------------------------------------------------------*/
/*------new_event.Set_event_data(EventType::EVENT_DELETE_GO, 0, 0);------------------------------------------*/
/*------new_event.delete_go.Todelte = this;------------------------------------------------------------------*/
/*------PushEvent(new_event);--------------------------------------------------------------------------------*/
/*---Example explanation:------------------------------------------------------------------------------------*/
/*------Event declaration------------------------------------------------------------------------------------*/
/*------Initialize the new event with type, frame delay and time delay---------------------------------------*/
/*------This event type only need a pointer to GameObject, so we assign it to the event type instance--------*/
/*------inside the Event union-------------------------------------------------------------------------------*/
/*------Now we have the event created, initialized and filled, we can push it to the system------------------*/
/*-->All events can have a type, used in a switch to know with specific event struct inside Event union we---*/
/*---should read. Events also have a frame delay, that means that the event will be executed X frames from---*/
/*---the moment we push it to the system. And events also can have a time delay, so the event will be--------*/
/*---executed in X milisedons/seconds from the moment we push it to the system. If you put both delays, the--*/
/*---event first will wait the frame delay and then the time delay.------------------------------------------*/
/*-->To send created events to the system, you should use one of this:---------------------------------------*/
/*------void PushEvent(Event& event);------------------------------------------------------------------------*/
/*---------The event will be evaluated/executed at Event System postUpdate, right bedore Render postUpdate---*/
/*------void PushImmediateEvent(Event& event);---------------------------------------------------------------*/
/*---------The event will be evaluated/executed immediately, careful because this may crash with some event--*/
/*---------type----------------------------------------------------------------------------------------------*/
/*-->To receive any event, you must register your module to the system using:--------------------------------*/
/*------void AddListener(EventType type, Module* listener);--------------------------------------------------*/
/*---You must call this function inside "bool SetEventListeners()" Module method-----------------------------*/
/*---With that when an event of the registered type is executed, you will receive it in the------------------*/
/*---"void OnEvent(Event& event)" Module method, inside that method you should use a-------------------------*/
/*---switch(event.Get_event_data_type()) and case EventType::EVENT_TIME_MANAGER:, for example----------------*/
/*-----------------------------------------------------------------------------------------------------------*/

enum EventValidation;

void AddListener(EventType type, Module* listener);
void PushEvent(Event& event);
void PushImmediateEvent(Event& event);
Event GetResponseFromRequestEvent(Event& event);
void ClearEvents(EventType type);
void ClearEvents(EventType type, void* component);

struct DrawVMultimapKey
{
	DrawVMultimapKey(uint uuid, float distance) : uuid(uuid), distance(distance){}
	uint uuid = 0;
	float distance = 0.0f;
};

struct Comparator
{
	bool operator()(const DrawVMultimapKey left, const DrawVMultimapKey right) const
	{
		if (left.uuid < right.uuid) return true;
		else if (left.uuid > right.uuid) return false;
		else return left.distance < right.distance;
		//return left.uuid > right.uuid;
	}
};

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

	void AddListener(EventType type, Module* listener);
	void PushEvent(Event& event);
	void PushImmediateEvent(Event& event);
	Event GetResponseFromRequestEvent(Event& event);
	void ClearEvents(EventType type); //Used by particle system, modulegui, window project, must fix
	void ClearEvents(EventType type, void* component);

private:
	void IterateDrawV(float dt);
	void IterateDrawGlowV(float dt);
	void IterateDrawAlphaV(float dt);
	void IterateNoDrawV(float dt);
	void IterateLightsV(float dt);
	EventValidation ValidEvent(Event& event, float dt);

private:
	std::multimap<DrawVMultimapKey, Event, Comparator> DrawV;	//Draw events are stored here ordered by resource number (faster draw, less bind/unbind)
	std::multimap<uint, Event> DrawGlowV;						//Draw glow events are stored here ordered by resource number (faster draw, less bind/unbind)
	std::multimap<float, Event> DrawAlphaV;						//Draw events are stored here ordered by distance to active camera
	std::multimap<EventType, Event> NoDrawV;					//No-Draw events are stored here ordered by EventType enum, less eventvector change when iterating
	std::multimap<float, Event> DrawLightV;						//Lights events are stored here ordered by distance to active
	std::list<Event> PushedWhileIteratingEvents;				//Store events while iterating, used by event system internally
	std::map<EventType, std::vector<Module*>> MEventListeners;
	bool IteratingMaps = false;
	/*
	bool EventPushedWhileIteratingMaps_DrawV = false;
	bool EventPushedWhileIteratingMaps_DrawGlowV = false;
	bool EventPushedWhileIteratingMaps_DrawAlphaV = false;
	bool EventPushedWhileIteratingMaps_NoDrawV = false;
	*/
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