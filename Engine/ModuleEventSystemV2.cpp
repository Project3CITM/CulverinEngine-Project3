#include "Application.h"
#include "ModuleEventSystemV2.h"
#include "ParticleSystem.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "CompMaterial.h"
#include "ModuleFramebuffers.h"
#include "Scene.h"

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

void ClearEvents(EventType type)
{
	App->event_system_v2->ClearEvents(type);
}

void ClearEvents(EventType type, Component* component)
{
	App->event_system_v2->ClearEvents(type, component);
}

ModuleEventSystemV2::ModuleEventSystemV2(bool start_enabled)
{
	name = "Event_System_V2";
	Awake_enabled = true;
	Start_enabled = true;
	preUpdate_enabled = true;
	Update_enabled = true;
	postUpdate_enabled = true;
}

ModuleEventSystemV2::~ModuleEventSystemV2()
{

}

bool ModuleEventSystemV2::Init(JSON_Object* node)
{
	perf_timer.Start();
	//creation of event-listeners map
	for (int i = 0; i < EventType::MAXEVENTS; i++)
		MEventListeners.insert(std::pair<EventType, std::vector<Module*>>((EventType)i, std::vector<Module*>()));
	//addlisteners
	const std::vector<Module*>* ModuleList = App->GetModuleList();
	for (std::vector<Module*>::const_iterator item = ModuleList->cbegin(); item != ModuleList->cend(); ++item)
		(*item)->SetEventListenrs();
	Awake_t = perf_timer.ReadMs();
	return true;
}

bool ModuleEventSystemV2::Start()
{
	perf_timer.Start();
	Start_t = perf_timer.ReadMs();
	return true;
}

update_status ModuleEventSystemV2::PreUpdate(float dt)
{
	perf_timer.Start();
	preUpdate_t = perf_timer.ReadMs();
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventSystemV2::Update(float dt)
{
	perf_timer.Start();
	Update_t = perf_timer.ReadMs();
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventSystemV2::PostUpdate(float dt)
{
	perf_timer.Start();
	IteratingMaps = true;
	FrameBuffer* active_frame = nullptr;
	//Draw opaque events
	App->renderer3D->render_mode = RenderMode::DEFAULT;
	active_frame = App->scene->scene_buff;
	active_frame->Bind("Scene");
	IterateDrawV(dt);
	active_frame->UnBind("Scene");
	//Draw alpha events
	App->renderer3D->render_mode = RenderMode::DEFAULT;
	active_frame = App->scene->scene_buff;
	active_frame->Bind("Scene");
	IterateDrawAlphaV(dt);
	active_frame->UnBind("Scene");
	//Draw opaque with glow events
	App->renderer3D->render_mode = RenderMode::GLOW;
	active_frame = App->scene->glow_buff;
	active_frame->Bind("Scene");
	IterateDrawGlowV(dt);
	active_frame->UnBind("Scene");
	//NoDraw events
	App->renderer3D->render_mode = RenderMode::DEFAULT;
	active_frame = App->scene->scene_buff;
	active_frame->Bind("Scene");
	IterateNoDrawV(dt);
	active_frame->UnBind("Scene");
	IteratingMaps = false;
	//Push events added while iterating to the respective multimaps
	if (PushedWhileIteratingEvents.size() > 0)
	{
		for (std::list<Event>::const_iterator item = PushedWhileIteratingEvents.cbegin(); item != PushedWhileIteratingEvents.cend(); item++)
			PushEvent(item._Ptr->_Myval);
		PushedWhileIteratingEvents.clear();
	}
	/*
	//Draw opaque events
	if (EventPushedWhileIteratingMaps_DrawV)
		for (std::multimap<uint, Event>::const_iterator item = DrawV.cbegin(); item != DrawV.cend(); item++)
			item._Ptr->_Myval.second.Set_event_data_PushedWhileIteriting(false);
	//Draw alpha events
	if (EventPushedWhileIteratingMaps_DrawGlowV)
		for (std::multimap<uint, Event>::const_iterator item = DrawGlowV.cbegin(); item != DrawGlowV.cend(); item++)
			item._Ptr->_Myval.second.Set_event_data_PushedWhileIteriting(false);
	//Draw alpha events
	if (EventPushedWhileIteratingMaps_DrawAlphaV)
		for (std::multimap<float, Event>::const_iterator item = DrawAlphaV.cbegin(); item != DrawAlphaV.cend(); item++)
			item._Ptr->_Myval.second.Set_event_data_PushedWhileIteriting(false);
	//NoDraw events
	if (EventPushedWhileIteratingMaps_NoDrawV)
		for (std::multimap<EventType, Event>::const_iterator item = NoDrawV.cbegin(); item != NoDrawV.cend(); item++)
			item._Ptr->_Myval.second.Set_event_data_PushedWhileIteriting(false);
	EventPushedWhileIteratingMaps_DrawV = false;
	EventPushedWhileIteratingMaps_DrawGlowV = false;
	EventPushedWhileIteratingMaps_DrawAlphaV = false;
	EventPushedWhileIteratingMaps_NoDrawV = false;
	*/
	postUpdate_t = perf_timer.ReadMs();
	return update_status::UPDATE_CONTINUE;
}

void ModuleEventSystemV2::IterateDrawV(float dt)
{
	for (std::multimap<uint, Event>::const_iterator item = DrawV.cbegin(); item != DrawV.cend();)
	{
		EventType type = item._Ptr->_Myval.second.Get_event_data_type();
		switch (ValidEvent(item._Ptr->_Myval.second, dt))
		{
		case EventValidation::EVENT_VALIDATION_VALID:
			//Here we can execute the event, any delay is left and this is a valid event
			switch (item._Ptr->_Myval.second.draw.Dtype)
			{
			case EDraw::DrawType::DRAW_3D:
			case EDraw::DrawType::DRAW_2D:
				((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->Draw();
				break;
//			case EDraw::DrawType::DRAW_SCREEN_CANVAS:
//				(*item2)->OnEvent(item._Ptr->_Myval.second);
//				break;
//			case EDraw::DrawType::DRAW_WORLD_CANVAS:
//				break;
			}
			item = DrawV.erase(item);
			break;
		case EventValidation::EVENT_VALIDATION_ACTIVE_DELAY:
		case EventValidation::EVENT_VALIDATION_ADD_CONTINUE:
			item++;
			continue;
		case EventValidation::EVENT_VALIDATION_ERASE_CONTINUE:
		case EventValidation::EVENT_VALIDATION_ERROR:
			item = DrawV.erase(item);
			continue;
		}
	}
}

void ModuleEventSystemV2::IterateDrawGlowV(float dt)
{
	for (std::multimap<uint, Event>::const_iterator item = DrawGlowV.cbegin(); item != DrawGlowV.cend();)
	{
		EventType type = item._Ptr->_Myval.second.Get_event_data_type();
		switch (ValidEvent(item._Ptr->_Myval.second, dt))
		{
		case EventValidation::EVENT_VALIDATION_VALID:
			//Here we can execute the event, any delay is left and this is a valid event
			switch (item._Ptr->_Myval.second.draw.Dtype)
			{
			case EDraw::DrawType::DRAW_3D:
			case EDraw::DrawType::DRAW_2D:
				((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->Draw();
				break;
//			case EDraw::DrawType::DRAW_3D_ALPHA:
//				((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->Draw(true);
//				break;
//			case EDraw::DrawType::DRAW_SCREEN_CANVAS:
//				(*item2)->OnEvent(item._Ptr->_Myval.second);
//				break;
//			case EDraw::DrawType::DRAW_WORLD_CANVAS:
//				break;
			}
			item = DrawGlowV.erase(item);
			break;
		case EventValidation::EVENT_VALIDATION_ACTIVE_DELAY:
		case EventValidation::EVENT_VALIDATION_ADD_CONTINUE:
			item++;
			continue;
		case EventValidation::EVENT_VALIDATION_ERASE_CONTINUE:
		case EventValidation::EVENT_VALIDATION_ERROR:
			item = DrawGlowV.erase(item);
			continue;
		}
	}
}

void ModuleEventSystemV2::IterateDrawAlphaV(float dt)
{
	/*
	bool alpha_draw = false;
	if (DrawAlphaV.size() > 0) alpha_draw = true;
	if (alpha_draw)
	{
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	}
	*/
	for (std::multimap<float, Event>::const_iterator item = DrawAlphaV.cbegin(); item != DrawAlphaV.cend();)
	{
		EventType type = item._Ptr->_Myval.second.Get_event_data_type();
		switch (ValidEvent(item._Ptr->_Myval.second, dt))
		{
		case EventValidation::EVENT_VALIDATION_VALID:
			//Here we can execute the event, any delay is left and this is a valid event
			switch (type)
			{
			case EventType::EVENT_DRAW:
				switch (item._Ptr->_Myval.second.draw.Dtype)
				{
				case EDraw::DrawType::DRAW_3D_ALPHA:
					((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->Draw(true);
					break;
//				case EDraw::DrawType::DRAW_SCREEN_CANVAS:
//					(*item2)->OnEvent(item._Ptr->_Myval.second);
//					break;
//				case EDraw::DrawType::DRAW_WORLD_CANVAS:
//					break;
				}
				break;
			case EventType::EVENT_PARTICLE_DRAW:
				App->renderer3D->particles_shader->Bind();
				((Particle*)item._Ptr->_Myval.second.particle_draw.ToDraw)->DrawParticle(App->renderer3D->particles_shader->programID);
				App->renderer3D->particles_shader->Unbind();
				break;
			}
			item = DrawAlphaV.erase(item);
			break;
		case EventValidation::EVENT_VALIDATION_ACTIVE_DELAY:
		case EventValidation::EVENT_VALIDATION_ADD_CONTINUE:
			item++;
			continue;
		case EventValidation::EVENT_VALIDATION_ERASE_CONTINUE:
		case EventValidation::EVENT_VALIDATION_ERROR:
			item = DrawAlphaV.erase(item);
			continue;
		}
	}
	/*
	if (alpha_draw)
	{
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	}
	*/
}

void ModuleEventSystemV2::IterateNoDrawV(float dt)
{
	std::map<EventType, std::vector<Module*>>::const_iterator EListener = MEventListeners.cbegin();
	for (std::multimap<EventType, Event>::const_iterator item = NoDrawV.cbegin(); item != NoDrawV.cend();)
	{
		switch (ValidEvent(item._Ptr->_Myval.second, dt))
		{
		case EventValidation::EVENT_VALIDATION_VALID:
			//Here we can execute the event, any delay is left and this is a valid event
			//If EListener iterator is different from the actual analysed event, find the new iterator
			if (item._Ptr->_Myval.first != EListener._Ptr->_Myval.first)
				EListener = MEventListeners.find(item._Ptr->_Myval.first);
			//if != end, we found it, iterate listeners and call OnEvent, if we didn't found it, erase and jump to next iteration
			if (EListener != MEventListeners.end())
				for (std::vector<Module*>::const_iterator item2 = EListener._Ptr->_Myval.second.cbegin(); item2 != EListener._Ptr->_Myval.second.cend(); ++item2)
					(*item2)->OnEvent(item._Ptr->_Myval.second);
			else
			{
				item = NoDrawV.erase(item);
				continue;
			}
			break;
		case EventValidation::EVENT_VALIDATION_ACTIVE_DELAY:
		case EventValidation::EVENT_VALIDATION_ADD_CONTINUE:
			item++;
			continue;
		case EventValidation::EVENT_VALIDATION_ERASE_CONTINUE:
		case EventValidation::EVENT_VALIDATION_ERROR:
			item = NoDrawV.erase(item);
			continue;
		}
		item = NoDrawV.erase(item);
	}
}

EventValidation ModuleEventSystemV2::ValidEvent(Event& event, float dt)
{
	//If this event is pushed while we iterate, skip it for the next frame
	/*
	if (event.Get_event_data_PushedWhileIteriting())
		return EventValidation::EVENT_VALIDATION_ADD_CONTINUE;
	*/
	//Check if it is a valid event type
	EventType type = event.Get_event_data_type();
	if ((type < 0) || (type >= EventType::MAXEVENTS))
		return EventValidation::EVENT_VALIDATION_ERASE_CONTINUE;
	//If this point is reached, the event is valid to be analysed
	//This event has frame delay?, if 0, evaluate time delay, if !=0, decrese 1 frame and change saved delay in event
	uint FrameDelay = event.Get_event_data_frame_delay();
	if (FrameDelay == 0)
	{
		//This event has time delay?, if 0, evaluate event, if !=0, decrese dt and change saved delay in event
		uint TimeDelay = event.Get_event_data_time_delay();
		if (TimeDelay == 0)
			return EventValidation::EVENT_VALIDATION_VALID;
		else
		{
			TimeDelay -= (uint)(dt * 1000.0f);
			event.Set_event_data_time_delay(TimeDelay);
			return EventValidation::EVENT_VALIDATION_ACTIVE_DELAY;
		}
	}
	else
	{
		FrameDelay -= 1;
		event.Set_event_data_frame_delay(FrameDelay);
		return EventValidation::EVENT_VALIDATION_ACTIVE_DELAY;
	}
	return EventValidation::EVENT_VALIDATION_ERROR;
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
		PushedWhileIteratingEvents.push_back(event);
		return;
	}

	switch (event.Get_event_data_type())
	{
	case EventType::EVENT_PARTICLE_DRAW:
		/*
		if (IteratingMaps)
		{
			event.Set_event_data_PushedWhileIteriting(true);
			EventPushedWhileIteratingMaps_DrawAlphaV = true;
		}
		*/
		DrawAlphaV.insert(std::pair<float, Event>(-((Particle*)event.particle_draw.ToDraw)->CameraDistance, event));
		break;
	case EventType::EVENT_DRAW:
	{
		float3 diff_vect = event.draw.ToDraw->GetGameObjectPos() - App->renderer3D->active_camera->frustum.pos;
		float DistanceCamToObject = diff_vect.Length();
		switch (event.draw.Dtype)
		{
		case event.draw.DRAW_3D:
			/*
			if (IteratingMaps)
			{
				event.Set_event_data_PushedWhileIteriting(true);
				EventPushedWhileIteratingMaps_DrawV = true;
			}
			*/
			DrawV.insert(std::pair<uint, Event>(event.draw.ToDraw->GetUUID(), event));
			DrawGlowV.insert(std::pair<uint, Event>(event.draw.ToDraw->GetUUID(), event));
			break;
		case event.draw.DRAW_3D_ALPHA:
			/*
			if (IteratingMaps)
			{
				event.Set_event_data_PushedWhileIteriting(true);
				EventPushedWhileIteratingMaps_DrawAlphaV = true;
			}
			*/
			DrawAlphaV.insert(std::pair<float, Event>(DistanceCamToObject, event));
			break;
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
		if (IteratingMaps)
		{
			event.Set_event_data_PushedWhileIteriting(true);
			EventPushedWhileIteratingMaps_DrawV = true;
			EventPushedWhileIteratingMaps_DrawAlphaV = true;
			EventPushedWhileIteratingMaps_NoDrawV = true;
		}
		*/
		Event event_temp;
		event_temp.Set_event_data(EventType::EVENT_SEND_3D_3DA_MM);
		event_temp.send_3d3damm.MM3DDrawEvent = &DrawV;
		event_temp.send_3d3damm.MM3DADrawEvent = &DrawAlphaV;
		event_temp.send_3d3damm.light = event.request_3d3damm.light;
		PushEvent(event_temp);
		break;
	}
	default:
		/*
		if (IteratingMaps)
		{
			event.Set_event_data_PushedWhileIteriting(true);
			EventPushedWhileIteratingMaps_NoDrawV = true;
		}
		*/
		NoDrawV.insert(std::pair<EventType, Event>(event.Get_event_data_type(), event));
		break;
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
			Event event_temp;
			event_temp.Set_event_data(EventType::EVENT_SEND_3D_3DA_MM);
			event_temp.send_3d3damm.MM3DDrawEvent = &DrawV;
			event_temp.send_3d3damm.MM3DADrawEvent = &DrawAlphaV;
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
	switch (type)
	{
	case EventType::EVENT_DRAW:
		DrawV.clear();
		DrawGlowV.clear();
		for (std::multimap<float, Event>::const_iterator item = DrawAlphaV.cbegin(); item != DrawAlphaV.cend();)
		{
			if (item._Ptr->_Myval.second.Get_event_data_type() == type) item = DrawAlphaV.erase(item);
			else item++;
		}
		for (std::list<Event>::const_iterator item = PushedWhileIteratingEvents.cbegin(); item != PushedWhileIteratingEvents.cend();)
		{
			if (item._Ptr->_Myval.Get_event_data_type() == type) item = PushedWhileIteratingEvents.erase(item);
			else item++;
		}
		break;
	case EventType::EVENT_PARTICLE_DRAW:
		for (std::multimap<float, Event>::const_iterator item = DrawAlphaV.cbegin(); item != DrawAlphaV.cend();)
		{
			if (item._Ptr->_Myval.second.Get_event_data_type() == type) item = DrawAlphaV.erase(item);
			else item++;
		}
		for (std::list<Event>::const_iterator item = PushedWhileIteratingEvents.cbegin(); item != PushedWhileIteratingEvents.cend();)
		{
			if (item._Ptr->_Myval.Get_event_data_type() == type) item = PushedWhileIteratingEvents.erase(item);
			else item++;
		}
		break;
	default:
		for (std::multimap<EventType, Event>::const_iterator item = NoDrawV.cbegin(); item != NoDrawV.cend();)
		{
			if (item._Ptr->_Myval.first == type) item = NoDrawV.erase(item);
			else item++;
		}
		break;
	}
}

void ModuleEventSystemV2::ClearEvents(EventType type, Component* component)
{

}