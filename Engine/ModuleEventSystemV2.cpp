#include "Application.h"
#include "ModuleEventSystemV2.h"

void PushEventV2(Event& event)
{
	App->event_system_v2->PushEvent(event);
}

void PushImmediateEventV2(Event& event)
{
	App->event_system_v2->PushImmediateEvent(event);
}

void AddListenerV2(EventType type, Module* listener)
{
	App->event_system_v2->AddListener(type, listener);
}

ModuleEventSystemV2::ModuleEventSystemV2(bool start_enabled)
{

}

ModuleEventSystemV2::~ModuleEventSystemV2()
{

}

bool ModuleEventSystemV2::Init(JSON_Object* node)
{
	//creation of event-listeners map
	for (int i = 0; i < EventType::MAXEVENTS; i++)
	{
		std::vector<Module*> ListenersVec;
		MEventListeners.insert(std::pair<EventType, std::vector<Module*>>((EventType)i, ListenersVec));
	}
	/*
	for (int i = 0; i < EventType::MAXEVENTS; i++)
		MEventListeners.insert(std::pair<EventType, std::vector<Module*>>((EventType)i, std::vector<Module*>()));
	*/
	//addlisteners
	const std::vector<Module*>* ModuleList = App->GetModuleList();
	for (std::vector<Module*>::const_iterator item = ModuleList->cbegin(); item != ModuleList->cend(); ++item)
		(*item)->SetEventListeners();
	return true;
}

bool ModuleEventSystemV2::Start()
{
	return true;
}

update_status ModuleEventSystemV2::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventSystemV2::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventSystemV2::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventSystemV2::UpdateConfig(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleEventSystemV2::SaveConfig(JSON_Object* node)
{
	return true;
}

bool ModuleEventSystemV2::CleanUp()
{
	return true;
}

void ModuleEventSystemV2::PushEvent(Event& event)
{

}

void ModuleEventSystemV2::PushImmediateEvent(Event& event)
{

}

void ModuleEventSystemV2::AddListener(EventType type, Module* listener)
{

}

void ModuleEventSystemV2::ClearEvents(EventType type)
{

}

void ModuleEventSystemV2::ClearEvents(EventType type, Component* component)
{

}