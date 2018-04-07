#include "Application.h"
#include "Globals.h"
#include "Component.h"
#include "CompMesh.h"
#include "CompMaterial.h"
#include "CompTransform.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "Scene.h"
#include "ImportMesh.h"
#include "ResourceMesh.h"
#include "Color.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "ModuleCamera3D.h"
#include <vector>
#include "Math\float3.h"
#include "CompCamera.h"
#include "CompBone.h"
#include "ModuleLightning.h"
#include "ModuleWindow.h"

//Delete this
#include "glm\glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


CompMesh::CompMesh(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	uid = App->random->Int();
	name_component = "CompMesh";
}

CompMesh::CompMesh(const CompMesh& copy, GameObject* parent) :Component(Comp_Type::C_MESH, parent)
{
	name = copy.name;
	resource_mesh = copy.resource_mesh;
	if (resource_mesh != nullptr)
	{
		resource_mesh->num_game_objects_use_me++;
	}
	//material = material;

	render = copy.render;

	name_component = "CompMesh";
}

CompMesh::~CompMesh()
{
	//RELEASE_ARRAY(name);
	
}

//void CompMesh::Init(std::vector<_Vertex> v, std::vector<uint> i)
//{
//	vertices = v;
//	indices = i;
//
//	float3 vert_origin;
//	float3 vert_norm;
//
//	SetupMesh();
//}

void CompMesh::PreUpdate(float dt)
{
	// Manage Resource -------------------------------------------------
	// Before delete Resource Set this pointer to nullptr
	if (resource_mesh != nullptr)
	{
		if (resource_mesh->GetState() == Resource::State::WANTDELETE)
		{
			resource_mesh = nullptr;
		}
		else if (resource_mesh->GetState() == Resource::State::REIMPORTED)
		{
			uuid_resource_reimported = resource_mesh->GetUUID();
			resource_mesh = nullptr;
		}
	}
	else
	{
		if (uuid_resource_reimported != 0)
		{
			resource_mesh = (ResourceMesh*)App->resource_manager->GetResource(uuid_resource_reimported);
			if (resource_mesh != nullptr)
			{
				resource_mesh->num_game_objects_use_me++;
				// Check if loaded!
				if (resource_mesh->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMesh->LoadResource(std::to_string(resource_mesh->GetUUID()).c_str(), resource_mesh);
				}
				uuid_resource_reimported = 0;
			}
		}
	}
	// -------------------------------------------------------------------
}

void CompMesh::Update(float dt)
{

	if (parent->GetComponentTransform()->GetUpdated())
	{
		if (resource_mesh != nullptr)
		{
			parent->box_fixed = resource_mesh->aabb_box;
		}
		parent->box_fixed.TransformAsAABB(parent->GetComponentTransform()->GetGlobalTransform());
	}
}

void CompMesh::ShowOptions()
{
	//ImGui::MenuItem("CREATE", NULL, false, false);
	if (ImGui::MenuItem("Reset"))
	{
		// Not implmented yet.
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component"))
	{
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetComponentCopy(this);
	}
	if (ImGui::MenuItem("Paste Component As New", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (parent->FindComponentByType(((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType()) == nullptr
			|| ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType() > Comp_Type::C_CAMERA)
		{
			parent->AddComponentCopy(*((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied());
		}
	}
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompMesh*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Reset Mesh"))
	{
		if (resource_mesh != nullptr)
		{
			if (resource_mesh->num_game_objects_use_me > 0)
			{
				resource_mesh->num_game_objects_use_me--;
			}
		}
		resource_mesh = nullptr;
		ImGui::CloseCurrentPopup();
	}
	if (ImGui::MenuItem("Select Mesh..."))
	{
		select_mesh = true;
		ImGui::CloseCurrentPopup();
	}
}

void CompMesh::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsMesh");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsMesh"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	if (resource_mesh != nullptr)
	{
		ImGui::Text("Name:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%s", resource_mesh->name.c_str());
		ImGui::Text("Vertices:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%i", resource_mesh->num_vertices);
		ImGui::Text("Indices:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%i", resource_mesh->num_indices);

		ImGui::Checkbox("Render", &render);

		if (skeleton != nullptr)
		{
			ImGui::Checkbox("Debug Skeleton", &debug_skeleton);

			if (debug_skeleton)
				skeleton->DebugDraw();
		}
	}
	if (resource_mesh == nullptr)
	{
		ImGui::Text("Name:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "None (Mesh)");
		if (ImGui::Button("Select Mesh..."))
		{
			select_mesh = true;
		}
	}
	if (resource_mesh == nullptr || select_mesh)
	{
		if (select_mesh)
		{
			ResourceMesh* temp = (ResourceMesh*)App->resource_manager->ShowResources(select_mesh, Resource::Type::MESH);
			if (temp != nullptr)
			{
				if (resource_mesh != nullptr)
				{
					if (resource_mesh->num_game_objects_use_me > 0)
					{
						resource_mesh->num_game_objects_use_me--;
					}
				}
				resource_mesh = temp;
				resource_mesh->num_game_objects_use_me++;
				if (resource_mesh->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMesh->LoadResource(std::to_string(resource_mesh->GetUUID()).c_str(), resource_mesh);
				}
				Enable();
				parent->AddBoundingBox(resource_mesh);
			}
		}
	}
	ImGui::TreePop();
}

void CompMesh::Draw(bool alpha)
{
	if (alpha)
	{
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}

	
	if (render && resource_mesh != nullptr)
	{
		Material* material = App->renderer3D->default_material;
		//if (material->material_shader != nullptr)
		if (comp_material != nullptr) material = comp_material->material;


		//DRAW MODE
		switch (App->renderer3D->render_mode) {
		case RenderMode::DEFAULT:
			glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
			break;
		case RenderMode::GLOW:
			if (!material->glow) material = App->renderer3D->non_glow_material;
			glViewport(0, 0, 128, 128);
			break;
		case RenderMode::DEPTH:

			break;
		}

		//BIND MATERIAL
		material->Bind();
		
	
		CompTransform* transform = (CompTransform*)parent->FindComponentByType(C_TRANSFORM);
	
		if (resource_mesh->vertices.size() > 0 && resource_mesh->indices.size() > 0)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			//Set Wireframe
			if (App->renderer3D->wireframe)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			int i = 0;
			if (App->renderer3D->texture_2d)
			{
				CompMaterial* temp = parent->GetComponentMaterial();
				if (temp != nullptr) 
				{
					for ( i = 0; i < temp->material->textures.size(); i++)
					{
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
				
				}
			}		
			

			Frustum camFrust = App->renderer3D->active_camera->frustum;
			float4x4 temp = camFrust.ViewMatrix();

			GLint view2Loc = glGetUniformLocation(material->GetProgramID(), "view");
			GLint modelLoc = glGetUniformLocation(material->GetProgramID(), "model");
			GLint viewLoc =  glGetUniformLocation(material->GetProgramID(), "viewproj");
			GLint modelviewLoc = glGetUniformLocation(material->GetProgramID(), "modelview");

			float4x4 matrixfloat = transform->GetGlobalTransform();

			GLfloat matrix[16] =
			{
				matrixfloat[0][0],matrixfloat[1][0],matrixfloat[2][0],matrixfloat[3][0],
				matrixfloat[0][1],matrixfloat[1][1],matrixfloat[2][1],matrixfloat[3][1],
				matrixfloat[0][2],matrixfloat[1][2],matrixfloat[2][2],matrixfloat[3][2],
				matrixfloat[0][3],matrixfloat[1][3],matrixfloat[2][3],matrixfloat[3][3]
			};
			
			float4x4 ModelViewMatrix = temp.Inverted() * matrixfloat;
			

			glUniformMatrix4fv(view2Loc, 1, GL_TRUE, temp.Inverted().ptr());			
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, matrix);
			glUniformMatrix4fv(viewLoc, 1, GL_TRUE, camFrust.ViewProjMatrix().ptr());
			glUniformMatrix4fv(modelviewLoc, 1, GL_TRUE, ModelViewMatrix.ptr());

	
			float4x4 MVP =  camFrust.ProjectionMatrix()* camFrust.ViewMatrix() * matrixfloat;
			
			glm::mat4 biasMatrix(
				0.5, 0.0, 0.0, 0,
				0.0, 0.5, 0.0, 0,
				0.0, 0.0, 0.5, 0,
				0.5, 0.5, 0.5, 1.0
			);
		
			glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);
			glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);		
			glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			glm::mat4 depthModelMatrix = glm::mat4(1.0);
			glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
			glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
			
			int depthMatrixID = glGetUniformLocation(material->GetProgramID(), "depthMVP");
			int depthBiasID = glGetUniformLocation(material->GetProgramID(), "depthBias");
			GLuint ShadowMapID = glGetUniformLocation(material->GetProgramID(), "shadowMap");
		

			if (material->name == "Shadow_World_Render")
			{
				glActiveTexture(GL_TEXTURE0 + (++i));
				glBindTexture(GL_TEXTURE_2D, App->module_lightning->test_fix.depthTex);
				glUniform1i(material->GetProgramID(), App->module_lightning->test_fix.depthTex);
				glUniform1i(ShadowMapID, i);
			}

			int total_save_buffer = 14;
			uint bones_size_in_buffer = 0;

			if (skeleton != nullptr)
			{

				bones_size_in_buffer = 4 * sizeof(GLint) + 4 * sizeof(GLfloat);

				GLuint skinning_texture_id = glGetUniformLocation(material->GetProgramID(), "_skinning_text");

				//Gen text
				skeleton->GenSkinningTexture(parent);

				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_BUFFER, skeleton->skinning_mats_id);
				glUniform1i(skinning_texture_id, i);
			}

			if (resource_mesh->vertices.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, resource_mesh->id_total_buffer);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (0 * sizeof(float)));
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (5 * sizeof(float)));

				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (8 * sizeof(float)) + bones_size_in_buffer);
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (11 * sizeof(float)) + bones_size_in_buffer);
				
				
				if (skeleton != nullptr)
				{
					glEnableVertexAttribArray(3);
					glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (8 * sizeof(float)));

					glEnableVertexAttribArray(4);
					glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (12 * sizeof(float)));
				}
				
				glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource_mesh->indices_id);
				glDrawElements(GL_TRIANGLES, resource_mesh->num_indices, GL_UNSIGNED_INT, NULL);
			}

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
		
			material->Unbind();		
		}
		else
		{
			LOG("Cannot draw the mesh");
		}
	}
	if (alpha)
	{
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	}


}

void CompMesh::Draw2(uint ID)
{
	if (render && resource_mesh != nullptr)
	{
		//BIND MATERIAL
		CompTransform* transform = (CompTransform*)parent->FindComponentByType(C_TRANSFORM);
		if (resource_mesh->vertices.size() > 0 && resource_mesh->indices.size() > 0)
		{
			if (App->renderer3D->texture_2d)
			{
				
			}
			uint TexturesSize = parent->GetComponentMaterial()->material->textures.size();
			
			//float4x4 temp = camFrust.ViewMatrix();
			//GLint view2Loc = glGetUniformLocation(ID, "view");
			GLint modelLoc = glGetUniformLocation(ID, "model");			
			//GLint modelviewLoc = glGetUniformLocation(ID, "modelview");
			float4x4 matrixfloat = transform->GetGlobalTransform();

			GLfloat matrix[16] =
			{
				matrixfloat[0][0],matrixfloat[1][0],matrixfloat[2][0],matrixfloat[3][0],
				matrixfloat[0][1],matrixfloat[1][1],matrixfloat[2][1],matrixfloat[3][1],
				matrixfloat[0][2],matrixfloat[1][2],matrixfloat[2][2],matrixfloat[3][2],
				matrixfloat[0][3],matrixfloat[1][3],matrixfloat[2][3],matrixfloat[3][3]
			};
			//float4x4 ModelViewMatrix = temp.Inverted() * matrixfloat;
			//glUniformMatrix4fv(view2Loc, 1, GL_TRUE, temp.Inverted().ptr());
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, matrix);

			//glUniformMatrix4fv(viewLoc, 1, GL_TRUE, camFrust.ViewProjMatrix().ptr());
			//glUniformMatrix4fv(modelviewLoc, 1, GL_TRUE, ModelViewMatrix.ptr());
			//Shadow mapping
			/*
			float4x4 MVP = camFrust.ProjectionMatrix()* camFrust.ViewMatrix() * matrixfloat;
			glm::mat4 biasMatrix(
				0.5, 0.0, 0.0, 0,
				0.0, 0.5, 0.0, 0,
				0.0, 0.0, 0.5, 0,
				0.5, 0.5, 0.5, 1.0
			);
			glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);
			glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
			glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			glm::mat4 depthModelMatrix = glm::mat4(1.0);
			glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
			glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
			int depthMatrixID = glGetUniformLocation(ID, "depthMVP");
			int depthBiasID = glGetUniformLocation(ID, "depthBias");
			GLuint ShadowMapID = glGetUniformLocation(ID, "shadowMap");
			if (material->name == "Shadow_World_Render")
			{
				glActiveTexture(GL_TEXTURE0 + (++TexturesSize));
				glBindTexture(GL_TEXTURE_2D, App->module_lightning->test_fix.depthTex);
				glUniform1i(ID, App->module_lightning->test_fix.depthTex);
				glUniform1i(ShadowMapID, TexturesSize);
			}
			*/
			int total_save_buffer = 14;
			uint bones_size_in_buffer = 0;
			if (skeleton != nullptr)
			{
				bones_size_in_buffer = 4 * sizeof(GLint) + 4 * sizeof(GLfloat);
				GLuint skinning_texture_id = glGetUniformLocation(ID, "_skinning_text");
				skeleton->GenSkinningTexture(parent);
				glActiveTexture(GL_TEXTURE0 + TexturesSize);
				glBindTexture(GL_TEXTURE_BUFFER, skeleton->skinning_mats_id);
				glUniform1i(skinning_texture_id, TexturesSize);
			}
			if (resource_mesh->vertices.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, resource_mesh->id_total_buffer);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (0 * sizeof(float)));
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (5 * sizeof(float)));
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (8 * sizeof(float)) + bones_size_in_buffer);
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (11 * sizeof(float)) + bones_size_in_buffer);
				if (skeleton != nullptr)
				{
					glEnableVertexAttribArray(3);
					glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (8 * sizeof(float)));
					glEnableVertexAttribArray(4);
					glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat) + bones_size_in_buffer, (char *)NULL + (12 * sizeof(float)));
				}
				glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource_mesh->indices_id);
				glDrawElements(GL_TRIANGLES, resource_mesh->num_indices, GL_UNSIGNED_INT, NULL);
			}
			//Reset TextureColor
			//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
		}
		else
			LOG("Cannot draw the mesh");
	}
}

void CompMesh::Clear()
{
	if (resource_mesh != nullptr)
	{
		if (resource_mesh->num_game_objects_use_me > 0)
		{
			resource_mesh->num_game_objects_use_me--;
		}
		resource_mesh = nullptr;
	}

	if (comp_material != nullptr)
	{
		comp_material->material->active_num--;
		comp_material = nullptr;
	}
}

void CompMesh::Render(bool render)
{
	this->render = render;
}

bool CompMesh::IsRendering() const
{
	return render;
}

void CompMesh::LinkMaterial(const CompMaterial* mat)
{
	if (mat != nullptr)
	{
		comp_material = mat;
		LOG("MATERIAL linked to MESH %s", name);
	}
}

void CompMesh::SetResource(ResourceMesh* resourse_mesh, bool isImport)
{
	if (resource_mesh != resourse_mesh)
	{
		if (resource_mesh != nullptr)
		{
			if (resource_mesh->num_game_objects_use_me > 0)
			{
				resource_mesh->num_game_objects_use_me--;
			}
		}
		resource_mesh = resourse_mesh;
		if (isImport)
		{
			// Fix Bug with Delete Import -----
			resource_mesh->num_game_objects_use_me = 0;
		}
		else
		{
			resource_mesh->num_game_objects_use_me++;
		}
	}
}



void CompMesh::CopyValues(const CompMesh* component)
{
	resource_mesh = component->resource_mesh;
	//more...
}

void CompMesh::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", C_MESH);
	json_object_dotset_number_with_std(object, name + "UUID", uid);

	if(resource_mesh != nullptr)
	{
		if (saveScene == false)
		{
			// Save Info of Resource in Prefab (next we use this info for Reimport this prefab)
			std::string temp = std::to_string(countResources++);
			json_object_dotset_number_with_std(object, "Info.Resources.Resource " + temp + ".UUID Resource", resource_mesh->GetUUID());
			json_object_dotset_string_with_std(object, "Info.Resources.Resource " + temp + ".Name", resource_mesh->name.c_str());
			json_object_dotset_number_with_std(object, "Info.Resources.Resource " + temp + ".Type", (int)resource_mesh->GetType());
		}
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", resource_mesh->GetUUID());

		json_object_dotset_boolean_with_std(object, name + "Has Skeleton", has_skeleton);
		json_object_dotset_boolean_with_std(object, name + "Generated Skeleton", generated_skeleton);

		if (skeleton != nullptr)
			skeleton->Save(object, name);
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", 0);
	}
}

void CompMesh::Load(const JSON_Object* object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Mesh UUID");

	if (resourceID > 0)
	{
		resource_mesh = (ResourceMesh*)App->resource_manager->GetResource(resourceID);
		if (resource_mesh != nullptr)
		{
			resource_mesh->num_game_objects_use_me++;
			
			// LOAD MESH ----------------------------
			if (resource_mesh->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMesh->LoadResource(std::to_string(resource_mesh->GetUUID()).c_str(), resource_mesh);
			}
			// Add bounding box ------
			parent->AddBoundingBox(resource_mesh);

			has_skeleton = json_object_dotget_boolean_with_std(object, name + "Has Skeleton");
			generated_skeleton = json_object_dotget_boolean_with_std(object, name + "Generated Skeleton");

			if (has_skeleton == true)
			{
				if (generated_skeleton == true)
				{
					skeleton = new Skeleton;
					skeleton->Load(object, name);
				}
				else
					GenSkeleton();
			}

		}
	}

	Enable();
}

void CompMesh::SyncComponent(GameObject * sync_parent)
{
	parent->GetComponentTransform()->UpdateGlobalTransform();
	if (resource_mesh != nullptr)
	{
		parent->box_fixed = resource_mesh->aabb_box;
	}
	float4x4 temp = parent->GetComponentTransform()->GetGlobalTransform();
	parent->box_fixed.TransformAsAABB(temp);
}

void CompMesh::LinkSkeleton()
{
	if (skeleton != nullptr)
		skeleton->Link(resource_mesh->skeleton);
}

bool CompMesh::HasSkeleton() const
{
	if (resource_mesh != nullptr) return resource_mesh->HasSkeleton();
	return false;
}

void CompMesh::SetSkeleton(bool has_skeleton)
{
	this->has_skeleton = has_skeleton;
}

void CompMesh::GenSkeleton()
{
	SkeletonSource* source = resource_mesh->skeleton;
	skeleton = new Skeleton;

	char* name_iterator = source->bone_hirarchy_names;

	uint generated_bones = 0;

	GameObject* new_skeleton = new GameObject("Skeleton");
	CompTransform* transform = (CompTransform*)new_skeleton->AddComponent(Comp_Type::C_TRANSFORM);

	float3 pos;
	Quat rot;
	float3 scale;

	source->transform.Decompose(pos, rot, scale);
	transform->SetPos(pos);
	transform->SetRot(rot);
	transform->SetScale(scale);

	skeleton->skinning_mats = new GLfloat[source->num_bones * 4 * 4]; //every vertex will be influenced by 4 float4x4

	skeleton->bones.resize(source->num_bones);

	parent->AddChildGameObject(new_skeleton);
	skeleton->root_bone = GenBone(&name_iterator, source, generated_bones, skeleton);
	new_skeleton->AddChildGameObject(skeleton->root_bone);
	has_skeleton = true;
	generated_skeleton = true;
}

GameObject* CompMesh::GenBone( char** name_iterator, const SkeletonSource* source, uint& generated_bones, Skeleton* skeleton)
{
	GameObject* new_bone = new GameObject(*name_iterator);
	*name_iterator += strlen(new_bone->GetName()) + 1;

	CompTransform* transform = (CompTransform*)new_bone->AddComponent(Comp_Type::C_TRANSFORM);

	float4x4 local_transform = source->bone_hirarchy_local_transforms[generated_bones];

	float3 pos;
	Quat rot;
	float3 scale;

	local_transform.Decompose(pos, rot, scale);
	transform->SetPos(pos);
	transform->SetRot(rot);
	transform->SetScale(scale);

	CompBone* comp_bone = (CompBone*)new_bone->AddComponent(Comp_Type::C_BONE);
	comp_bone->resource_mesh = resource_mesh;
	resource_mesh->num_game_objects_use_me++;

	for (int i = 0; i < source->num_bones; i++)
		if (source->bones[i].name == new_bone->GetName())
		{
			skeleton->bones[i] = new_bone;

			comp_bone->offset = source->bones[i].offset;

			for (int j = 0; j < source->bones[i].num_weights; j++)
			{
				CompBone::Weight weight;
				weight.vertex_id = source->bones[i].weights[j].vertex_id;
				weight.weight = source->bones[i].weights[j].weight;
				comp_bone->weights.push_back(weight);
			}
		}

	uint num_childs = source->bone_hirarchy_num_childs[generated_bones];
	generated_bones++;

	for (int i = 0; i < num_childs; i++)
	{
		GameObject* child = GenBone(name_iterator, source, generated_bones, skeleton);
		new_bone->AddChildGameObject(child);
		comp_bone->child_bones.push_back(child->GetComponentBone());
	}		

	return new_bone;
}

CompMaterial * CompMesh::GetMaterial() const
{
	return (CompMaterial*)comp_material;
}

void Skeleton::GenSkinningTexture(const GameObject* mesh_go)
{
	root_bone->GetComponentBone()->GenSkinningMatrix(root_bone->GetParent()->GetComponentTransform()->GetLocalTransform());

	for (int i = 0; i < bones.size(); i++)
	{
		uint pos_in_buffer = i * 4 * 4;

		float4x4 skinning_mat = bones[i]->GetComponentBone()->GetSkinningMatrix();

		float4 col1(skinning_mat.Col(0));
		memcpy(&skinning_mats[pos_in_buffer], &col1.x, sizeof(float) * 4);
		float4 col2(skinning_mat.Col(1));
		memcpy(&skinning_mats[pos_in_buffer + 4], &col2.x, sizeof(float) * 4);
		float4 col3(skinning_mat.Col(2));
		memcpy(&skinning_mats[pos_in_buffer + 8], &col3.x, sizeof(float) * 4);
		float4 col4(skinning_mat.Col(3));
		memcpy(&skinning_mats[pos_in_buffer + 12], &col4.x, sizeof(float) * 4);
	}

	GLint size = bones.size() * 4 * 4;

	glDeleteBuffers(1, &buffer_id);
	glGenBuffers(1, &buffer_id);

	glBindBuffer(GL_TEXTURE_BUFFER, buffer_id);
	glBufferData(GL_TEXTURE_BUFFER, size * sizeof(GLfloat), &skinning_mats[0], GL_DYNAMIC_READ);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glDeleteTextures(1, &skinning_mats_id);
	glGenTextures(1, &skinning_mats_id);
	glBindTexture(GL_TEXTURE_BUFFER, skinning_mats_id);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, buffer_id);

	glBindTexture(GL_TEXTURE_BUFFER, 0);
}

void Skeleton::DebugDraw() const
{
	float joint_sphere_radius = 0.5f;

	for (std::vector<GameObject*>::const_iterator it = bones.begin(); it != bones.end(); ++it)
	{
		glBegin(GL_LINES);

		float4 pos(0.0f, 0.0f, 0.0f, 1.0f);
		float4 x(1.0f, 0.0f, 0.0f, 1.0f);
		float4 y(0.0f, 1.0f, 0.0f, 1.0f);
		float4 z(0.0f, 0.0f, 1.0f, 1.0f);

		float4x4 transform((*it)->GetComponentTransform()->GetGlobalTransform());

		pos = transform * pos;

		x = transform * x;
		float3 x_vec(x.xyz() - pos.xyz());
		x_vec = x_vec / x_vec.Length() * joint_sphere_radius;
		x_vec = pos.xyz() + x_vec;

		y = transform * y;
		float3 y_vec(y.xyz() - pos.xyz());
		y_vec = y_vec / y_vec.Length() * joint_sphere_radius;
		y_vec = pos.xyz() + y_vec;

		z = transform * z;
		float3 z_vec(z.xyz() - pos.xyz());
		z_vec = z_vec / z_vec.Length() * joint_sphere_radius;
		z_vec = pos.xyz() + z_vec;

		//X
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(pos.x, pos.y, pos.z); glVertex3f(x_vec.x, x_vec.y, x_vec.z);

		//Y
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(pos.x, pos.y, pos.z); glVertex3f(y_vec.x, y_vec.y, y_vec.z);

		//Z
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(pos.x, pos.y, pos.z); glVertex3f(z_vec.x, z_vec.y, z_vec.z);

		//Bones
		glColor4f(1.0f, 0.65f, 0.0f, 1.0f);

		for (int i  = 0; i < (*it)->GetNumChilds(); i++)
		{
			if ((*it)->GetChildbyIndex(i)->GetComponentBone() != nullptr)
			{
				float4 child_vec(0.0f, 0.0f, 0.0f, 1.0f);

				float4x4 child_world_transform((*it)->GetChildbyIndex(i)->GetComponentTransform()->GetGlobalTransform());
				child_vec = child_world_transform * child_vec;

				glVertex3f(pos.x, pos.y, pos.z);
				glVertex3f(child_vec.x, child_vec.y, child_vec.z);
			}
		}

		glEnd();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		//Sphere
		Sphere sphere(pos.xyz() , joint_sphere_radius);
		sphere.Draw(0.0f, 0.65f, 1.0f, 1.0f);
	}
}

void Skeleton::Save(JSON_Object * object, std::string name) const
{
	json_object_dotset_number_with_std(object, name + "Bones Size", bones.size());

	for (int i = 0; i < bones.size(); i++)
	{
		char tmp[255];
		sprintf(tmp, "Bone %i.UUID", i);
		json_object_dotset_number_with_std(object, name + std::string(tmp), bones[i]->GetUUID());
	}

	json_object_dotset_number_with_std(object, name + "Root UUID", root_bone->GetUUID());
}

void Skeleton::Load(const JSON_Object * object, std::string name)
{
	uint num_bones = json_object_dotget_number_with_std(object, name + "Bones Size");
	bones.resize(num_bones);
	bone_uids.resize(num_bones);

	for (int i = 0; i < num_bones; i++)
	{
		char tmp[255];
		sprintf(tmp, "Bone %i.UUID", i);
		bone_uids[i] = json_object_dotget_number_with_std(object, name + std::string(tmp));
	}

	skinning_mats = new GLfloat[num_bones * 4 * 4];

	root_bone_uid = json_object_dotget_number_with_std(object, name + "Root UUID");
}

void Skeleton::Link(const SkeletonSource* source)
{
	for (int i = 0; i < bone_uids.size(); i++)
	{
		bones[i] = App->scene->GetGameObjectbyuid(bone_uids[i]);
		bones[i]->GetComponentBone()->Link();
	}

	root_bone = App->scene->GetGameObjectbyuid(root_bone_uid);
	root_bone->GetComponentBone()->Link();
}
