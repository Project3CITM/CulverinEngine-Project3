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
	name_map = "New Map";
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
	if (!ImGui::Begin("Edit Map Tile", &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
	}
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Map"))
			{
				for (int y = 0; y < MAX_ARRAY; y++)
				{
					for (int x = 0; x < MAX_ARRAY; x++)
					{
						map[x][y] = -1;
					}
				}
				map_created = false;
			}
			if (ImGui::MenuItem("Open Map", NULL, false, false))
			{

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save Scene"))
			{
				
			}
			if (ImGui::MenuItem("Save Scene as...", NULL, false, false))
			{

			}
			if (ImGui::MenuItem("Load Map"))
			{
				
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
			{
				active = !active;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Close"))
		{
			active = !active;
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::PushItemWidth(70);
	ImGui::Text("Edit the tiles. Click ID and set type of /nthe ID (Walkable / No-Walkable / ...)");
	ImGui::InputInt("width", &width_map); ImGui::SameLine();
	ImGui::InputInt("height", &height_map);
	ImGui::Text("Set Name: "); ImGui::SameLine();
	char namedit[50];
	strcpy_s(namedit, 50, name_map);
	if (ImGui::InputText("##nameModel", namedit, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		name_map = App->fs->ConverttoChar(std::string(namedit).c_str());
	}

	ImGui::PopItemWidth();
	ImGui::PushItemWidth(10);
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
				ImGui::TextUnformatted("Edit Walkable Map, this export and inport with 'Name'.mapwalk.json");
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
				ImGui::TextUnformatted("Create 3D Map (fast), this export and inport with 'Name'.map3d.json");
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
				ImGui::TextUnformatted("Edit Navigation Map (IA), this export and inport with 'Name'.mapnavi.json");
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
				for (int y = 0; y < MAX_ARRAY; y++)
				{
					for (int x = 0; x < MAX_ARRAY; x++)
					{
						map[x][y] = -1;
					}
				}
				map_created = true;
			}
		}
		if (map_created)
		{
			if (ImGui::Button("Reset Map"))
			{
				for (int y = 0; y < MAX_ARRAY; y++)
				{
					for (int x = 0; x < MAX_ARRAY; x++)
					{
						map[x][y] = -1;
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Map"))
			{
				for (int y = 0; y < MAX_ARRAY; y++)
				{
					for (int x = 0; x < MAX_ARRAY; x++)
					{
						map[x][y] = -1;
					}
				}
				map_created = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Complet with No-Walk"))
			{
				for (int y = 0; y < height_map; y++)
				{
					for (int x = 0; x < width_map; x++)
					{
						if (map[x][y] == -1)
						{
							map[x][y] = 1;
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
	for (int y = 0; y < height_map; y++)
	{
		for (int x = 0; x < width_map; x++)
		{
			if (x > 0) ImGui::SameLine();
			ImGui::PushID(x + y * 1000);
			if (map[x][y] > -1)
			{
				ShowTextWithColor(ImGuiCol_Button, map[x][y]);
			}
			//	ImGui::OpenPopup("ID");
			bool force_pop = false;
			if (map[x][y] > -1)
			{
				force_pop = true;
			}
			if (ImGui::Button("ID"))
			{
				map[x][y] = paint;
			}

			if (map[x][y] > -1 && force_pop)
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
		for (int y = 0; y < height_map; y++)
		{
			for (int x = 0; x < width_map; x++)
			{
				map_string += std::to_string(map[x][y]);
			}
		}
	}
 	if (ImGui::Button("Export Map ('Name'.map.json)"))
	{
		for (int y = 0; y < height_map; y++)
		{
			for (int x = 0; x < width_map; x++)
			{
				if (map[x][y] == -1)
				{
					map[x][y] == 1;
				}
			}
		}
		for (int y = 0; y < height_map; y++)
		{
			std::string line = "";
			for (int x = 0; x < width_map; x++)
			{
				line += std::to_string(map[x][y]);
			}
			export_map.push_back(line);
		}
		App->json_seria->SaveMapWalkable(export_map, height_map, width_map, name_map);
	}
	if (ImGui::Button("Create Level Map"))
	{
		//map_string = "";
		for (int y = 0; y < height_map; y++)
		{
			for (int x = 0; x < width_map; x++)
			{
				if (map[x][y] > -1)
				{
					if (map[x][y] == 0)
					{
						GameObject* obj = App->scene->CreateCube();
						CompTransform* transform = obj->GetComponentTransform();
						math::float3 pos = transform->GetPos();
						pos.x += x * 2; pos.z += y * 2;
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
	//static bool settings = false;
	static int numPrefabs = 0;
	//if (ImGui::Button("Open Settings"))
	//{
	//	settings = true;
	//	//GetAllFilesByExtension
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Close Settings"))
	//{
	//	settings = false;
	//	//GetAllFilesByExtension
	//}
	ImGui::Separator();
	ImGui::BeginColumns("Setting_map", 2, ImGuiColumnsFlags_GrowParentContentsSize);
	ImGui::SetColumnWidth(0, 180);
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

	ImGui::NextColumn();
	ImGui::PushItemWidth(10);
	//ImGui::Separator();
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
	int total_map = width_map * height_map;
	for (int y = 0; y < height_map; y++)
	{
		for (int x = 0; x < width_map; x++)
		{
			if (x > 0) ImGui::SameLine();
			ImGui::PushID(x + y * 1000);
			if (map[x][y] > -1)
			{
				ShowTextWithColor(ImGuiCol_Button, map[x][y]);
				total_map--;
			}
			//	ImGui::OpenPopup("ID");
			bool force_pop = false;
			if (map[x][y] > -1)
			{
				force_pop = true;
			}
			if (ImGui::Button("ID"))
			{
				map[x][y] = paint;
			}

			if (map[x][y] > -1 && force_pop)
			{
				ImGui::PopStyleColor();
			}
			ImGui::PopID();
		}
	}
	ImGui::EndColumns();
	ImGui::Separator();
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
		for (int y = 0; y < height_map; y++)
		{
			for (int x = 0; x < width_map; x++)
			{
				if (map[x][y] > -1)
				{
					for (int n = 0; n < prefabs.size(); n++)
					{
						if (map[x][y] == n)
						{
							std::string directory_prebaf = App->fs->GetMainDirectory();
							directory_prebaf += "/";
							directory_prebaf += prefabs[n];
							directory_prebaf += ".meta.json";
							GameObject* temp = App->json_seria->GetLoadPrefab(directory_prebaf.c_str());
							CompTransform* transform = temp->GetComponentTransform();
							math::float3 pos = transform->GetPos();
							float min_size = 0;
							float max_size = 0;
							GetSizePrefab(temp, min_size, max_size);
							float t = max_size - min_size;
							pos.x += x * t; pos.z += y * t;
							transform->SetPos(pos);
							obj->AddChildGameObject(temp);
						}
					}
				}
			}
		}
	}
	//if (settings)
	//{

	//}
	//else
	//{

	//}
}

void ModuleMap::GetSizePrefab(GameObject* obj, float& min_size, float& max_size)
{
	if (obj->GetComponentMesh() != nullptr)
	{
		if (min_size > obj->bounding_box->MinX())
		{
			min_size = obj->bounding_box->MinX();
		}
		if (max_size < obj->bounding_box->MaxX())
		{
			max_size = obj->bounding_box->MaxX();
		}
	}
	for (int i = 0; i < obj->GetNumChilds(); i++)
	{
		GetSizePrefab(obj->GetChildbyIndex(i), min_size, max_size);
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
