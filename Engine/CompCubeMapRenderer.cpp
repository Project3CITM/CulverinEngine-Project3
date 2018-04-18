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
#include "CompLight.h"
CompCubeMapRenderer::CompCubeMapRenderer(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	uid = App->random->Int();
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

	std::vector<Material*>::iterator item = App->module_shaders->materials.begin();
	while (item != App->module_shaders->materials.end())
	{
		uint ID = (*item)->GetProgramID();
		if ((*item)->active_num == 0) {
			item++;
			continue;
		}

		(*item)->Bind();
		std::vector<CompLight*> lights_vec = App->module_lightning->GetSceneLights();
		GLint lightsizeLoc = glGetUniformLocation(ID, "_numLights");
		if (lightsizeLoc != -1) glUniform1i(lightsizeLoc, lights_vec.size());




		if (lightsizeLoc != -1)
			for (size_t i = 0; i < lights_vec.size(); ++i) {

				if (lights_vec[i]->type == Light_type::DIRECTIONAL_LIGHT)
					App->module_shaders->SetLightUniform(ID, "position", i, lights_vec[i]->GetParent()->GetComponentTransform()->GetEulerToDirection());
				if (lights_vec[i]->type == Light_type::POINT_LIGHT)
					App->module_shaders->SetLightUniform(ID, "position", i, lights_vec[i]->GetParent()->GetComponentTransform()->GetPosGlobal());

				App->module_shaders->SetLightUniform(ID, "type", i, (int)lights_vec[i]->type);
				App->module_shaders->SetLightUniform(ID, "l_color", i, lights_vec[i]->color);
				App->module_shaders->SetLightUniform(ID, "ambientCoefficient", i, lights_vec[i]->ambientCoefficient);

				App->module_shaders->SetLightUniform(ID, "properties", i, lights_vec[i]->properties);

				// Bias matrix. TODO: Might be better to set other place

				int depthBiasID = glGetUniformLocation(ID, "depthBias");
				glUniformMatrix4fv(depthBiasID, 1, GL_FALSE, &lights_vec[i]->depthBiasMat[0][0]);

				// End Bias matrix.
			}
		item++;

	}


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
			if (m->HasSkeleton())
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
	ImGui::InputText("##nameModel", namedit, 150);
	cube_map.SetName(App->fs->ConverttoChar(std::string(namedit).c_str()));
	
	if (ImGui::Button("Bake")) {
		Event draw_event;
		draw_event.Set_event_data(EventType::EVENT_CUBEMAP_REQUEST);
		draw_event.cube_map_request.comp_cubemap = this;
		PushEvent(draw_event);
	}
	ImGui::TreePop();
}

void CompCubeMapRenderer::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	json_object_dotset_string_with_std(object, name + "Name:", cube_map.GetName().c_str());

}

void CompCubeMapRenderer::Load(const JSON_Object * object, std::string name)
{

	uid = json_object_dotget_number_with_std(object, name + "UUID");
	cube_map.SetName(json_object_dotget_string_with_std(object, name + "Name:"));
}

