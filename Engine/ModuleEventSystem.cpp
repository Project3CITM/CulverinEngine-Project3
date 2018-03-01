#include "Application.h"
#include "ModuleEventSystem.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "CompMesh.h"
#include "EventDef.h"
#include "CompParticleSystem.h"
#include "ParticleSystem.h"

//You use this function to push new events to the system, with that is no needed to use App->eventsystem->PushEvent(event), only PushEvent(event)
void PushEvent(Event& event)
{
	App->event_system->PushEvent(event);
}

//You use this function to send events directly, this event send system doesn't wait until postupdate, with that is no needed to use App->eventsystem->PushImmediateEvent(event), only PushImmediateEvent(event)
void PushImmediateEvent(Event& event)
{
	App->event_system->PushImmediateEvent(event);
}

//Used in SetEventListenrs method of Modules to register this module to receive one event type
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
	for (int i = 0; i < EventType::MAXEVENTS; i++)
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
	const std::vector<Module*>* ModuleList = App->GetModuleList();
	std::map<EventType, std::vector<Module*>>::const_iterator EListener = MEventListeners.cbegin();
	uint size = 0;
	uint count = 0;

	if (!QShadowMapEvent.empty())
	{
		size = QShadowMapEvent.size();
		EListener = MEventListeners.find(EventType::EVENT_SEND_3D_3DA_MM);
		if (EListener != MEventListeners.end())
			for (int i = 0; i < size; i++)
			{
				for (std::vector<Module*>::const_iterator item2 = EListener._Ptr->_Myval.second.cbegin(); item2 != EListener._Ptr->_Myval.second.cend(); ++item2)
					(*item2)->OnEvent(QShadowMapEvent.front());
				QShadowMapEvent.pop();
			}
	}
	
	std::multimap<float, Event>* MultimapToIterate = &MM3DDrawEvent;
	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0: //iterate and send MM3DDrawEvent
			if (MM3DDrawEvent.size() == 0) continue;
			MultimapToIterate = &MM3DDrawEvent; break;
		case 1: //iterate and send MM3DADrawEvent
			if (MM3DADrawEvent.size() == 0) continue;
			MultimapToIterate = &MM3DADrawEvent; break;
		case 2: //iterate and send MM2DCanvasDrawEvent
			if (MM2DCanvasDrawEvent.size() == 0) continue;
			MultimapToIterate = &MM2DCanvasDrawEvent;
			//TODO Set perspective to orthographic here
			break; 
		}
		size = MultimapToIterate->size();
		count = 0;
		for (std::multimap<float, Event>::const_iterator item = MultimapToIterate->cbegin(); item != MultimapToIterate->cend();)
		{
			if (count >= size) break;
			if (item._Ptr->_Myval.first != EListener._Ptr->_Myval.first)
				EListener = MEventListeners.find(item._Ptr->_Myval.second.type);
			if (EListener != MEventListeners.end())
				for (std::vector<Module*>::const_iterator item2 = EListener._Ptr->_Myval.second.cbegin(); item2 != EListener._Ptr->_Myval.second.cend(); ++item2)
				{
					if (item._Ptr->_Myval.second.type == EVENT_PARTICLE_DRAW)
					{ 
						((Particle*)item._Ptr->_Myval.second.particle.ToDraw)->DrawParticle();
					}
					else switch (item._Ptr->_Myval.second.draw.Dtype)
					{
					case EDraw::DrawType::DRAW_3D:
					
					case EDraw::DrawType::DRAW_2D:
						((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->Draw();
						break;
					case EDraw::DrawType::DRAW_3D_ALPHA:
						((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->Draw(true);
						break;
					case EDraw::DrawType::DRAW_SCREEN_CANVAS:
						(*item2)->OnEvent(item._Ptr->_Myval.second);
						break;
					case EDraw::DrawType::DRAW_WORLD_CANVAS:
						//TODO
						break;

					}
				}
			else
			{
				item = MultimapToIterate->erase(item);
				continue;
			}
			item = MultimapToIterate->erase(item);
			count++;
		}
		if (i == 2)
		{
			//TODO Set perspective back to perspective here
		}
	}

	//iterate and send MMNormalEvent
	size = MMNormalEvent.size();
	count = 0;
	for (std::multimap<EventType, Event>::const_iterator item = MMNormalEvent.cbegin(); item != MMNormalEvent.cend();)
	{
		if ((item._Ptr->_Myval.first < 0) || (item._Ptr->_Myval.first > EventType::MAXEVENTS))
		{
			item = MMNormalEvent.erase(item);
			continue;
		}
		if (count >= size) break;
		if (item._Ptr->_Myval.first != EListener._Ptr->_Myval.first)
			EListener = MEventListeners.find(item._Ptr->_Myval.first);
		if (EListener != MEventListeners.end())
			for (std::vector<Module*>::const_iterator item2 = EListener._Ptr->_Myval.second.cbegin(); item2 != EListener._Ptr->_Myval.second.cend(); ++item2)
				(*item2)->OnEvent(item._Ptr->_Myval.second);
		else
		{
			item = MMNormalEvent.erase(item);
			continue;
		}
		item = MMNormalEvent.erase(item);
		count++;
	}

	//We had analysed all events, just clean them and wait for new ones in the next frame
	//MMNormalEvent.clear();
	//MM3DDrawEvent.clear();
	//MM3DADrawEvent.clear();
	//MM2DCanvasDrawEvent.clear();

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
	case EventType::EVENT_PARTICLE_DRAW:
		MM3DADrawEvent.insert(std::pair<float, Event>(-((Particle*)event.particle.ToDraw)->CameraDistance, event));
		break;
	case EventType::EVENT_DRAW:
	{
		float3 diff_vect = event.draw.ToDraw->GetGameObjectPos() - App->renderer3D->active_camera->frustum.pos;
		float DistanceCamToObject = diff_vect.Length();
		switch (event.draw.Dtype)
		{
		case event.draw.DRAW_3D: MM3DDrawEvent.insert(std::pair<float, Event>(DistanceCamToObject, event)); break;
		case event.draw.DRAW_3D_ALPHA: MM3DADrawEvent.insert(std::pair<float, Event>(DistanceCamToObject, event));  break;
		case event.draw.DRAW_2D: MM2DCanvasDrawEvent.insert(std::pair<float, Event>(DistanceCamToObject, event)); break;
		case event.draw.DRAW_WORLD_CANVAS: MM3DADrawEvent.insert(std::pair<float, Event>(DistanceCamToObject, event));  break;
		case event.draw.DRAW_SCREEN_CANVAS: MM2DCanvasDrawEvent.insert(std::pair<float, Event>(DistanceCamToObject, event)); break;
		}
		break;
	}
	case EventType::EVENT_REQUEST_3D_3DA_MM: 
	{
		Event event_temp;
		event_temp.send_3d3damm.type = EventType::EVENT_SEND_3D_3DA_MM;
		event_temp.send_3d3damm.MM3DDrawEvent = &MM3DDrawEvent;
		event_temp.send_3d3damm.MM3DADrawEvent = &MM3DADrawEvent;
		event_temp.send_3d3damm.light = event.request_3d3damm.light;
		QShadowMapEvent.push(event_temp); 
		break;
	}
	default: MMNormalEvent.insert(std::pair<EventType, Event>(event.type, event)); break;
	}
}

void ModuleEventSystem::PushImmediateEvent(Event & event)
{
	std::map<EventType, std::vector<Module*>>::const_iterator EListener = MEventListeners.find(event.type);
	if (EListener != MEventListeners.end())
	{
		switch (event.type)
		{
		case EventType::EVENT_REQUEST_3D_3DA_MM:
		{
			Event event_temp;
			event_temp.send_3d3damm.type = EventType::EVENT_SEND_3D_3DA_MM;
			event_temp.send_3d3damm.MM3DDrawEvent = &MM3DDrawEvent;
			event_temp.send_3d3damm.MM3DADrawEvent = &MM3DADrawEvent;
			event_temp.send_3d3damm.light = event.request_3d3damm.light;
			PushImmediateEvent(event_temp);
			break;
		}
		default:
			for (std::vector<Module*>::const_iterator item = EListener._Ptr->_Myval.second.cbegin(); item != EListener._Ptr->_Myval.second.cend(); ++item)
				(*item)->OnEvent(event);
		}
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