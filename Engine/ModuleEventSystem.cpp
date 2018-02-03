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
	//addlisteners
	const std::vector<Module*>* ModuleList = App->GetModuleList();
	for (std::vector<Module*>::const_iterator item = ModuleList->cbegin(); item != ModuleList->cend(); ++item)
		(*item)->SetEventListenrs();
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
	/*--------------------------------------------------*/
	/*---Need to clean, this is copy&paste to test :)---*/
	/*--------------------------------------------------*/

	const std::vector<Module*>* ModuleList = App->GetModuleList();
	std::map<EventType, std::vector<Module*>>::const_iterator EListener = MEventListeners.cbegin();

	//iterate and send MMNormalEvent
	for (std::multimap<EventType, Event>::const_iterator item = MMNormalEvent.cbegin(); item != MMNormalEvent.cend(); ++item)
	{
		if (item._Ptr->_Myval.first != EListener._Ptr->_Myval.first)
			EListener = MEventListeners.find(item._Ptr->_Myval.first);
		if (EListener != MEventListeners.end())
			for (std::vector<Module*>::const_iterator item2 = EListener._Ptr->_Myval.second.cbegin(); item2 != EListener._Ptr->_Myval.second.cend(); ++item2)
			{
				(*item2)->OnEvent(item._Ptr->_Myval.second);
			}
		else continue;
	}

	std::multimap<float, Event>& MultimapToIterate = MM3DDrawEvent;
	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0: //iterate and send MM3DDrawEvent
			if (MM3DDrawEvent.size() == 0) continue;
			MultimapToIterate = MM3DDrawEvent; break;
		case 1: //iterate and send MM3DADrawEvent
			if (MM3DADrawEvent.size() == 0) continue;
			MultimapToIterate = MM3DADrawEvent; break;
		case 2: //iterate and send MM2DCanvasDrawEvent
			if (MM2DCanvasDrawEvent.size() == 0) continue;
			MultimapToIterate = MM2DCanvasDrawEvent;
			//TODO Set perspective to orthographic here
			break; 
		}
		for (std::multimap<float, Event>::const_iterator item = MultimapToIterate.cbegin(); item != MultimapToIterate.cend(); ++item)
		{
			if (item._Ptr->_Myval.first != EListener._Ptr->_Myval.first)
				EListener = MEventListeners.find(item._Ptr->_Myval.second.type);
			if (EListener != MEventListeners.end())
				for (std::vector<Module*>::const_iterator item2 = EListener._Ptr->_Myval.second.cbegin(); item2 != EListener._Ptr->_Myval.second.cend(); ++item2)
					(*item2)->OnEvent(item._Ptr->_Myval.second);
			else continue;
		}
		if (i == 2)
		{
			//TODO Set perspective back to perspective here
		}
	}
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
		case event.draw.DRAW3DALPHA_WCANVAS: MM3DADrawEvent.insert(std::pair<float, Event>(event.draw.DistanceCamToObject, event)); break;
		case event.draw.DRAW2DSCREENCANVAS: MM2DCanvasDrawEvent.insert(std::pair<float, Event>(event.draw.DistanceCamToObject, event)); break;
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