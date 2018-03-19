#include "ModuleLightning.h"
#include"Application.h"
#include "GL3W/include/glew.h"
#include"MathGeoLib.h"
#include"ShadersLib.h"
#include"Scene.h"
#include "ModuleRenderer3D.h"
#include "CompCamera.h"
#include "ModuleResourceManager.h"
#include "ModuleEventSystem.h"
#include "CompTransform.h"
#include "ModuleShaders.h"
#include "GameObject.h"
#include "CompMesh.h"
#include "CompMaterial.h"
#include "CompLight.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "DefaultShaders.h"
#include"ModuleWindow.h"
#include "DepthCubeMap.h"

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

	Resize(App->window->GetWidth(), App->window->GetHeight());
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
	shadow_cast_points_count = 1;

	Awake_t = perf_timer.ReadMs();
	return true;
}

bool ModuleLightning::Start()
{
	perf_timer.Start();

	AddShadowMapCastViews(shadow_cast_points_count);

	test_fix.Create(shadow_maps_res_w, shadow_maps_res_h);

	//------------------------------------

	shadow_Shader = App->module_shaders->CreateDefaultShader("Shadow_Map", ShadowMapFrag, ShadowMapVert);
	point_light_shadow_depth_shader = App->module_shaders->CreateDefaultShader("Point_Shadow_Map", PointShadowMapFrag, PointShadowMapVert, PointShadowMapGeo);
	
	//-------------------------------------

	App->renderer3D->LoadImage_devil("Assets/Bulb_Texture.png", &texture_bulb);

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

	Start_t = perf_timer.ReadMs();

	return true;
}

update_status ModuleLightning::PreUpdate(float dt)
{
	perf_timer.Start();

	//TODO: Should think on optimitzations on this.

	/*frame_used_lights.clear();
	std::sort(scene_lights.begin(), scene_lights.end(), OrderLights); 
	for(uint i = 0; i < shadow_cast_points_count; ++i)
	{
		if(i < scene_lights.size())
		{
			CompLight* l = scene_lights[i];
			if (l->type == Light_type::POINT_LIGHT)
			{
				l->use_light_to_render = true;
				frame_used_lights.push_back(l);
			}
		}
	}
	*/

	preUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModuleLightning::Update(float dt)
{

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

	RELEASE(shadow_Shader);
	RELEASE(point_light_shadow_depth_shader);


	return true;
}

void ModuleLightning::OnEvent(Event & event)
{
	/*if (App->scene->scene_buff != nullptr)
	{
		//This is only for shadows 

		test_fix.Bind("peter");

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		// Cull back-facing triangles -> draw only front-facing triangles

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	switch (event.type)
	{
	case EventType::EVENT_SEND_3D_3DA_MM:
		
	
		for (std::multimap<float, Event>::const_iterator item = event.send_3d3damm.MM3DDrawEvent->begin(); item != event.send_3d3damm.MM3DDrawEvent->end();item++)
		{

			CompMesh* m = ((CompMesh*)item._Ptr->_Myval.second.draw.ToDraw);

			if (m->resource_mesh != nullptr)
			{
				if (m->resource_mesh->vertices.size() > 0 && m->resource_mesh->indices.size() > 0)
				{
					const CompLight* light = event.send_3d3damm.light;

					if (light->type == Light_type::DIRECTIONAL_LIGHT)
						CalcDirectionalShadowMap((CompLight*)light, m);
					else if(light->type == Light_type::POINT_LIGHT)
						CalcPointShadowMaps((CompLight*)light, m);
				}

				else
				{
					LOG("Cannot draw the mesh");
				}
			}

		}
		break;
	}
	if (App->scene->scene_buff != nullptr)
	{
		ImGui::Image((ImTextureID*)test_fix.depthTex, ImVec2(500, 500));
		App->scene->scene_buff->Bind("Scene");
		glCullFace(GL_BACK);
		shadow_Shader->Unbind();
	}*/
}

void ModuleLightning::CalcPointShadowMaps(CompLight* light, CompMesh* mesh_to_render)
{
	if (!mesh_to_render || !mesh_to_render->resource_mesh) return;
	if(mesh_to_render->resource_mesh->vertices.size() <= 0) return;

	// First  check if the mesh is in range of the point light
	float3 object_pos = mesh_to_render->GetGameObjectPos();

	/*uint depth_cube_map_index = 0;
	for(std::vector<CompLight*>::iterator it = frame_used_lights.begin(); it != frame_used_lights.end(); ++it)
	{
		if((*it)->type == Light_type::POINT_LIGHT)
		{
			float3 light_pos = (*it)->GetGameObjectPos();
			//TODO: Add range to point lights
			//if(light_pos.Distance(object_pos) <= (*it)->range * (*it)->range)
			//{
			//	
			//}

			// If is range bind the frame buffer and the shader
			DepthCubeMap* fbo = shadow_point_lights_maps[depth_cube_map_index];

			fbo->Bind();
			point_light_shadow_depth_shader->Bind();
			
			// Actually render the mesh

			//Get vieport and resize it to shadow res
			GLint viewport_size[4];
			glGetIntegerv(GL_VIEWPORT, viewport_size);
			uint shadow_w, shadow_h;
			fbo->GetShadowResolution(shadow_w, shadow_h);
			glViewport(0, 0, shadow_w, shadow_h);

			//
			float near_plane = 1.f;
			float far_plane = 25.f; //TODO: Change it for light range
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
			for(uint i = 0; i < 6; ++i)
			{
				glUniformMatrix4fv(glGetUniformLocation(sh_id, ("shadow_matrices[" + std::to_string(i) + "]").c_str()),
					1, GL_FALSE, &((shadow_transforms[i])[0][0]));
			}
			glUniform1f(glGetUniformLocation(sh_id, "far_plane"), far_plane);
			glUniform3fv(glGetUniformLocation(sh_id, "light_pos"), 1, &l_pos.x);

			ResourceMesh* m = mesh_to_render->resource_mesh;
			glBindBuffer(GL_ARRAY_BUFFER, m->id_total_buffer);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (char*)NULL + (0 * sizeof(float)));


			glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indices_id);
			glDrawElements(GL_TRIANGLES, m->num_indices, GL_UNSIGNED_INT, NULL);


			//Reset viewport
			glViewport(viewport_size[0], viewport_size[1], viewport_size[2], viewport_size[3]);
			
			//Unbind shader and cube map
			point_light_shadow_depth_shader->Unbind();
			fbo->UnBind();

			++depth_cube_map_index;
		}
	}*/
}

void ModuleLightning::CalcDirectionalShadowMap(CompLight* light, CompMesh* m)
{
	int x = 0;
	/*shadow_Shader->Bind();

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
	if (App->renderer3D->texture_2d)
	{
		CompMaterial* temp = m->GetParent()->GetComponentMaterial();
		if (temp != nullptr) {
			for (int i = 0; i < temp->material->textures.size(); i++) {


				uint texLoc = glGetUniformLocation(temp->material->GetProgramID(), temp->material->textures[i].var_name.c_str());
				glUniform1i(texLoc, i);


				glActiveTexture(GL_TEXTURE0 + i);


				if (temp->material->textures[i].value == nullptr)
				{
					glBindTexture(GL_TEXTURE_2D, App->renderer3D->id_checkImage);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, temp->material->textures[i].value->GetTextureID());
				}

			}


			uint texture_2D_sampler = 0;
			glActiveTexture(GL_TEXTURE0);
			if (temp->GetTextureID() == 0)
			{
				glBindTexture(GL_TEXTURE_2D, App->renderer3D->id_checkImage);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, temp->GetTextureID());
			}
			texture_2D_sampler = glGetUniformLocation(App->renderer3D->default_shader->programID, "_texture");
			glUniform1i(texture_2D_sampler, 0);


		}
	}
	//

	Frustum camFrust = App->renderer3D->active_camera->frustum;// App->camera->GetFrustum();
	float4x4 temp = camFrust.ViewMatrix();


	float4x4 matrixfloat = transform->GetGlobalTransform();
	GLfloat matrix[16] =
	{
		matrixfloat[0][0],matrixfloat[1][0],matrixfloat[2][0],matrixfloat[3][0],
		matrixfloat[0][1],matrixfloat[1][1],matrixfloat[2][1],matrixfloat[3][1],
		matrixfloat[0][2],matrixfloat[1][2],matrixfloat[2][2],matrixfloat[3][2],
		matrixfloat[0][3],matrixfloat[1][3],matrixfloat[2][3],matrixfloat[3][3]
	};

	GameObject* parent = light->GetParent();
	CompTransform* trans = parent->GetComponentTransform();


	float3 pos_light = light->GetParent()->GetComponentTransform()->GetPos();
	float3 rot_eu_ang = light->GetParent()->GetComponentTransform()->GetRotEuler();


	Quat rot = light->GetParent()->GetComponentTransform()->GetRot();

	float3 dir = light->GetParent()->GetComponentTransform()->GetEulerToDirection();

	float4x4 MVP = camFrust.ProjectionMatrix()* camFrust.ViewMatrix() * matrixfloat;

	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0,
		0.0, 0.5, 0.0, 0,
		0.0, 0.0, 0.5, 0,
		0.5, 0.5, 0.5, 1.0
	);


	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	//glm::mat4 depthViewMatrix = glm::lookAt(vec3(rot_eu_ang.x, rot_eu_ang.y, rot_eu_ang.z), glm::vec3(pos_light.x, pos_light.y, pos_light.z), glm::vec3(0, 1, 0));
	glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(dir.x, dir.y, dir.z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMatrix = glm::mat4(1.0);
	//	depthModelMatrix[3][0] = pos_light.x;
	//depthModelMatrix[3][1] = pos_light.y;
	//depthModelMatrix[3][2] = pos_light.z;



	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	glm::mat4 depthBiasMVP = biasMatrix * depthMVP;

	//This is needed for the shadow projection in CompMesh cpp



	//----------------------------

	GLint modelLoc = glGetUniformLocation(shadow_Shader->programID, "model");
	uint depthMatrixID = glGetUniformLocation(shadow_Shader->programID, "depthMVP");


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, matrix);

	glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);


	int total_save_buffer = 8;

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
		GLuint ShadowMapID = glGetUniformLocation(material->GetProgramID(), "shadowMap");
		GLuint light_dir_id = glGetUniformLocation(material->GetProgramID(), "_light_dir");
		//-----------------------


		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(depthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniform3fv(light_dir_id, 1, dir.ptr());
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
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
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

bool ModuleLightning::SetEventListeners()
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

	l1->use_light_to_render = false;
	l2->use_light_to_render = false;

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