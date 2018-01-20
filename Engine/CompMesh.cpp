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

#include <vector>


CompMesh::CompMesh(Comp_Type t, GameObject* parent) : Component(t, parent)
{
	uid = App->random->Int();
	nameComponent = "Mesh";
}

CompMesh::CompMesh(const CompMesh& copy, GameObject* parent) :Component(Comp_Type::C_MESH, parent)
{
	name = copy.name;
	resourceMesh = copy.resourceMesh;
	if (resourceMesh != nullptr)
	{
		resourceMesh->NumGameObjectsUseMe++;
	}
	//material = material;
	hasNormals = copy.hasNormals;
	render = copy.render;

	nameComponent = "Mesh";
}

CompMesh::~CompMesh()
{
	//RELEASE_ARRAY(name);
	if (resourceMesh != nullptr)
	{
		if (resourceMesh->NumGameObjectsUseMe > 0)
		{
			resourceMesh->NumGameObjectsUseMe--;
		}
	}
	material = nullptr;
	resourceMesh = nullptr;
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

void CompMesh::preUpdate(float dt)
{
	// Before delete Resource Set this pointer to nullptr
	if (resourceMesh != nullptr)
	{
		if (resourceMesh->GetState() == Resource::State::WANTDELETE)
		{
			resourceMesh = nullptr;
		}
		else if (resourceMesh->GetState() == Resource::State::REIMPORTED)
		{
			uuidResourceReimported = resourceMesh->GetUUID();
			resourceMesh = nullptr;
		}
	}
	else
	{
		if (uuidResourceReimported != 0)
		{
			resourceMesh = (ResourceMesh*)App->resource_manager->GetResource(uuidResourceReimported);
			if (resourceMesh != nullptr)
			{
				resourceMesh->NumGameObjectsUseMe++;
				// Check if loaded!
				if (resourceMesh->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMesh->LoadResource(std::to_string(resourceMesh->GetUUID()).c_str(), resourceMesh);
				}
				uuidResourceReimported = 0;
			}
		}
	}
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
		toDelete = true;
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component", NULL, false, false))
	{
		// Component* Copy = this;
	}
	if (ImGui::MenuItem("Paste Component", NULL, false, false))
	{
		//parent->AddComponent(App->scene->copyComponent->GetType())
		// Create contructor Component Copy or add funtion to add info
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
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 1.00f));
	if (ImGui::BeginPopup("OptionsMesh"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 2));
		if (ImGui::Button("Reset Mesh"))
		{
			if (resourceMesh != nullptr)
			{
				if (resourceMesh->NumGameObjectsUseMe > 0)
				{
					resourceMesh->NumGameObjectsUseMe--;
				}
			}
			resourceMesh = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::Separator();
		if (ImGui::Button("Select Mesh..."))
		{
			SelectMesh = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	ImGui::PopStyleColor();
	if (resourceMesh != nullptr)
	{
		ImGui::Text("Name:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%s", resourceMesh->name);
		ImGui::Text("Vertices:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%i", resourceMesh->num_vertices);
		ImGui::Text("Indices:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%i", resourceMesh->num_indices);

		ImGui::Checkbox("Render", &render);
	}
	if (resourceMesh == nullptr)
	{
		if (ImGui::Button("Select Mesh..."))
		{
			SelectMesh = true;
		}
	}
	if(resourceMesh == nullptr || SelectMesh)
	{
		//if (1)
		//{
		//	if (ImGui::Button("Select Mesh..."))
		//	{
		//		SelectMesh = true;
		//	}
		//}
		if (SelectMesh)
		{
			ResourceMesh* temp = (ResourceMesh*)App->resource_manager->ShowResources(SelectMesh, Resource::Type::MESH);
			if (temp != nullptr)
			{
				if (resourceMesh != nullptr)
				{
					if (resourceMesh->NumGameObjectsUseMe > 0)
					{
						resourceMesh->NumGameObjectsUseMe--;
					}
				}
				resourceMesh = temp;
				resourceMesh->NumGameObjectsUseMe++;
				if (resourceMesh->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMesh->LoadResource(std::to_string(resourceMesh->GetUUID()).c_str(), resourceMesh);
				}
				Enable();
				parent->AddBoundingBox(resourceMesh);
			}
		}
	}
	ImGui::TreePop();
}

void CompMesh::Draw()
{
	if (render && resourceMesh != nullptr)
	{
		CompTransform* transform = (CompTransform*)parent->FindComponentByType(C_TRANSFORM);
		if (transform != nullptr)
		{
			/* Push Matrix to Draw with transform applied, only if it contains a transform component */
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMultMatrixf(transform->GetMultMatrixForOpenGL());
		}

		if (resourceMesh->vertices.size() > 0 && resourceMesh->indices.size() > 0)
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

			//Set Color
			if (material != nullptr)
			{
				glColor4f(material->GetColor().r, material->GetColor().g, material->GetColor().b, material->GetColor().a);
			}
			else
			{
				glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
			}

			if (App->renderer3D->texture_2d)
			{
				CompMaterial* temp = parent->GetComponentMaterial();
				if(temp != nullptr)
					glBindTexture(GL_TEXTURE_2D, temp->GetTextureID());
			}

			glBindBuffer(GL_ARRAY_BUFFER, resourceMesh->vertices_id); //VERTEX ID
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex), NULL);
			glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, norm));
			glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resourceMesh->indices_id); // INDICES ID
			glDrawElements(GL_TRIANGLES, resourceMesh->indices.size(), GL_UNSIGNED_INT, NULL);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			// NORMALS ----------------------------------
			if (App->renderer3D->normals && hasNormals)
			{
				glBindBuffer(GL_ARRAY_BUFFER, resourceMesh->vertices_norm_id);
				glVertexPointer(3, GL_FLOAT, sizeof(float3), NULL);
				glDrawArrays(GL_LINES, 0, resourceMesh->vertices.size() * 2);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			//Reset TextureColor
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, 0);

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

		else
		{
			LOG("Cannot draw the mesh");
		}

		if (transform != nullptr)
		{
			glPopMatrix();
		}
	}
}

void CompMesh::Clear()
{
	resourceMesh = nullptr;
}


void CompMesh::Render(bool render)
{
	this->render = render;
}

bool CompMesh::isRendering() const
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
	if (resourceMesh != resourse_mesh)
	{
		if (resourceMesh != nullptr)
		{
			if (resourceMesh->NumGameObjectsUseMe > 0)
			{
				resourceMesh->NumGameObjectsUseMe--;
			}
		}
		resourceMesh = resourse_mesh;
		if (isImport)
		{
			// Fix Bug with Delete Import -----
			resourceMesh->NumGameObjectsUseMe = 0;
		}
		else
		{
			resourceMesh->NumGameObjectsUseMe++;
		}
	}
}

void CompMesh::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_number_with_std(object, name + "Type", C_MESH);
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	if(resourceMesh != nullptr)
	{
		if (saveScene == false)
		{
			// Save Info of Resource in Prefab (next we use this info for Reimport this prefab)
			std::string temp = std::to_string(countResources++);
			json_object_dotset_number_with_std(object, "Info.Resources.Resource " + temp + ".UUID Resource", resourceMesh->GetUUID());
			json_object_dotset_string_with_std(object, "Info.Resources.Resource " + temp + ".Name", resourceMesh->name);
		}
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", resourceMesh->GetUUID());
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
		resourceMesh = (ResourceMesh*)App->resource_manager->GetResource(resourceID);
		if (resourceMesh != nullptr)
		{
			resourceMesh->NumGameObjectsUseMe++;
			
			// LOAD MESH ----------------------------
			if (resourceMesh->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMesh->LoadResource(std::to_string(resourceMesh->GetUUID()).c_str(), resourceMesh);			
			}
			// Add bounding box ------
			parent->AddBoundingBox(resourceMesh);
		}
	}
	Enable();
}
