/*
#ifndef _EVENTDEF_
#define _EVENTDEF_
#include <vector>
#include <map>
#include "Math\float2.h"

class GameObject;

enum EventType
{
	EVENT_UNKNOWN = -1,
	EVENT_DELETE_GO,
	MAXEVENTS
};

struct EventData
{
	EventType type = EventType::EVENT_UNKNOWN;
	float delay = 0.0f;
	enum Consumability
	{
		CONSUMABILITY_CONSUMABLE,
		CONSUMABILITY_UNCONSUMABLE,
		CONSUMABILITY_CONSUMED
	};
	Consumability button = Consumability::CONSUMABILITY_UNCONSUMABLE;
};

struct EDeleteGO
{
	EventData event_data;
	GameObject* Todelte = nullptr;
};

union Event
{
	Event() {}
	~Event() {}
	EventData event_data;
	EDeleteGO delete_go;
};

#endif //_EVENTDEF_
*/