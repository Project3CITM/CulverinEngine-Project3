#include "Application.h"
#include "CompCubeMapRenderer.h"
#include "ModuleRenderer3D.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GameObject.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "ModuleEventSystemV2.h"
#include "EventDefV2.h"
#include "CubeMap_Texture.h"
#include "ResourceMesh.h"
#include "CompMaterial.h"
#include "ModuleLightning.h"
#include "ModuleFS.h"
CompCubeMapRenderer::CompCubeMapRenderer(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	name_component = "CubeMapRenderer";	
	cube_map.Create();
	App->renderer3D->cube_maps.push_back(&cube_map);
}

CompCubeMapRenderer::~CompCubeMapRenderer()
{
	auto item = std::find(App->renderer3D->cube_maps.begin(), App->renderer3D->cube_maps.end(), &cube_map);
	App->renderer3D->cube_maps.erase(item);
}

void CompCubeMapRenderer::Bake(Event& event)
{
	//cube_map.Destroy();
	//cube_map.Create();
	uint depth_cube_map_index = 0;

	float3 light_pos = GetGameObjectPos();

	CubeMap_Texture* fbo = &cube_map;

	fbo->Bind();
	


	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);

	uint size = 1024;
	glViewport(0, 0, size, size);

	//
	float near_plane = 1.f;
	float far_plane = 50.f; //TODO: Change it for light range
	glm::mat4 shadow_proj = glm::perspective(glm::radians(90.f), (float)size / (float)size, near_plane, far_plane);

	std::vector<glm::mat4> shadow_transforms;
	glm::vec3 l_pos = glm::vec3(light_pos.x, light_pos.y, light_pos.z);
	shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
	shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
	shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
	shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));
	shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)));
	shadow_transforms.push_back(shadow_proj * glm::lookAt(l_pos, l_pos + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)));

	
	App->renderer3D->cube_map_shader->Bind();
	for (uint i = 0; i < 6; ++i)
	{

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, fbo->GetTextureId(), 0);
		glClear(GL_COLOR_BUFFER_BIT);
		//GET VECTOR OF GAMEOBJECTS
	
		for (std::vector<GameObject*>::const_iterator item = event.cube_map_draw.all_gameobjects->begin(); item != event.cube_map_draw.all_gameobjects->end(); item++)
		{
				
			CompMesh* m = (*item)->GetComponentMesh();

			if (m == nullptr)
				continue;

			uint sh_id = m->GetMaterial()->material->GetProgramID();
			m->GetMaterial()->material->Bind();
			float3 object_pos = m->GetGameObjectPos();

			CompTransform* transform = (*item)->GetComponentTransform();
			float4x4 matrixfloat = transform->GetGlobalTransform();

			GLfloat matrix[16] =
			{
				matrixfloat[0][0],matrixfloat[1][0],matrixfloat[2][0],matrixfloat[3][0],
				matrixfloat[0][1],matrixfloat[1][1],matrixfloat[2][1],matrixfloat[3][1],
				matrixfloat[0][2],matrixfloat[1][2],matrixfloat[2][2],matrixfloat[3][2],
				matrixfloat[0][3],matrixfloat[1][3],matrixfloat[2][3],matrixfloat[3][3]
			};
	
			App->module_shaders->SetUniformVariables(m->GetMaterial()->material);

			GLint modelLoc = glGetUniformLocation(sh_id, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, matrix);

			GLint viewLoc = glGetUniformLocation(sh_id, "viewproj");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &((shadow_transforms[i])[0][0]));

			glUniform1f(glGetUniformLocation(sh_id, "_far_plane"), far_plane);
			glUniform3fv(glGetUniformLocation(sh_id, "_light_pos"), 1, &l_pos.x);


			int total_save_buffer = 14;
			ResourceMesh* mesh = m->resource_mesh;
			glBindBuffer(GL_ARRAY_BUFFER, mesh->id_total_buffer);


			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (0 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) , (char *)NULL + (3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (5 * sizeof(float)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (8 * sizeof(float)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) , (char *)NULL + (11 * sizeof(float)) );
			
			glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_id);
			glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);

		}
	}
	
	//Reset viewport
	glViewport(viewport_size[0], viewport_size[1], viewport_size[2], viewport_size[3]);

	//Unbind shader and cube map
	glUseProgram(0);
	fbo->UnBind();

	++depth_cube_map_index;



}

void CompCubeMapRenderer::ShowInspectorInfo()
{
	char namedit[150];
	strcpy_s(namedit, 150, cube_map.GetName().c_str());
	if (ImGui::InputText("##nameModel", namedit, 150))
	{
		cube_map.SetName(App->fs->ConverttoChar(std::string(namedit).c_str()));
	}
	if (ImGui::Button("Bake")) {
		Event draw_event;
		draw_event.Set_event_data(EventType::EVENT_CUBEMAP_REQUEST);
		draw_event.cube_map_request.comp_cubemap = this;
		PushEvent(draw_event);
	}
	ImGui::TreePop();
}

