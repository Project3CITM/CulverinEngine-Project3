#include "Application.h"
#include "ModuleEventSystemV2.h"
#include "ParticleSystem.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "CompMaterial.h"
#include "ModuleFramebuffers.h"
#include "Scene.h"
#include "ModuleWindow.h"
#include "CompLight.h"
#include "ModuleLightning.h"
#include "CompCubeMapRenderer.h"

void AddListener(EventType type, Module* listener)
{
	App->event_system_v2->AddListener(type, listener);
}

void PushEvent(Event& event)
{
	App->event_system_v2->PushEvent(event);
}

void PushImmediateEvent(Event& event)
{
	App->event_system_v2->PushImmediateEvent(event);
}

Event GetResponseFromRequestEvent(Event& event)
{
	return App->event_system_v2->GetResponseFromRequestEvent(event);
}

void ClearEvents(EventType type)
{
	App->event_system_v2->ClearEvents(type);
}

void ClearEvents(EventType type, void* component)
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
	//Set scene for cube map render


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
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if (App->renderer3D->wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	IterateLightsV(dt);
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
	active_frame = App->scene->scene_buff;
	active_frame->Bind("Scene");
	IterateDrawV(dt);
	//active_frame->UnBind("Scene");
	//Draw alpha events
	//active_frame->Bind("Scene");
	IterateDrawAlphaV(dt);
	active_frame->UnBind("Scene");
	//Draw opaque with glow events
	glViewport(0, 0, 128, 128);
	active_frame = App->scene->glow_buff;
	active_frame->Bind("Scene");
	IterateDrawGlowV(dt);
	active_frame->UnBind("Scene");
	glUseProgram(NULL);
	if (App->renderer3D->wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//NoDraw events
	//glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
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
	uint LastBindedProgram = 0;
	uint NewProgramID = 0;
	Material* LastUsedMaterial = nullptr;
	Material* ActualMaterial =  nullptr;
	for (std::multimap<uint, Event>::const_iterator item = DrawV.cbegin(); item != DrawV.cend();)
	{
		EventType type = item._Ptr->_Myval.second.Get_event_data_type();
		switch (ValidEvent(item._Ptr->_Myval.second, dt))
		{
		case EventValidation::EVENT_VALIDATION_VALID: //Here we can execute the event, any delay is left and this is a valid event
			NewProgramID = 0;
			if (item._Ptr->_Myval.first != 0)
				NewProgramID = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->GetMaterial()->GetShaderProgram()->programID;

				ActualMaterial = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->GetMaterial()->material;

			if (LastUsedMaterial != ActualMaterial)
			{
				
				LastUsedMaterial = ActualMaterial;

				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE0);				
				if ((NewProgramID != LastBindedProgram) && (NewProgramID != 0))
				{
					LastBindedProgram = NewProgramID;					
					ActualMaterial->Bind();

				}			
				
				App->module_shaders->SetUniformVariables(ActualMaterial);

				GLuint ShadowMapLoc = glGetUniformLocation(ActualMaterial->GetProgramID(), "_shadowMap");
				glUniform1i(ShadowMapLoc, (ActualMaterial->textures.size()));
				glActiveTexture(GL_TEXTURE0 + (ActualMaterial->textures.size()));
				glBindTexture(GL_TEXTURE_2D, App->module_lightning->test_fix.depthTex);

				

			}
			switch (item._Ptr->_Myval.second.draw.Dtype)
			{
			case EDraw::DrawType::DRAW_3D:
			case EDraw::DrawType::DRAW_2D:
				((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->Draw2(LastBindedProgram);
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
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

void ModuleEventSystemV2::IterateDrawGlowV(float dt)
{
	uint LastBindedProgram = 0;
	uint NewProgramID = 0;
	bool UseGlow = false;
	Material* LastUsedMaterial = nullptr;
	Material* ActualMaterial = nullptr;
	for (std::multimap<uint, Event>::const_iterator item = DrawGlowV.cbegin(); item != DrawGlowV.cend();)
	{
		EventType type = item._Ptr->_Myval.second.Get_event_data_type();
		switch (ValidEvent(item._Ptr->_Myval.second, dt))
		{
		case EventValidation::EVENT_VALIDATION_VALID: //Here we can execute the event, any delay is left and this is a valid event

			switch (type)
			{

			case EventType::EVENT_PARTICLE_DRAW:
				glEnable(GL_BLEND);
				glDisable(GL_CULL_FACE);
				NewProgramID = App->renderer3D->particles_shader->programID;
				if ((NewProgramID != LastBindedProgram) && (NewProgramID != 0))
				{
					LastBindedProgram = NewProgramID;
					App->renderer3D->particles_shader->Bind();
				}
				((Particle*)item._Ptr->_Myval.second.particle_draw.ToDraw)->DrawParticle(App->renderer3D->particles_shader->programID);
				glDisable(GL_BLEND);
				glEnable(GL_CULL_FACE);
				break;



			case EventType::EVENT_DRAW:

				NewProgramID = 0;

				UseGlow = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->GetMaterial()->material->glow;
				if (item._Ptr->_Myval.first != 0)
				{
					if (UseGlow) NewProgramID = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->GetMaterial()->GetShaderProgram()->programID;
					else NewProgramID = App->renderer3D->non_glow_material->GetProgramID();
				}

				if (!UseGlow) ActualMaterial = App->renderer3D->non_glow_material;
				else ActualMaterial = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->GetMaterial()->material;

				if (LastUsedMaterial != ActualMaterial)
				{
					LastUsedMaterial = ActualMaterial;

					glBindTexture(GL_TEXTURE_2D, 0);
					glActiveTexture(GL_TEXTURE0);
					if ((NewProgramID != LastBindedProgram) && (NewProgramID != 0))
					{
						LastBindedProgram = NewProgramID;
						ActualMaterial->Bind();

					}
					for (uint i = 0; i < ActualMaterial->textures.size(); i++)
					{
						uint texLoc = glGetUniformLocation(ActualMaterial->GetProgramID(), ActualMaterial->textures[i].var_name.c_str());
						glUniform1i(texLoc, i);
						glActiveTexture(GL_TEXTURE0 + i);
						if (ActualMaterial->textures[i].value == nullptr) glBindTexture(GL_TEXTURE_2D, App->renderer3D->id_checkImage);
						else glBindTexture(GL_TEXTURE_2D, ActualMaterial->textures[i].value->GetTextureID());
					}

				}

				switch (item._Ptr->_Myval.second.draw.Dtype)
				{
				case EDraw::DrawType::DRAW_3D:
				case EDraw::DrawType::DRAW_2D:
					((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->Draw2(LastBindedProgram);
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

				break;
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
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

void ModuleEventSystemV2::IterateDrawAlphaV(float dt)
{
	bool alpha_draw = false;
	if (DrawAlphaV.size() > 0) alpha_draw = true;
	if (alpha_draw)
	{
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}
	uint LastBindedProgram = 0;
	uint NewProgramID = 0;
	bool UseGlow = 0;
	Material* LastUsedMaterial = nullptr;
	Material* ActualMaterial = nullptr;
	for (std::multimap<float, Event>::const_iterator item = DrawAlphaV.cbegin(); item != DrawAlphaV.cend();)
	{
		EventType type = item._Ptr->_Myval.second.Get_event_data_type();
		switch (ValidEvent(item._Ptr->_Myval.second, dt))
		{
		case EventValidation::EVENT_VALIDATION_VALID: //Here we can execute the event, any delay is left and this is a valid event
			switch (type)
			{
			case EventType::EVENT_DRAW:

				NewProgramID = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->GetMaterial()->GetShaderProgram()->programID;					
				ActualMaterial = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->GetMaterial()->material;

				if (LastUsedMaterial != ActualMaterial)
				{
					LastUsedMaterial = ActualMaterial;
					glBindTexture(GL_TEXTURE_2D, 0);
					glActiveTexture(GL_TEXTURE0);
					if ((NewProgramID != LastBindedProgram) && (NewProgramID != 0))
					{
						LastBindedProgram = NewProgramID;
						ActualMaterial->Bind();

					}
					for (uint i = 0; i < ActualMaterial->textures.size(); i++)
					{
						uint texLoc = glGetUniformLocation(ActualMaterial->GetProgramID(), ActualMaterial->textures[i].var_name.c_str());
						glUniform1i(texLoc, i);
						glActiveTexture(GL_TEXTURE0 + i);
						if (ActualMaterial->textures[i].value == nullptr) glBindTexture(GL_TEXTURE_2D, App->renderer3D->id_checkImage);
						else glBindTexture(GL_TEXTURE_2D, ActualMaterial->textures[i].value->GetTextureID());
					}
				}
				switch (item._Ptr->_Myval.second.draw.Dtype)
				{
				case EDraw::DrawType::DRAW_3D_ALPHA:
					((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw)->Draw2(LastBindedProgram);
					break;
//				case EDraw::DrawType::DRAW_SCREEN_CANVAS:
//					(*item2)->OnEvent(item._Ptr->_Myval.second);
//					break;
//				case EDraw::DrawType::DRAW_WORLD_CANVAS:
//					break;
				}
				break;
			case EventType::EVENT_PARTICLE_DRAW:
				NewProgramID = App->renderer3D->particles_shader->programID;
				if ((NewProgramID != LastBindedProgram) && (NewProgramID != 0))
				{
					LastBindedProgram = NewProgramID;
					App->renderer3D->particles_shader->Bind();
				}
				((Particle*)item._Ptr->_Myval.second.particle_draw.ToDraw)->DrawParticle(App->renderer3D->particles_shader->programID);
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
	if (alpha_draw)
	{
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
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

void ModuleEventSystemV2::IterateLightsV(float dt)
{
	uint LastBindedProgram = 0;
	uint NewProgramID = 0;
	Material* LastUsedMaterial = nullptr;
	Material* ActualMaterial = nullptr;
	for (std::multimap<float, Event>::const_iterator item = DrawLightV.cbegin(); item != DrawLightV.cend();)
	{
		EventType type = item._Ptr->_Myval.second.Get_event_data_type();
		switch (ValidEvent(item._Ptr->_Myval.second, dt))
		{
		case EventValidation::EVENT_VALIDATION_VALID: //Here we can execute the event, any delay is left and this is a valid event
		{
			App->module_lightning->OnEvent(item._Ptr->_Myval.second);
			
			item = DrawLightV.erase(item);
		}
			break;
		case EventValidation::EVENT_VALIDATION_ACTIVE_DELAY:
		case EventValidation::EVENT_VALIDATION_ADD_CONTINUE:
			item++;
			continue;
		case EventValidation::EVENT_VALIDATION_ERASE_CONTINUE:
		case EventValidation::EVENT_VALIDATION_ERROR:
			item = DrawLightV.erase(item);
			continue;
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);


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

void ModuleEventSystemV2::AddListener(EventType type, Module* listener)
{
	if (listener != nullptr)
	{
		std::map<EventType, std::vector<Module*>>::iterator EListener = MEventListeners.find(type);
		if (EListener != MEventListeners.end()) EListener._Ptr->_Myval.second.push_back(listener);
	}
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

		if((Particle*)event.particle_draw.ToDraw->glow)
			DrawGlowV.insert(std::pair<float, Event>(-((Particle*)event.particle_draw.ToDraw)->CameraDistance, event));

		break;
	case EventType::EVENT_DRAW:
	{
		float3 diff_vect = event.draw.ToDraw->GetGameObjectPos() - App->renderer3D->active_camera->frustum.pos;
		uint ID = 0;
		if ((event.draw.ToDraw != nullptr) && (((CompMesh*)event.draw.ToDraw)->GetMaterial() != nullptr) && (((CompMesh*)event.draw.ToDraw)->GetMaterial()->GetShaderProgram() != nullptr))
			ID = ((CompMesh*)event.draw.ToDraw)->GetMaterial()->GetShaderProgram()->programID;
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
			DrawV.insert(std::pair<uint, Event>(ID, event));
			DrawGlowV.insert(std::pair<uint, Event>(ID, event));
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

		float3 diff_vect = event_temp.send_3d3damm.light->GetGameObjectPos() - App->renderer3D->active_camera->frustum.pos;
		float DistanceCamToObject = diff_vect.Length();
		DrawLightV.insert(std::pair<float, Event>(DistanceCamToObject, event_temp));
		
		break;
	}

	case EventType::EVENT_CUBEMAP_REQUEST:
	{
		Event event_temp;
		event_temp.Set_event_data(EventType::EVENT_CUBEMAP_DRAW);
		event_temp.cube_map_draw.all_gameobjects = App->scene->GetAllSceneObjects();
		((CompCubeMapRenderer*)event.cube_map_request.comp_cubemap)->Bake(event_temp);	

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

Event ModuleEventSystemV2::GetResponseFromRequestEvent(Event & event)
{
	return Event();
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

void ModuleEventSystemV2::ClearEvents(EventType type, void* component)
{

}