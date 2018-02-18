#include "ModuleMap.h"
#include "GameObject.h"
#include "Scene.h"
#include "Application.h"
#include "Component.h"
#include "ModuleFS.h"
#include "CompTransform.h"
#include "CompMesh.h"
#include "ModuleResourceManager.h"
#include "JSONSerialization.h"


ModuleMap::ModuleMap()
{
	name = "Map";
}


ModuleMap::~ModuleMap()
{
	prefabs.clear();
	all_prefabs.clear();
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
	ImGui::PushItemWidth(70);
	ImGui::Text("Edit the tiles. Click ID and set type of /nthe ID (Walkable / No-Walkable / ...)");
	ImGui::InputInt("width", &width_map); ImGui::SameLine();
	ImGui::InputInt("height", &height_map);

	ImGui::PopItemWidth();
	ImGui::PushItemWidth(10);
	static bool map_created = false;
	static std::string mode[] = { "Edit Walkable Map", "Create 3D Map (fast)", "Edit Navigation Map (IA)" };
	static int selected_type = -1;
	static const char* current_item_2 = NULL;
	ImGui::PushItemWidth(150);
	ImGui::Text("Select Map Mode: "); ImGui::SameLine();
	if (ImGui::BeginCombo("##Mode", current_item_2))
	{
		for (int n = 0; n < IM_ARRAYSIZE(mode); n++)
		{
			if (ImGui::Selectable(mode[n].c_str()))
			{
				selected_type = n;
				current_item_2 = mode[n].c_str();
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PushItemWidth(10);
	if (selected_type != -1)
	{
		ImGui::SameLine();
		switch (selected_type)
		{
		case 0:
		{
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(450.0f);
				ImGui::TextUnformatted("Edit Walkable Map");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			break;
		}
		case 1:
		{
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(450.0f);
				ImGui::TextUnformatted("Create 3D Map (fast)");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			break;
		}
		case 2:
		{
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(450.0f);
				ImGui::TextUnformatted("Edit Navigation Map (IA)");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			break;
		}
		}
	}
	if (ImGui::BeginPopup("select"))
	{
		for (int i = 0; i < IM_ARRAYSIZE(mode); i++)
			if (ImGui::Selectable(mode[i].c_str()))
			{
				selected_type = i;
			}
		ImGui::EndPopup();
	}
	if (selected_type != -1)
	{
		if (map_created == false)
		{
			//ImGui::Separator();
			if (ImGui::Button("Create Map Tiled"))
			{
				for (int i = 0; i < MAX_ARRAY; i++)
				{
					for (int j = 0; j < MAX_ARRAY; j++)
					{
						map[i][j] = -1;
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
				for (int i = 0; i < width_map; i++)
				{
					for (int j = 0; j < height_map; j++)
					{
						if (map[i][j] == -1)
						{
							map[i][j] = 1;
						}
					}
				}
			}
			if (selected_type != -1)
			{
				switch (selected_type)
				{
				case 0:
				{
					ShowWalkableMap();
					break;
				}
				case 1:
				{
					ShowCreationMap();
					break;
				}
				case 2:
				{
					ShowNavigationMap();
					break;
				}
				}
			}
		}
	}
	ImGui::PopItemWidth();
	ImGui::End();
}

void ModuleMap::ShowWalkableMap()
{
	// General BeginCombo() API, you have full control over your selection data and display type
	static std::string type_Name[] = { "Walkable", "No-Walkable", "Initial Position" };
	static int paint = -1;
	static const char* current_item_2 = NULL;
	ImGui::PushItemWidth(150);
	ImGui::Text("Select Color (Mode)"); ImGui::SameLine();
	ShowTextWithColor(ImGuiCol_Text, paint);
	bool do_pop = false;
	if (paint > -1)
		do_pop = true;
	if (ImGui::BeginCombo("##TypeWalkable", current_item_2))
	{
		for (int n = 0; n < IM_ARRAYSIZE(type_Name); n++)
		{
			ShowTextWithColor(ImGuiCol_Text, n);
			if (ImGui::Selectable(type_Name[n].c_str()))
			{
				paint = n;
				current_item_2 = type_Name[n].c_str();
				ImGui::SetItemDefaultFocus();
			}
			ImGui::PopStyleColor();
		}
		ImGui::EndCombo();
	}
	if (paint > -1 && do_pop)
		ImGui::PopStyleColor();
	ImGui::PushItemWidth(10);
	//Frist Select Type
	ImGui::Separator();
	for (int i = 0; i < width_map; i++)
	{
		for (int j = 0; j < height_map; j++)
		{
			if (j > 0) ImGui::SameLine();
			ImGui::PushID(j + i * 1000);
			if (map[i][j] > -1)
			{
				ShowTextWithColor(ImGuiCol_Button, map[i][j]);
			}
			//	ImGui::OpenPopup("ID");
			bool force_pop = false;
			if (map[i][j] > -1)
			{
				force_pop = true;
			}
			if (ImGui::Button("ID"))
			{
				map[i][j] = paint;
			}

			if (map[i][j] > -1 && force_pop)
			{
				ImGui::PopStyleColor();
			}
			ImGui::PopID();
		}
	}
	ImGui::Separator();
	if (ImGui::Button("Save Map"))
	{
		map_string = "";
		for (int i = 0; i < width_map; i++)
		{
			for (int j = 0; j < height_map; j++)
			{
				map_string += std::to_string(map[i][j]);
			}
		}
	}
	if (ImGui::Button("Export Map ('Name'.map.json)"))
	{
		
	}
	if (ImGui::Button("Create Level Map"))
	{
		//map_string = "";
		for (int i = 0; i < width_map; i++)
		{
			for (int j = 0; j < height_map; j++)
			{
				if (map[i][j] > -1)
				{
					if (map[i][j] == 0)
					{
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

void ModuleMap::ShowCreationMap()
{
	static bool settings = false;
	static int numPrefabs = 0;
	if (ImGui::Button("Open Settings"))
	{
		settings = true;
		//GetAllFilesByExtension
	}
	ImGui::SameLine();
	if (ImGui::Button("Close Settings"))
	{
		settings = false;
		//GetAllFilesByExtension
	}
	if (settings)
	{
		ImGui::Text("Select number of Prefabs we will use...");
		ImGui::PushItemWidth(70);
		static bool force_reload = false;
		if (ImGui::InputInt("Num Prefabs", &numPrefabs))
		{
			force_reload = true;
		}
		ImGui::Text("Press to load all prefabs:");
		static bool go_select_prefab = false;
		if (ImGui::Button("Load / Reload Prefabs") || force_reload)
		{
			prefabs.clear();
			all_prefabs.clear();
			App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), all_prefabs, "fbx");
			App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), all_prefabs, "obj");
			App->fs->FixNames_directories(all_prefabs);
			if (numPrefabs > all_prefabs.size())
				numPrefabs = all_prefabs.size();
			go_select_prefab = true;
			force_reload = false;
			for (int i = 0; i < numPrefabs; i++)
			{
				prefabs.push_back("");
			}
		}
		if (numPrefabs > 0 && go_select_prefab && numPrefabs == prefabs.size())
		{
			ImGui::PushItemWidth(150);
			for (int i = 0; i < numPrefabs; i++)
			{
				ImGui::PushID(i * 10);
				ShowTextWithColor(ImGuiCol_Text, i);
				ImGui::Text("%i: ", i); ImGui::SameLine();
				if (ImGui::BeginCombo("##Pref", prefabs[i].c_str()))
				{
					for (int n = 0; n < all_prefabs.size(); n++)
					{
						if (ImGui::Selectable(all_prefabs[n].c_str()))
						{
							prefabs[i] = all_prefabs[n];
							ImGui::SetItemDefaultFocus();//
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopStyleColor();
				ImGui::PopID();
			}
		}
		ImGui::PushItemWidth(10);
	}
	else
	{
		ImGui::Separator();
		static int paint = -1;
		static const char* current_item_2 = NULL;
		ImGui::PushItemWidth(150);
		ImGui::Text("Select Color (Mode)"); ImGui::SameLine();
		ShowTextWithColor(ImGuiCol_Text, paint);
		bool do_pop = false;
		if (paint > -1)
			do_pop = true;
		if (ImGui::BeginCombo("##TypeCreateMap", current_item_2))
		{
			for (int i = 0; i < numPrefabs; i++)
			{
				ShowTextWithColor(ImGuiCol_Text, i);
				if (ImGui::Selectable(prefabs[i].c_str()))
				{
					paint = i;
					current_item_2 = prefabs[i].c_str();
					ImGui::SetItemDefaultFocus();
				}
				ImGui::PopStyleColor();
			}
			ImGui::EndCombo();
		}
		if (paint > -1 && do_pop)
			ImGui::PopStyleColor();
		ImGui::PushItemWidth(10);
		//Frist Select Type
		ImGui::Separator();
		int total_map = width_map * height_map;
		for (int i = 0; i < width_map; i++)
		{
			for (int j = 0; j < height_map; j++)
			{
				if (j > 0) ImGui::SameLine();
				ImGui::PushID(j + i * 1000);
				if (map[i][j] > -1)
				{
					ShowTextWithColor(ImGuiCol_Button, map[i][j]);
					total_map--;
				}
				//	ImGui::OpenPopup("ID");
				bool force_pop = false;
				if (map[i][j] > -1)
				{
					force_pop = true;
				}
				if (ImGui::Button("ID"))
				{
					map[i][j] = paint;
				}

				if (map[i][j] > -1 && force_pop)
				{
					ImGui::PopStyleColor();
				}
				ImGui::PopID();
			}
		}
		if (ImGui::Button("Save Map"))
		{

		} 
		ImGui::SameLine();
		if (ImGui::Button("Export Map ('Name'.map.json)"))
		{

		} 
		ImGui::SameLine();
		if (ImGui::Button("Create Level Map"))
		{
			GameObject* obj = App->scene->CreateGameObject();
			obj->SetName("New Map");
			for (int i = 0; i < width_map; i++)
			{
				for (int j = 0; j < height_map; j++)
				{
					if (map[i][j] > -1)
					{
						for (int n = 0; n < prefabs.size(); n++)
						{
							if (map[i][j] == n)
							{
								std::string directory_prebaf = App->fs->GetMainDirectory();
								directory_prebaf += "/";
								directory_prebaf += prefabs[n];
								directory_prebaf += ".meta.json";
								GameObject* temp = App->json_seria->GetLoadPrefab(directory_prebaf.c_str());
								CompTransform* transform = temp->GetComponentTransform();
								math::float3 pos = transform->GetPos();
								float t = temp->bounding_box->MaxX();
								pos.x += i * 1; pos.z += j * 1;
								transform->SetPos(pos);
								obj->AddChildGameObject(temp);
							}
						}
					}
				}
			}
		}
	}
}

void ModuleMap::ShowNavigationMap()
{
}

void ModuleMap::ShowTextWithColor(ImGuiCol_ type,int id)
{
	if (id == 0) // Green
	{
		ImGui::PushStyleColor(type, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	else if (id == 1) // Red
	{
		ImGui::PushStyleColor(type, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else if (id == 2) // Yellow
	{
		ImGui::PushStyleColor(type, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	}
	else if (id == 3) // Blue
	{
		ImGui::PushStyleColor(type, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
	}
	else if (id == 4) // Pink
	{
		ImGui::PushStyleColor(type, ImVec4(1.0f, 0.0f, 1.0f, 1.0f));
	}
	else if (id == 5) // Purpule
	{
		ImGui::PushStyleColor(type, ImVec4(0.5f, 0.0f, 1.0f, 1.0f));
	}
	else if (id == 6) // Sky
	{
		ImGui::PushStyleColor(type, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
	}
	else if (id == 7) // Orange
	{
		ImGui::PushStyleColor(type, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
	}
	else if (id == 8) // White
	{
		ImGui::PushStyleColor(type, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

int ModuleMap::GetHeightMap()
{
	return height_map;
}

int ModuleMap::GetWidthMap()
{
	return width_map;
}
