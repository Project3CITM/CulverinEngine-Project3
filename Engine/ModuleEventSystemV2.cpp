#include "Application.h"
#include "ModuleEventSystemV2.h"
#include "ParticleSystem.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"

void PushEvent(Event& event)
{
	App->event_system_v2->PushEvent(event);
}

void PushImmediateEvent(Event& event)
{
	App->event_system_v2->PushImmediateEvent(event);
}

void AddListener(EventType type, Module* listener)
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
		MEventListeners.insert(std::pair<EventType, std::vector<Module*>>((EventType)i, std::vector<Module*>()));
	/*
	for (int i = 0; i < EventType::MAXEVENTS; i++)
	{
		std::vector<Module*> ListenersVec;
		MEventListeners.insert(std::pair<EventType, std::vector<Module*>>((EventType)i, ListenersVec));
	}
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
	IteratingMaps = true;
	/*
	std::map<EventType, std::vector<Module*>>::const_iterator EListener = MEventListeners.cbegin();
	//Draw opaque events
	for (std::multimap<uint, Event>::const_iterator item = DrawV.cbegin(); item != DrawV.cend();)
	{
		if (item._Ptr->_Myval.second.Get_event_data_PushedWhileIteriting())
		{
			item++;
			continue;
		}
		//WIP
	}
	//Draw alpha events
	for (std::multimap<float, Event>::const_iterator item = DrawAlphaV.cbegin(); item != DrawAlphaV.cend();)
	{
		if (item._Ptr->_Myval.second.Get_event_data_PushedWhileIteriting())
		{
			item++;
			continue;
		}
		//WIP
	}
	//NoDraw events
	for (std::multimap<EventType, Event>::const_iterator item = NoDrawV.cbegin(); item != NoDrawV.cend();)
	{
		if (item._Ptr->_Myval.second.Get_event_data_PushedWhileIteriting())
		{
			item++;
			continue;
		}
		if ((item._Ptr->_Myval.first < 0) || (item._Ptr->_Myval.first > EventType::MAXEVENTS))
		{
			item = NoDrawV.erase(item);
			continue;
		}
		if (item._Ptr->_Myval.first != EListener._Ptr->_Myval.first)
			EListener = MEventListeners.find(item._Ptr->_Myval.first);
		if (EListener != MEventListeners.end())
			for (std::vector<Module*>::const_iterator item2 = EListener._Ptr->_Myval.second.cbegin(); item2 != EListener._Ptr->_Myval.second.cend(); ++item2)
				(*item2)->OnEvent(item._Ptr->_Myval.second);
		else
		{
			item = NoDrawV.erase(item);
			continue;
		}
		item = NoDrawV.erase(item);
	}
	*/
	IteratingMaps = false;

	if (EventPushedWhileIteratingMaps)
	{
		//Draw opaque events
		for (std::multimap<uint, Event>::const_iterator item = DrawV.cbegin(); item != DrawV.cend(); item++)
			item._Ptr->_Myval.second.Set_event_data_PushedWhileIteriting(false);
		//Draw alpha events
		for (std::multimap<float, Event>::const_iterator item = DrawAlphaV.cbegin(); item != DrawAlphaV.cend(); item++)
			item._Ptr->_Myval.second.Set_event_data_PushedWhileIteriting(false);
		//NoDraw events
		for (std::multimap<EventType, Event>::const_iterator item = NoDrawV.cbegin(); item != NoDrawV.cend(); item++)
			item._Ptr->_Myval.second.Set_event_data_PushedWhileIteriting(false);
		EventPushedWhileIteratingMaps = false;
	}

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
	if (IteratingMaps)
	{
		event.Set_event_data_PushedWhileIteriting(true);
		EventPushedWhileIteratingMaps = true;
	}
	switch (event.Get_event_data_type())
	{
	case EventType::EVENT_PARTICLE_DRAW:
		DrawAlphaV.insert(std::pair<float, Event>(-((Particle*)event.particle_draw.ToDraw)->CameraDistance, event));
		break;
	case EventType::EVENT_DRAW:
	{
		float3 diff_vect = event.draw.ToDraw->GetGameObjectPos() - App->renderer3D->active_camera->frustum.pos;
		float DistanceCamToObject = diff_vect.Length();
		switch (event.draw.Dtype)
		{
		case event.draw.DRAW_3D: DrawV.insert(std::pair<uint, Event>(event.draw.ToDraw->GetUUID(), event)); break;
		case event.draw.DRAW_3D_ALPHA: DrawAlphaV.insert(std::pair<float, Event>(DistanceCamToObject, event)); break;
		case event.draw.DRAW_2D: 
			//MM2DCanvasDrawEvent.insert(std::pair<float, Event>(DistanceCamToObject, event)); 
			break;
		case event.draw.DRAW_WORLD_CANVAS:
			//MM3DADrawEvent.insert(std::pair<float, Event>(DistanceCamToObject, event)); 
			break;
		case event.draw.DRAW_SCREEN_CANVAS: 
			//MM2DCanvasDrawEvent.insert(std::pair<float, Event>(DistanceCamToObject, event));
			break;
		}
		break;
	}
	case EventType::EVENT_REQUEST_3D_3DA_MM:
	{
		/*
		Event event_temp;
		event_temp.send_3d3damm.type = EventType::EVENT_SEND_3D_3DA_MM;
		event_temp.send_3d3damm.MM3DDrawEvent = &MM3DDrawEvent;
		event_temp.send_3d3damm.MM3DADrawEvent = &MM3DADrawEvent;
		event_temp.send_3d3damm.light = event.request_3d3damm.light;
		QShadowMapEvent.push(event_temp);
		*/
		break;
	}
	default: NoDrawV.insert(std::pair<EventType, Event>(event.Get_event_data_type(), event)); break;
	}
}

void ModuleEventSystemV2::PushImmediateEvent(Event& event)
{
	std::map<EventType, std::vector<Module*>>::const_iterator EListener = MEventListeners.find(event.Get_event_data_type());
	if (EListener != MEventListeners.end())
	{
		switch (event.Get_event_data_type())
		{
		case EventType::EVENT_REQUEST_3D_3DA_MM:
		{
			/*
			Event event_temp;
			event_temp.send_3d3damm.type = EventType::EVENT_SEND_3D_3DA_MM;
			event_temp.send_3d3damm.MM3DDrawEvent = &MM3DDrawEvent;
			event_temp.send_3d3damm.MM3DADrawEvent = &MM3DADrawEvent;
			event_temp.send_3d3damm.light = event.request_3d3damm.light;
			PushImmediateEvent(event_temp);
			*/
			break;
		}
		default:
			for (std::vector<Module*>::const_iterator item = EListener._Ptr->_Myval.second.cbegin(); item != EListener._Ptr->_Myval.second.cend(); ++item)
				(*item)->OnEvent(event);
		}
	}
}

void ModuleEventSystemV2::AddListener(EventType type, Module* listener)
{
	if (listener != nullptr)
	{
		std::map<EventType, std::vector<Module*>>::iterator EListener = MEventListeners.find(type);
		if (EListener != MEventListeners.end()) EListener._Ptr->_Myval.second.push_back(listener);
	}
}

void ModuleEventSystemV2::ClearEvents(EventType type)
{
	/*
	switch (type)
	{
	case EventType::EVENT_DRAW:
		MM3DDrawEvent.clear();
		for (std::multimap<float, Event>::const_iterator item = MM3DADrawEvent.cbegin(); item != MM3DADrawEvent.cend();)
		{
			if (item._Ptr->_Myval.second.type == type)
				item = MM3DADrawEvent.erase(item);
			else item++;
		}
		MM2DCanvasDrawEvent.clear();
		break;
	case EventType::EVENT_PARTICLE_DRAW:
		for (std::multimap<float, Event>::const_iterator item = MM3DADrawEvent.cbegin(); item != MM3DADrawEvent.cend();)
		{
			if (item._Ptr->_Myval.second.type == type)
				item = MM3DADrawEvent.erase(item);
			else item++;
		}
		break;
	case EventType::EVENT_SEND_3D_3DA_MM:
		//QShadowMapEvent
		break;
	default:
		for (std::multimap<EventType, Event>::const_iterator item = MMNormalEvent.cbegin(); item != MMNormalEvent.cend();)
		{
			if (item._Ptr->_Myval.first == type) item = MMNormalEvent.erase(item);
			else item++;
		}
		break;
	}
	*/
}

void ModuleEventSystemV2::ClearEvents(EventType type, Component* component)
{

}