#include "ModuleLightning.h"
#include "Application.h"
#include "GL3W/include/glew.h"
#include "MathGeoLib.h"
#include "ShadersLib.h"
#include "Scene.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "ModuleResourceManager.h"
#include "ModuleEventSystemV2.h"
#include "CompTransform.h"
#include "ModuleShaders.h"
#include "GameObject.h"
#include "CompMesh.h"
#include "CompMaterial.h"
#include "CompLight.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "DefaultShaders.h"
#include "ModuleWindow.h"
#include "DepthCubeMap.h"
#include "ResourceMesh.h"

using namespace glm;


DepthFrameBuffer::DepthFrameBuffer()
{
}

DepthFrameBuffer::DepthFrameBuffer(int width, int height)
{
	Create(width, height);
}

DepthFrameBuffer::~DepthFrameBuffer()
{
	Destroy();
}

void DepthFrameBuffer::Create(int width, int height)
{
	if (width <= 0 || height <= 0)
		return;

	this->width = width;
	this->height = height;


	
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// Assign the depth buffer texture to texture channel 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTex);

	// Create and set up the FBO
	glGenFramebuffers(1, &frame_id);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, depthTex, 0);

	glDrawBuffer(GL_NONE); // Since we dont need the color buffer we must tell OpenGl explicitly


	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthFrameBuffer::Destroy()
{
	glDeleteFramebuffers(1, &frame_id);
	glDeleteTextures(1, &texture);
}

void DepthFrameBuffer::Resize(int width, int height)
{
	if(this->width != width || this->height != height)
	{
		Destroy();
		Create(width, height);
	}
}

void DepthFrameBuffer::Bind(const char* window)
{
	//size = GetSizeDock(window);

	//Resize(App->window->GetWidth(), App->window->GetHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, frame_id);
}

void DepthFrameBuffer::UnBind(const char* window)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint DepthFrameBuffer::GetTexture() const
{
	return texture;
}

//---------------------------------------------------------

ModuleLightning::ModuleLightning(bool start_enabled) : Module(start_enabled)
{
	name = "Lightning";
	have_config = true;
	Awake_enabled = true;
	Start_enabled = true;
	preUpdate_enabled = true;
}


ModuleLightning::~ModuleLightning()
{
}

bool ModuleLightning::Init(JSON_Object* node)
{
	perf_timer.Start();

	// TODO: Read ammount of shadow cast point from config. Will use default for now for testing purposes
	//shadow_cast_points_count = 1;

	Awake_t = perf_timer.ReadMs();
	return true;
}

bool ModuleLightning::Start()
{
	perf_timer.Start();

	//AddShadowMapCastViews(shadow_cast_points_count);

	test_fix.Create(shadow_maps_res_w, shadow_maps_res_h);

	//------------------------------------

	shadow_Shader = App->module_shaders->CreateDefaultShader("Shadow_Map", ShadowMapFrag, ShadowMapVert, nullptr, true);
	point_light_shadow_depth_shader = App->module_shaders->CreateDefaultShader("Point_Shadow_Map", PointShadowMapFrag, PointShadowMapVert, nullptr, true);
	
	//-------------------------------------

	

	light_UI_plane = (ResourceMesh*)App->resource_manager->GetResource(4);

	light_UI_plane->vertices[0].texCoords = float2(0, 0);
	light_UI_plane->vertices[1].texCoords = float2(1, 0);
	light_UI_plane->vertices[2].texCoords = float2(0, 1);
	light_UI_plane->vertices[3].texCoords = float2(1, 1);


	char* total_buffer_mesh = nullptr;
	int total_size_buffer = 0;

	if (light_UI_plane->vertices.size()>0)
	{
		total_size_buffer += light_UI_plane->num_vertices * 3;
		total_size_buffer += light_UI_plane->num_vertices * 2;
	}

	total_buffer_mesh = new char[total_size_buffer * sizeof(float)];
	char* cursor = total_buffer_mesh;

	for (int i = 0; i <light_UI_plane->num_vertices; i++) {
		memcpy(cursor, &light_UI_plane->vertices[i].pos, 3 * sizeof(float));
		cursor += 3 * sizeof(float);

		memcpy(cursor, &light_UI_plane->vertices[i].texCoords, 2 * sizeof(float));
		cursor += 2 * sizeof(float);
	}
	light_UI_plane->num_game_objects_use_me++;
	light_UI_plane->LoadToMemory();
	delete[] total_buffer_mesh;

	Start_t = perf_timer.ReadMs();
	return true;
}

update_status ModuleLightning::PreUpdate(float dt)
{
	perf_timer.Start();

	//TODO: Should think on optimitzations on this.

	frame_used_lights.clear();
	std::sort(scene_lights.begin(), scene_lights.end(), OrderLights); 


	for (uint i = 0; i < scene_lights.size(); ++i)
	{
		if(scene_lights[i]->use_light_to_render == true)
			frame_used_lights.push_back(scene_lights[i]);
	}
	

	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleLightning::Update(float dt)
{

	for (uint i = 0; i < frame_used_lights.size(); ++i)
	{
		CompLight* light = frame_used_lights[i];

		if (light->type == Light_type::DIRECTIONAL_LIGHT) {
			float3 dir = light->GetParent()->GetComponentTransform()->GetEulerToDirection();
			dir = dir.Normalized();

			
			Frustum* frustum = /*&App->renderer3D->game_camera->frustum;//*/&App->renderer3D->GetActiveCamera()->frustum;

			glm::vec3 lDir = glm::vec3(dir.x, dir.y, dir.z);
			glm::vec3 camPos = glm::vec3(frustum->pos.x, frustum->pos.y, frustum->pos.z);

			glm::vec3 lookPos = camPos + (glm::vec3(frustum->front.x, frustum->front.y, frustum->front.z) * distanceFromSceneCameraToLookAt);

			glm::vec3 eye = lookPos + (lDir * distanceFromTheShadowMapRenderLookAtPosToShadowMapRenderCamPos);
			
			glm::mat4 biasMatrix(
				0.5, 0.0, 0.0, 0,
				0.0, 0.5, 0.0, 0,
				0.0, 0.0, 0.5, 0,
				0.5, 0.5, 0.5, 1.0
			);
			
			glm::mat4 depthProjectionMatrix = glm::ortho<float>(-projSize, projSize, -projSize, projSize, nearPlane, farPlane);
			glm::mat4 depthViewMatrix = glm::lookAt(eye, lookPos, glm::vec3(0, 1, 0));


			light->depthMVPMat = depthProjectionMatrix * depthViewMatrix;
			light->depthBiasMat = biasMatrix * light->depthMVPMat;
		}
	}



/*
	shadow_Shader->Bind();
	text.Bind("peter");

	glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);

	// Compute the MVP matrix from the light's point of view
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = glm::mat4(1.0);
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

	


	uint depthMatrixID= glGetUniformLocation(shadow_Shader->programID, "depthMVP");
	glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

	App->scene->scene_buff->Bind("Scene");
	ImGui::Image((ImTextureID*)text.frame_id, ImVec2(shadow_maps_res_w, shadow_maps_res_h));
	shadow_Shader->Unbind();
	//text.UnBind("peter");

	*/


	return UPDATE_CONTINUE;
}

bool ModuleLightning::SaveConfig(JSON_Object* node)
{
	return true;
}

bool ModuleLightning::CleanUp()
{
	for (std::vector<DepthCubeMap*>::iterator it = shadow_point_lights_maps.begin(); it != shadow_point_lights_maps.end(); ++it)
	{
		RELEASE((*it));
	}

	//RELEASE(shadow_Shader);
	//RELEASE(point_light_shadow_depth_shader);


	return true;
}

void ModuleLightning::OnEvent(Event & event)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	switch (event.Get_event_data_type())
	{
	case EventType::EVENT_SEND_3D_3DA_MM:

		const CompLight* light = event.send_3d3damm.light;

		if (light->type == Light_type::DIRECTIONAL_LIGHT) {
			test_fix.Bind("peter");
			glViewport(0, 0, 1024, 1024);
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (std::multimap<uint, Event>::const_iterator item = event.send_3d3damm.MM3DDrawEvent->begin(); item != event.send_3d3damm.MM3DDrawEvent->end(); item++)
			{
				CompMesh* m = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw);

				if (m->resource_mesh != nullptr)
				{
					if (m->resource_mesh->vertices.size() > 0 && m->resource_mesh->indices.size() > 0)
					{
						CalcDirectionalShadowMap((CompLight*)light, m);
					}

					else
					{
						LOG("Cannot draw the mesh");
					}
				}

			}
			
		}

		else if (light->type == Light_type::POINT_LIGHT) {

			//CalcPointShadowMaps(event, (CompLight*)light);
		}

	}

	glCullFace(GL_BACK);

	App->scene->scene_buff->Bind("Scene"); glCullFace(GL_BACK);
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
	
}

void ModuleLightning::CalcPointShadowMaps(Event& events, CompLight* light)
{


	// First  check if the mesh is in range of the point light


	uint depth_cube_map_index = 0;
	for (std::vector<CompLight*>::iterator it = frame_used_lights.begin(); it != frame_used_lights.end(); ++it)
	{
		if ((*it)->type == Light_type::POINT_LIGHT)
		{
			float3 light_pos = (*it)->GetGameObjectPos();
			
			DepthCubeMap* fbo = shadow_point_lights_maps[depth_cube_map_index];

			fbo->Bind();
			point_light_shadow_depth_shader->Bind();

			
			GLint viewport_size[4];
			glGetIntegerv(GL_VIEWPORT, viewport_size);
			uint shadow_w, shadow_h;
			fbo->GetShadowResolution(shadow_w, shadow_h);
			glViewport(0, 0, shadow_w, shadow_h);

			//
			float near_plane = 1.f;
			float far_plane = 50.f; //TODO: Change it for light range
			glm::mat4 shadow_proj = glm::perspective(glm::radians(90.f), (float)shadow_w / (float)shadow_h, near_plane, far_plane);

			std::vector<glm::mat4> shadow_transforms;
			glm::vec3 l_pos = glm::vec3(light_pos.x, light_pos.y, light_pos.z);
			shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
			shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
			shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
			shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));
			shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)));
			shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)));

			uint sh_id = point_light_shadow_depth_shader->programID;

			for (uint i = 0; i < 6; ++i)
			{

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, fbo->GetTexture(), 0);
				glClear(GL_DEPTH_BUFFER_BIT);
				for (std::multimap<uint, Event>::const_iterator item = events.send_3d3damm.MM3DDrawEvent->begin(); item != events.send_3d3damm.MM3DDrawEvent->end(); item++)
				{
					CompMesh* m = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw);

					float3 object_pos = m->GetGameObjectPos();

					CompTransform* transform = (CompTransform*)m->GetParent()->FindComponentByType(C_TRANSFORM);
					float4x4 matrixfloat = transform->GetGlobalTransform();

					GLfloat matrix[16] =
					{
						matrixfloat[0][0],matrixfloat[1][0],matrixfloat[2][0],matrixfloat[3][0],
						matrixfloat[0][1],matrixfloat[1][1],matrixfloat[2][1],matrixfloat[3][1],
						matrixfloat[0][2],matrixfloat[1][2],matrixfloat[2][2],matrixfloat[3][2],
						matrixfloat[0][3],matrixfloat[1][3],matrixfloat[2][3],matrixfloat[3][3]
					};

					uint sh_id = point_light_shadow_depth_shader->programID;

					GLint modelLoc = glGetUniformLocation(sh_id, "model");
					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, matrix);

					glUniformMatrix4fv(glGetUniformLocation(sh_id, "viewproj"),
						1, GL_FALSE, &((shadow_transforms[i])[0][0]));

					glUniform1f(glGetUniformLocation(sh_id, "_far_plane"), far_plane);
					glUniform3fv(glGetUniformLocation(sh_id, "_light_pos"), 1, &l_pos.x);

					ResourceMesh* mesh = m->resource_mesh;
					glBindBuffer(GL_ARRAY_BUFFER, mesh->id_total_buffer);

					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (char*)NULL + (0 * sizeof(float)));


					glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_id);
					glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);

				}
			}

			//Reset viewport
			glViewport(viewport_size[0], viewport_size[1], viewport_size[2], viewport_size[3]);

			//Unbind shader and cube map
			point_light_shadow_depth_shader->Unbind();
			fbo->UnBind();

			++depth_cube_map_index;
		}
	}
}

void ModuleLightning::CalcDirectionalShadowMap(CompLight* light, CompMesh* m)
{
	int x = 0;
	shadow_Shader->Bind();

	Material* material = App->renderer3D->default_material;
	//if (material->material_shader != nullptr)
	if (m->GetMaterial() != nullptr) material = m->GetMaterial()->material;
	//shader->Bind();

	CompTransform* transform = (CompTransform*)m->GetParent()->FindComponentByType(C_TRANSFORM);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Set Wireframe
	if (App->renderer3D->wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//Temporary, future delete incoming
	//Set Color
	if (m->GetMaterial() != nullptr)
	{
		glColor4f(m->GetMaterial()->GetColor().r, m->GetMaterial()->GetColor().g, m->GetMaterial()->GetColor().b, m->GetMaterial()->GetColor().a);
	}
	else
	{
		glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	}
	//


	//Future Delete
	
	//

	Frustum camFrust = App->renderer3D->active_camera->frustum;

	float4x4 matrixfloat = transform->GetGlobalTransform();
	GLfloat matrix[16] =
	{
		matrixfloat[0][0],matrixfloat[1][0],matrixfloat[2][0],matrixfloat[3][0],
		matrixfloat[0][1],matrixfloat[1][1],matrixfloat[2][1],matrixfloat[3][1],
		matrixfloat[0][2],matrixfloat[1][2],matrixfloat[2][2],matrixfloat[3][2],
		matrixfloat[0][3],matrixfloat[1][3],matrixfloat[2][3],matrixfloat[3][3]
	};

	float4x4 MVP = camFrust.ProjectionMatrix()* camFrust.ViewMatrix() * matrixfloat;

	//This is needed for the shadow projection in CompMesh cpp
	
	//----------------------------

	GLint modelLoc = glGetUniformLocation(shadow_Shader->programID, "model");
	uint depthMatrixID = glGetUniformLocation(shadow_Shader->programID, "depthMVP");


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, matrix);
	glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &light->depthMVPMat[0][0]);


	int total_save_buffer = 14;

	if (m->resource_mesh->vertices.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m->resource_mesh->id_total_buffer);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (0 * sizeof(float)));


		glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->resource_mesh->indices_id);
		glDrawElements(GL_TRIANGLES, m->resource_mesh->num_indices, GL_UNSIGNED_INT, NULL);

	}

	if (material->name == "Shadow_World_Render") {

		material->Bind();

		int depthMatrixID = glGetUniformLocation(material->GetProgramID(), "depthMVP");
		int depthBiasID = glGetUniformLocation(material->GetProgramID(), "depthBias");		
		//-----------------------
		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(depthBiasID, 1, GL_FALSE, &light->depthBiasMat[0][0]);

		material->Unbind();
	}
	shadow_Shader->Bind();

	//-----------------

	//Reset TextureColor
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	//Disable Wireframe -> only this object will be wireframed
	if (App->renderer3D->wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ModuleLightning::SetShadowCastPoints(uint points)
{
	shadow_cast_points_count = points;
	if(shadow_cast_points_count > shadow_point_lights_maps.size())
	{
		// Then create new frame buffers
		// TODO: Probably would be better to add a max ammount of shadow maps

		AddShadowMapCastViews(shadow_cast_points_count - shadow_point_lights_maps.size());
	}
}

void ModuleLightning::ResizeShadowMaps(uint w_res, uint h_res)
{
	shadow_maps_res_w = w_res;
	shadow_maps_res_h = h_res;

	for(std::vector<DepthCubeMap*>::iterator it = shadow_point_lights_maps.begin(); it != shadow_point_lights_maps.end(); ++it)
	{
		(*it)->Resize(shadow_maps_res_w, shadow_maps_res_h);
	}
}

std::vector<DepthCubeMap*>* ModuleLightning::GetShadowMaps()
{
	return &shadow_point_lights_maps;
}

void ModuleLightning::GetShadowMapsResolution(uint& w_res, uint& h_res)
{
	w_res = shadow_maps_res_w;
	h_res = shadow_maps_res_h;
}

void ModuleLightning::OnLightCreated(CompLight* l)
{
	scene_lights.push_back(l);
}

void ModuleLightning::OnLightDestroyed(CompLight* l)
{
	for(std::vector<CompLight*>::iterator it = scene_lights.begin(); it != scene_lights.end(); ++it)
	{
		if((*it) == l)
		{
			scene_lights.erase(it);

			for(std::vector<CompLight*>::iterator it2 = frame_used_lights.begin(); it2 != frame_used_lights.end(); ++it2)
			{
				frame_used_lights.erase(it2);
				return;
			}

			return;
		}
	}
}

std::vector<CompLight*> ModuleLightning::GetSceneLights() const
{
	return scene_lights;
}

std::vector<CompLight*>* ModuleLightning::GetActiveLights() 
{
	return &frame_used_lights;
}

void ModuleLightning::GetActiveLightsCount(uint ammount, std::vector<CompLight*>& to_fill)
{
	for(uint i = 0; i < ammount; ++i)
	{
		if (i < scene_lights.size())
		{
			to_fill.push_back(scene_lights[i]);
		}
		else
		{
			// If index ammount is passing the lights ammount just return.
			return;
		}
	}
}

bool ModuleLightning::SetEventListenrs()
{
	AddListener(EventType::EVENT_SEND_3D_3DA_MM, this);
	return false;
}

void ModuleLightning::AddShadowMapCastViews(uint ammount)
{
	for(int i = 0; i < ammount; ++i)
	{
		DepthCubeMap* buff = new DepthCubeMap(shadow_maps_res_w, shadow_maps_res_h);
		buff->Create();
		shadow_point_lights_maps.push_back(buff);
	}
}

// ------------- UI -----------------------------------------

update_status ModuleLightning::UpdateConfig(float dt)
{
	//TODO: Add possibility to change this. For now just info display
	ImGui::Text("Shadow cast points used:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", shadow_cast_points_count);

	ImGui::Text("Shadow maps resolution:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i x %i", shadow_maps_res_w, shadow_maps_res_h);

	ImGui::Text("Lights in the scene:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", scene_lights.size());

	ImGui::Text("Lights used on frame:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", frame_used_lights.size());

	ImGui::DragFloat("Distance from camera front to render shadow map", &distanceFromSceneCameraToLookAt, 1.0f, 1.0f, 100.0f);
	ImGui::DragFloat("Shadow map camera size", &projSize, 1.0f, 10.0f, 100.0f);
	ImGui::DragFloat("Shadow map camera near plane dist", &nearPlane, 1.0f, 1.0f, 100.0f);
	ImGui::DragFloat("Shadow map camera far plane dist", &farPlane, 1.0f, 10.0f, 100.0f);
	ImGui::DragFloat("Shadow map camera distance from look at position", &distanceFromTheShadowMapRenderLookAtPosToShadowMapRenderCamPos, 1.0f, 1.0f, 100.0f);
	
	int u_l = shadow_cast_points_count;
	if(ImGui::DragInt("Set used lights", &u_l, 1, 0, 8))
	{
		SetShadowCastPoints(u_l);
	}

	ImGui::Checkbox("Display extense debug info", &light_extense_debug_info);
	if (light_extense_debug_info)
	{
		Frustum* cam = &App->renderer3D->active_camera->frustum;
		ImGui::TextColored(ImVec4(0.f, 0.5f, 1.f, 1.f), "Camera pos: x:%.3f y:%.3f z:%.3f", cam->pos.x, cam->pos.y, cam->pos.z);
		for (std::vector<CompLight*>::iterator it = scene_lights.begin(); it != scene_lights.end(); ++it)
		{
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "\tName: %s - x:%f y:%f z:%f ---- Dist to active camera: %.3f \t Used: %s", (*it)->GetParent()->GetName(), (*it)->GetGameObjectPos().x, (*it)->GetGameObjectPos().y, (*it)->GetGameObjectPos().z, cam->pos.Distance((*it)->GetGameObjectPos()), (*it)->use_light_to_render ? "Y" : "N");
		}

		ImGui::Separator();

		for (std::vector<CompLight*>::iterator it = frame_used_lights.begin(); it != frame_used_lights.end(); ++it)
		{
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "\tName: %s", (*it)->GetParent()->GetName());
		}
	}

	if(ImGui::TreeNodeEx("Shadow maps"))
	{
		for(uint i = 0; i < shadow_point_lights_maps.size(); ++i)
		{
			if(i == shadow_cast_points_count)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "This shadow maps are not used currently!");
				ImGui::Separator();
			}

			std::string shadowmap_name = "Shadow map: " + std::to_string(i);
			if(ImGui::TreeNodeEx(shadowmap_name.c_str()))
			{
				DepthCubeMap* frame = shadow_point_lights_maps[i];
				ImGui::Image((ImTextureID*)frame->GetTexture(), ImVec2(256, 256));

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	return UPDATE_CONTINUE;
}

bool OrderLights(CompLight* l1, CompLight* l2)
{
	if (!l1 || !l2) return false;

	//l1->use_light_to_render = false;
	//l2->use_light_to_render = false;

	if(l1->type == Light_type::POINT_LIGHT && l2->type == Light_type::POINT_LIGHT)
	{
		// Only point lights must be ordered
		
		// Must calc distance to main camera from both lights.
		Frustum* cam = &App->renderer3D->GetActiveCamera()->frustum;

		float l1_dist = cam->pos.Distance(l1->GetGameObjectPos());
		float l2_dist = cam->pos.Distance(l2->GetGameObjectPos());;

		if (l1_dist < l2_dist) return true;
	}
	return false;
}