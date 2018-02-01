#include "Application.h"
#include "ModuleEventSystem.h"

int MaxEvents = EventType::MAXEVENTS - 1;

void PushEvent(Event& event)
{
	App->event_system->PushEvent(event);
}

void AddListener(EventType type, Module* listener)
{
	App->event_system->AddListener(type, listener);
}

ModuleEventSystem::ModuleEventSystem(bool start_enabled)
{

}

ModuleEventSystem::~ModuleEventSystem()
{

}

bool ModuleEventSystem::Init(JSON_Object* node)
{
	//creation of event-listeners map
	for (int i = 0; i < MaxEvents; i++)
	{
		std::vector<Module*> ListenersVec;
		MEventListeners.insert(std::pair<EventType, std::vector<Module*>>((EventType)i, ListenersVec));
	}
	//addlistenrs
	const std::list<Module*>* ModuleList = App->GetModuleList();
	for (std::list<Module*>::const_iterator item = ModuleList->cbegin(); item != ModuleList->cend(); ++item)
		item._Ptr->_Myval->SetEventListenrs();
	return true;
}

bool ModuleEventSystem::Start()
{
	return true;
}

update_status ModuleEventSystem::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventSystem::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventSystem::PostUpdate(float dt)
{
	const std::list<Module*>* ModuleList = App->GetModuleList();
	//iterate and send MMNormalEvent

	//iterate and send MM3DDrawEvent

	//iterate and send MM3DADrawEvent

	//iterate and send MM2DDrawEvent

	/*
	for (std::list<Module*>::const_iterator item = ModuleList->cbegin(); item != ModuleList->cend(); ++item)
		item._Ptr->_Myval->SetEventListenrs();
	*/
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventSystem::UpdateConfig(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleEventSystem::SaveConfig(JSON_Object* node)
{
	return true;
}

bool ModuleEventSystem::CleanUp()
{
	return true;
}

void ModuleEventSystem::PushEvent(Event& event)
{
	switch (event.type)
	{
	case EventType::DRAW:
		switch (event.draw.type)
		{
		case event.draw.DRAW3D: MM3DDrawEvent.insert(std::pair<float, Event>(event.draw.DistanceCamToObject, event)); break;
		case event.draw.DRAW3DAlpha: MM3DADrawEvent.insert(std::pair<float, Event>(event.draw.DistanceCamToObject, event)); break;
		case event.draw.DRAW2D: MM2DDrawEvent.insert(std::pair<float, Event>(event.draw.DistanceCamToObject, event)); break;
		}
		break;
	default: MMNormalEvent.insert(std::pair<EventType, Event>(event.type, event)); break;
	}
}

void ModuleEventSystem::AddListener(EventType type, Module* listener)
{
	if (listener != nullptr)
	{
		std::map<EventType, std::vector<Module*>>::iterator EListener = MEventListeners.find(type);
		if (EListener != MEventListeners.end()) EListener._Ptr->_Myval.second.push_back(listener);
	}
}