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


CompMesh::CompMesh(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Mesh";
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
	has_normals = copy.has_normals;
	render = copy.render;

	name_component = "Mesh";
}

CompMesh::~CompMesh()
{
	//RELEASE_ARRAY(name);
	if (resource_mesh != nullptr)
	{
		if (resource_mesh->num_game_objects_use_me > 0)
		{
			resource_mesh->num_game_objects_use_me--;
		}
	}
	material = nullptr;
	resource_mesh = nullptr;
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
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%s", resource_mesh->name);
		ImGui::Text("Vertices:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%i", resource_mesh->num_vertices);
		ImGui::Text("Indices:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%i", resource_mesh->num_indices);

		ImGui::Checkbox("Render", &render);
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

void CompMesh::Draw()
{

	
	if (render && resource_mesh != nullptr)
	{
		ShaderProgram* shader = App->renderer3D->default_shader;
		//if (material->material_shader)
			//shader = material->material_shader;

		shader->Bind();

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

			//Temporary, future delete incoming
			//Set Color
			if (material != nullptr)
			{
				glColor4f(material->GetColor().r, material->GetColor().g, material->GetColor().b, material->GetColor().a);
			}
			else
			{
				glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
			}
			//


			//Future Delete
			if (App->renderer3D->texture_2d)
			{
				CompMaterial* temp = parent->GetComponentMaterial();
				if (temp != nullptr) {
					for (int i = 0; i < temp->material_shader.textures.size(); i++) {
						glActiveTexture(GL_TEXTURE0 + i);

						if (temp->material_shader.textures[i].res_material == nullptr)
						{
							glBindTexture(GL_TEXTURE_2D, App->renderer3D->id_checkImage);
						}
						else
						{
							glBindTexture(GL_TEXTURE_2D, temp->material_shader.textures[i].res_material->GetTextureID());
						}

					}
					/*

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
					*/

				}
			}		
			//
			


			// NORMALS ----------------------------------
			if (App->renderer3D->normals && has_normals)
			{
				glBindBuffer(GL_ARRAY_BUFFER, resource_mesh->vertices_norm_id);
				glVertexPointer(3, GL_FLOAT, sizeof(float3), NULL);
				glDrawArrays(GL_LINES, 0, resource_mesh->vertices.size() * 2);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

	
			Frustum camFrust = App->camera->GetFrustum();
			float4x4 temp = camFrust.ViewMatrix();

			GLint view2Loc = glGetUniformLocation(shader->programID, "view");
			GLint modelLoc = glGetUniformLocation(shader->programID, "model");
			GLint viewLoc =  glGetUniformLocation(shader->programID, "viewproj");

			

			glUniformMatrix4fv(view2Loc, 1, GL_TRUE, temp.Inverted().ptr());			
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform->GetMultMatrixForOpenGL());			
			glUniformMatrix4fv(viewLoc, 1, GL_TRUE, camFrust.ViewProjMatrix().ptr());


			int total_save_buffer = 8;

			if (resource_mesh->vertices.size()>0) {
				glBindBuffer(GL_ARRAY_BUFFER, resource_mesh->id_total_buffer);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (0 * sizeof(float)));
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (5 * sizeof(float)));

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

			shader->Unbind();
			
		}

		else
		{
			LOG("Cannot draw the mesh");
		}
	}
}

void CompMesh::Clear()
{
	resource_mesh = nullptr;
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
		material = mat;
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
			json_object_dotset_string_with_std(object, "Info.Resources.Resource " + temp + ".Name", resource_mesh->name);
		}
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", resource_mesh->GetUUID());
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
		}
	}
	Enable();
}
