#ifndef EVENTSYSTEM
#define EVENTSYSTEM

//TEMPORAL WARNING: DON'T PUSH EVENTS INSIDE EVENTS, YOU CAN CALL IMMEDIATE EVENTS INSTEAD, FIX INCOMMING

/*--------------------------------------------------------------------------------------------------------------*/
/*---To use "void PushEvent(Event& event);" & "AddListener(EventType type, Module* listener);" include this-----*/
/*---file and call them directly, no need to do App-> before call.----------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------*/
/*---To generate new events, include EventDef.h-----------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------*/
/*---Create events:---------------------------------------------------------------------------------------------*/
/*---Just make an instance of the union Event, define the type and all the especific properties of the event----*/
/*---type. Then just call "PushEvent(Event& event)" the system will store the event and send it at the end of---*/
/*---the frame, so you can make events in preupdate, update and postupdate.-------------------------------------*/
/*---Example:---------------------------------------------------------------------------------------------------*/
/*---Event event;-----------------------------------------------------------------------------------------------*/
/*---event.type = EventType::EVENT_DELETE_GO;-------------------------------------------------------------------*/
/*---event.deletego.Todelte = (Something to delete);------------------------------------------------------------*/
/*---PushEvent(event);------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------*/
/*---Receive events:--------------------------------------------------------------------------------------------*/
/*---In the module you want to receive events, define the method "bool SetEventListenrs()", inside that, call---*/
/*---the function "AddListener(EventType, (Module*)this)" with the propper event type you want to receive.------*/
/*---Now this module is ready to receive events of the type added, but you need to define the method to---------*/
/*---receive the events, define "void OnEvent(Event& event)" with a switch inside like the one in input---------*/
/*---module with SDL. Like "switch(event.type){case: EventType::EVENT_DELETE_GO: (call functions); break;}".----*/
/*--------------------------------------------------------------------------------------------------------------*/

#include <queue>
#include <map>

#include "Module.h"

//You use this function to push new events to the system, with that is no needed to use App->eventsystem->PushEvent(event), only PushEvent(event)
void PushEvent(Event& event);
//You use this function to send events directly, this event send system doesn't wait until postupdate, with that is no needed to use App->eventsystem->PushImmediateEvent(event), only PushImmediateEvent(event)
void PushImmediateEvent(Event& event);
//Used in SetEventListenrs method of Modules to register this module to receive one event type
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

private:
	std::multimap<EventType, Event> MMNormalEvent;	//All the events that are not draw type
	std::multimap<float, Event> MM3DDrawEvent;		//3D Game Objects without alpha added here
	std::multimap<float, Event> MM3DADrawEvent;		//3D Game Objects with alpha & World space canvas added here
	std::multimap<float, Event> MM2DCanvasDrawEvent;//2D Game Objects & Screen space canvas added here
	std::map<EventType, std::vector<Module*>> MEventListeners;
};

#endif //EVENTSYSTEM