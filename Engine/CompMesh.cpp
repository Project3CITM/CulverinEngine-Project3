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
//Delete this
#include "glm\glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

void CompMesh::Draw()
{
	if (render && resource_mesh != nullptr)
	{
		ShaderProgram* shader = App->renderer3D->default_shader;
		//if (material->material_shader != nullptr)
		if (material != nullptr) shader = (ShaderProgram*)&material->material_shader;
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
				if (temp != nullptr) 
				{
					for (int i = 0; i < temp->material_shader.textures.size(); i++)
					{
						uint texLoc = glGetUniformLocation(temp->material_shader.programID, temp->material_shader.textures[i].var_name.c_str());
						glUniform1i(texLoc, i);

						glActiveTexture(GL_TEXTURE0 +i);
					
						if (temp->material_shader.textures[i].value == nullptr)
						{
							glBindTexture(GL_TEXTURE_2D, App->renderer3D->id_checkImage);
						}
						else
						{
							glBindTexture(GL_TEXTURE_2D, temp->material_shader.textures[i].value->GetTextureID());
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
			SetUniformVariables(shader);

			Frustum camFrust = App->renderer3D->active_camera->frustum;// App->camera->GetFrustum();
			float4x4 temp = camFrust.ViewMatrix();

			GLint view2Loc = glGetUniformLocation(shader->programID, "view");
			GLint modelLoc = glGetUniformLocation(shader->programID, "model");
			GLint viewLoc =  glGetUniformLocation(shader->programID, "viewproj");

			float4x4 matrixfloat = transform->GetGlobalTransform();

			GLfloat matrix[16] =
			{
				matrixfloat[0][0],matrixfloat[1][0],matrixfloat[2][0],matrixfloat[3][0],
				matrixfloat[0][1],matrixfloat[1][1],matrixfloat[2][1],matrixfloat[3][1],
				matrixfloat[0][2],matrixfloat[1][2],matrixfloat[2][2],matrixfloat[3][2],
				matrixfloat[0][3],matrixfloat[1][3],matrixfloat[2][3],matrixfloat[3][3]
			};
			
			glUniformMatrix4fv(view2Loc, 1, GL_TRUE, temp.Inverted().ptr());			
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, matrix);
			glUniformMatrix4fv(viewLoc, 1, GL_TRUE, camFrust.ViewProjMatrix().ptr());

			//---------------------------------------------
	
		//--------------this
			float4x4 MVP =  camFrust.ProjectionMatrix()* camFrust.ViewMatrix() * matrixfloat;
			
			glm::mat4 biasMatrix(
				0.5, 0.0, 0.0, 0,
				0.0, 0.5, 0.0, 0,
				0.0, 0.0, 0.5, 0,
				0.5, 0.5, 0.5, 1.0
			);
		
			glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);
			glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
			//glm::mat4 depthViewMatrix = glm::lookAt(vec3(rot_eu_ang.x, rot_eu_ang.y, rot_eu_ang.z), glm::vec3(pos_light.x, pos_light.y, pos_light.z), glm::vec3(0, 1, 0));
			glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			glm::mat4 depthModelMatrix = glm::mat4(1.0);
			glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
			glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
			
			int depthMatrixID = glGetUniformLocation(shader->programID, "depthMVP");
			int depthBiasID = glGetUniformLocation(shader->programID, "depthBias");
			GLuint ShadowMapID = glGetUniformLocation(shader->programID, "shadowMap");
			//-----------------------

			//glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &MVP[0][0]);
			//glUniformMatrix4fv(depthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

			if (shader->name == "Shadow_World_Render")
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, App->module_lightning->text.GetTexture());
				glUniform1i(shader->programID, App->module_lightning->text.rbo);
				glUniform1i(ShadowMapID, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, App->module_lightning->text.GetTexture());
				glUniform1i(shader->programID, App->module_lightning->text.rbo);
				glUniform1i(ShadowMapID, 1);
			}

			int total_save_buffer = 8;

			if (skeleton != nullptr)
				total_save_buffer += 16;

			if (resource_mesh->vertices.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, resource_mesh->id_total_buffer);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (0 * sizeof(float)));
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (5 * sizeof(float)));

				if (skeleton != nullptr)
				{
					glEnableVertexAttribArray(3);
					glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (8 * sizeof(float)));
					glEnableVertexAttribArray(4);
					glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, total_save_buffer * sizeof(GLfloat), (char *)NULL + (12 * sizeof(float)));
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

void CompMesh::SetUniformVariables(ShaderProgram * shader)
{
	shader->RestartIterators();
	//BOOL
	if(shader->bool_variables.size() != 0)
	while (shader->it_bool_variables != shader->bool_variables.end()) {

		GLint bool_loc = glGetUniformLocation(shader->programID, (*shader->it_bool_variables).var_name.c_str());
		glUniform1i(bool_loc,(*shader->it_bool_variables).value);
		shader->it_bool_variables++;
	}
	//INT
	if (shader->int_variables.size() != 0)
	while (shader->it_int_variables != shader->int_variables.end()) {
		GLint int_loc = glGetUniformLocation(shader->programID, (*shader->it_int_variables).var_name.c_str());
		glUniform1i(int_loc, (*shader->it_int_variables).value);
		shader->it_int_variables++;
	}
	if (shader->float_variables.size() != 0)
	while (shader->it_float_variables != shader->float_variables.end()) {
		GLint float_loc = glGetUniformLocation(shader->programID, (*shader->it_float_variables).var_name.c_str());		
		glUniform1f(float_loc, (*shader->it_float_variables).value);
		shader->it_float_variables++;
	}

	if (shader->float3_variables.size() != 0)
	while (shader->it_float3_variables != shader->float3_variables.end()) {
		GLint float3_loc = glGetUniformLocation(shader->programID, (*shader->it_float3_variables).var_name.c_str());
		glUniform3fv(float3_loc,1, &(*shader->it_float3_variables).value[0]);
		shader->it_float3_variables++;
	}

	if (shader->color_variables.size() != 0)
	while (shader->it_color_variables != shader->color_variables.end()) {
		GLint color_loc = glGetUniformLocation(shader->programID, (*shader->it_color_variables).var_name.c_str());
		glUniform4fv(color_loc,1, &(*shader->it_color_variables).value[0]);
		shader->it_color_variables++;
	}

	shader->RestartIterators();
}

bool CompMesh::HasSkeleton() const
{
	if (resource_mesh != nullptr) return resource_mesh->HasSkeleton();
	return false;
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

	skeleton->bones.reserve(source->num_bones);
	skeleton->influences.resize(resource_mesh->num_vertices);

	for (int i = 0; i < resource_mesh->num_vertices; i++)
		skeleton->influences[i].resize(source->vertex_weights[i].size());

	parent->AddChildGameObject(new_skeleton);
	new_skeleton->AddChildGameObject(GenBone(&name_iterator, source, generated_bones, skeleton));
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
			comp_bone->offset = source->bones[i].offset;

			for (int j = 0; j < source->bones[i].num_weights; j++)
				comp_bone->weights.push_back(CompBone::Weight(source->bones[i].weights[j].weight, source->bones[i].weights[j].vertex_id));
		}

	ImportBone& bone = source->bones[generated_bones];

	for (int i = 0; i < bone.num_weights; i++)
	{
		uint pos = 0;

		for (std::vector<std::pair<uint, float>>::const_iterator it = source->vertex_weights[i].begin(); it != source->vertex_weights[i].end(); ++it)
		{
			if (it->first == generated_bones)
				break;
			pos++;
		}
		
		skeleton->influences[bone.weights->vertex_id][pos] = new_bone;
	}

	uint num_childs = source->bone_hirarchy_num_childs[generated_bones];
	generated_bones++;
	
	skeleton->bones.push_back(new_bone);

	for (int i = 0; i < num_childs; i++)
		new_bone->AddChildGameObject(GenBone(name_iterator, source, generated_bones, skeleton));

	return new_bone;
}

CompMaterial * CompMesh::GetMaterial() const
{
	return (CompMaterial*)material;
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
