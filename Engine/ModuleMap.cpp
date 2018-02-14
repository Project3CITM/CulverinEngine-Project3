#include "ModuleMap.h"
#include "GameObject.h"
#include "Scene.h"
#include "Application.h"
#include "Component.h"
#include "CompTransform.h"
#include "CompMesh.h"
#include "ModuleResourceManager.h"


ModuleMap::ModuleMap()
{
}


ModuleMap::~ModuleMap()
{
}

bool ModuleMap::Init(JSON_Object* node)
{
	return true;
}

bool ModuleMap::Start()
{
	map_string = "Hola Joan";
	return true;
}

update_status ModuleMap::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleMap::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleMap::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

void ModuleMap::ShowEditorMap(bool &active)
{
	if (!ImGui::Begin("Edit Map Tile", &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
	}
	static int ws = 10, hs = 10;
	ImGui::PushItemWidth(70);
	ImGui::Text("Edit the tiles. Click ID and set type of /nthe ID (Walkable / No-Walkable / ...)");
	ImGui::InputInt("width", &ws); ImGui::SameLine();
	ImGui::InputInt("height", &hs);

	ImGui::PopItemWidth();
	ImGui::PushItemWidth(10);
	static bool map_created = false;
	if (map_created == false)
	{
		if (ImGui::Button("Create Map Tiled"))
		{
			for (int i = 0; i < MAX_ARRAY; i++)
			{
				for (int j = 0; j < MAX_ARRAY; j++)
				{
					map[i][j] = -1;
				}
			}
			for (int i = 0; i < ws; i++)
			{
				for (int j = 0; j < hs; j++)
				{
					map[i][j] = 0;
				}
			}
			map_created = true;
		}
	}
	if (map_created)
	{
		ImGui::Separator();
		if (ImGui::Button("Reset Map"))
		{
			for (int i = 0; i < MAX_ARRAY; i++)
			{
				for (int j = 0; j < MAX_ARRAY; j++)
				{
					map[i][j] = -1;
				}
			}
			for (int i = 0; i < ws; i++)
			{
				for (int j = 0; j < hs; j++)
				{
					map[i][j] = 0;
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Map"))
		{
			for (int i = 0; i < MAX_ARRAY; i++)
			{
				for (int j = 0; j < MAX_ARRAY; j++)
				{
					map[i][j] = -1;
				}
			}
			map_created = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Complet with No-Walk"))
		{
			for (int i = 0; i < ws; i++)
			{
				for (int j = 0; j < hs; j++)
				{
					if (map[i][j] == 0)
					{
						map[i][j] = 2;
					}
				}
			}
		}
		static int selected_type = -1;
		static std::string type_Name[] = { "Walkable", "No-Walkable", "Joan" };
		//Frist Select Type
		ImGui::Separator();
		int total_map = ws * hs;
		for (int i = 0; i < ws; i++)
		{
			for (int j = 0; j < hs; j++)
			{
				if (j > 0) ImGui::SameLine();
				ImGui::PushID(j + i * 1000);
				if (map[i][j] > 0)
				{
					if (map[i][j] == 1)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
					}
					else if (map[i][j] == 2)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
					}
					else if (map[i][j] == 3)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
					}
					total_map--;
				}
				if (ImGui::Button("ID"))
					ImGui::OpenPopup("ID");
				if (map[i][j] > 0)
				{
					ImGui::PopStyleColor();
				}
				if (ImGui::BeginPopup("ID"))
				{
					for (int k = 0; k < IM_ARRAYSIZE(type_Name); k++)
						if (ImGui::Selectable(type_Name[k].c_str()))
						{
							map[i][j] = k + 1;
						}
					ImGui::EndPopup();
				}
				ImGui::PopID();
			}
		}
		ImGui::Text("Finish");
		if (total_map == 0)
		{
			ImGui::SameLine();
			if (ImGui::Button("Save Map"))
			{
				map_string = "";
				for (int i = 0; i < ws; i++)
				{
					for (int j = 0; j < hs; j++)
					{
						map_string += std::to_string(map[i][j]);
					}
				}
			}
			if (ImGui::Button("Export Map ('Name'.map.json)"))
			{
				//map_string = "";
				//for (int i = 0; i < ws; i++)
				//{
				//	for (int j = 0; j < hs; j++)
				//	{
				//		map_string += std::to_string(map[i][j]);
				//	}
				//}
				// ...........
			}
			if (ImGui::Button("Create Level Map"))
			{
				//map_string = "";
				for (int i = 0; i < ws; i++)
				{
					for (int j = 0; j < hs; j++)
					{
						if (map[i][j] > 0)
						{
							if (map[i][j] == 1)
							{
								//GameObject* obj = new GameObject("");
								//CompTransform* transform = (CompTransform*)obj->AddComponent(Comp_Type::C_TRANSFORM);
								//transform->Enable();
								//math::float3 pos = transform->GetPos();
								//pos.x *= i; pos.z *= j;
								//transform->SetPos(pos);
								//CompMesh* mesh = (CompMesh*)obj->AddComponent(Comp_Type::C_MESH);
								//mesh->Enable();
								//mesh->resource_mesh = (ResourceMesh*)App->resource_manager->GetResource(2);
								GameObject* obj = App->scene->CreateCube();
								CompTransform* transform = obj->GetComponentTransform();
								math::float3 pos = transform->GetPos();
								pos.x -= i * 2; pos.z -= j * 2;
								transform->SetPos(pos);
								//App->scene->root->AddChildGameObject(obj);
							}
						}
					}
				}
				// ...........
			}
		}
	}
	ImGui::PopItemWidth();
	ImGui::End();
}
