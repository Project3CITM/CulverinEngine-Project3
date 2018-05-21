#include "ModuleMap.h"
#include "GameObject.h"
#include "Scene.h"
#include "Application.h"
#include "Component.h"
#include "ModuleFS.h"
#include "CompTransform.h"
#include "ModuleTextures.h"
#include "CompMesh.h"
#include "ModuleResourceManager.h"
#include "JSONSerialization.h"
#include "ModuleInput.h"
#include "ModuleGUI.h"


ModuleMap::ModuleMap()
{
	name = "Map";
	Awake_enabled = true;
	Start_enabled = true;
}


ModuleMap::~ModuleMap()
{
	prefabs.clear();
	all_prefabs.clear();
}

bool ModuleMap::Init(JSON_Object* node)
{
	perf_timer.Start();
	name_map = json_object_get_string(node, "Walkable Map");
	if (name_map.size() > 0)
	{
		imported_map = App->fs->GetMainDirectory();
		imported_map += "/Maps/";
		imported_map += name_map;
		imported_map += ".mapwalk.json";
		ImportMap();

		map_created = true;
	}
	else
	{
		name_map = "New Map";
	}
	Awake_t = perf_timer.ReadMs();
	return true;
}

bool ModuleMap::Start()
{
	perf_timer.Start();
	imported_map.clear();
	icon_arrow_north = App->textures->LoadTexture("Images/UI/Arrow_north.png");
	icon_arrow_east = App->textures->LoadTexture("Images/UI/Arrow_east.png");
	icon_arrow_south = App->textures->LoadTexture("Images/UI/Arrow_south.png");
	icon_arrow_west = App->textures->LoadTexture("Images/UI/Arrow_west.png");
	icon_circle = App->textures->LoadTexture("Images/UI/Circle.png");
	Start_t = perf_timer.ReadMs();
	return true;
}

update_status ModuleMap::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate: ModuleMap", Profiler::Color::Blue);
	perf_timer.Start();
	if (imported_map.size() > 0)
	{
		ImportMap();
	}
	preUpdate_t = perf_timer.ReadMs();
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleMap::Update(float dt)
{
	perf_timer.Start();
	Update_t = perf_timer.ReadMs();
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleMap::PostUpdate(float dt)
{
	perf_timer.Start();
	postUpdate_t = perf_timer.ReadMs();
	return update_status::UPDATE_CONTINUE;
}

void ModuleMap::ShowEditorMap(bool &active)
{
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.176f, 0.176f, 0.176f, 1.0f));
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
			if (ImGui::MenuItem("Save Map", NULL, false, false))
			{
				
			}
			if (ImGui::MenuItem("Save Map as...", NULL, false, false))
			{

			}
			if (ImGui::BeginMenu("Import Map"))
			{
				if (ImGui::MenuItem("Import Walkable Map", NULL, false, false))
				{

				}
				if (ImGui::MenuItem("Import 3D Map", NULL, false, false))
				{

				}
				if (ImGui::MenuItem("Import Navigation Map", NULL, false, false))
				{

				}
				if (ImGui::MenuItem("Import Oclusion Map", NULL, false, false))
				{

				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close", NULL, !active))
			{
				active = !active;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options"))
		{
			//show_numeration
			if (ImGui::MenuItem("Show Numeration", NULL, show_numeration))
			{
				show_numeration = !show_numeration;
			}
			if (ImGui::MenuItem("Show Editable Style", NULL, show_editable_style))
			{
				show_editable_style = !show_editable_style;
			}
			if (ImGui::MenuItem("Revert Style"))
			{
				frame_padding_x = 4;
				frame_padding_y = 3;
				item_spacing_x = 8;
				item_spacing_y = 4;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::PushItemWidth(150);
	ImGui::Text("Edit the tiles. Click ID and set type of /nthe ID (Walkable / No-Walkable / ...)");
	ImGui::InputInt("width", &width_map); ImGui::SameLine();
	ImGui::InputInt("height", &height_map);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Set Name: "); ImGui::SameLine();
	char namedit[50];
	strcpy_s(namedit, 50, name_map.c_str());
	if (ImGui::InputText("##nameModel", namedit, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		name_map = std::string(namedit);
	}
	ImGui::PushItemWidth(70);
	ImGui::SameLine();
	ImGui::Text("Set Separation: "); ImGui::SameLine();
	ImGui::InputFloat("##Separation2", &size_separation);

	ImGui::PopItemWidth();
	ImGui::PushItemWidth(10);
	static std::string mode[] = { "Edit Walkable Map", "Create 3D Map (fast)", "Edit Navigation Map (IA)", "Oclusion Map" };
	static int selected_type = 0;
	static const char* current_item_2 = "Edit Walkable Map";
	ImGui::PushItemWidth(150);
	ImGui::AlignTextToFramePadding();
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
		case 3:
		{
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(450.0f);
				ImGui::TextUnformatted("Oclusion Map, this export and inport with 'Name'.mapoclusion.json");
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
			if (ImGui::Button("Delete Map"))
			{
				DeleteMap();
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset Map"))
			{
				ResetMap();
			}
			ImGui::SameLine();
			if (ImGui::Button("Complet with No-Walk"))
			{
				CompletNoWalk();
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
				case 3:
				{
					ShowOclusionMap();
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
	static std::string type_Name[] = { "Walkable", "No-Walkable", "Initial Position", "Valyrian Fire" };
	static int paint = 0;
	static const char* current_item_2 = "Walkable";
	ImGui::PushItemWidth(150);
	ImGui::AlignTextToFramePadding();
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

	if (show_editable_style)
	{
		ShowEditableStyle();
	}

	ImGui::PushItemWidth(10);
	//Frist Select Type
	ImGui::Separator();
	bool do_numeration = false;
	if (show_numeration)
	{
		do_numeration = true;
	}
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(frame_padding_x, frame_padding_y));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(item_spacing_x, item_spacing_y));
	for (int y = 0; y < height_map; y++)
	{
		if (do_numeration)
		{
			do_numeration = false;
			ShowTextWithColor(ImGuiCol_Button, 20);
			ImGui::ButtonEx("", ImVec2(19, 0), ImGuiButtonFlags_Disabled);
			ImGui::SameLine();
			ImGui::PopStyleColor();
			for (int n = 0; n < width_map; n++)
			{
				ShowTextWithColor(ImGuiCol_Button, 20);
				ImGui::ButtonEx(std::to_string(n).c_str(), ImVec2(19, 0), ImGuiButtonFlags_Disabled);
				if(n + 1 < width_map)
					ImGui::SameLine();
				ImGui::PopStyleColor();
			}
			y--;
		}
		else
		{
			if (show_numeration)
			{
				ShowTextWithColor(ImGuiCol_Button, 20);
				ImGui::ButtonEx(std::to_string(y).c_str(), ImVec2(19, 0), ImGuiButtonFlags_Disabled);
				ImGui::SameLine();
				ImGui::PopStyleColor();
			}
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
				if (ImGui::IsItemHovered() && App->gui->develop_mode)
				{
					if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
					{
						map[x][y] = paint;
					}
					if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT)
					{
						map[x][y] = -1;
					}
				}

				std::string position_tile = "x:" + std::to_string(x);
				position_tile += " - y:" + std::to_string(y);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(450.0f);
					ImGui::TextUnformatted(position_tile.c_str());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				if (ImGui::BeginPopupContextItem("Options##maptile"))
				{
					OptionsTile(x, y);
					ImGui::EndPopup();
				}
				ImGui::PopID();
			}
		}
	}
	ImGui::PopStyleVar(2);
	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Save Map"))
	{
		map_string = "";
		for (int y = 0; y < height_map; y++)
		{
			for (int x = 0; x < width_map; x++)
			{
				if (map[x][y] > IM_ARRAYSIZE(type_Name))
				{
					map[x][y] = 1;
				}
				map_string += std::to_string(map[x][y]);
			}
		}
	}
	ImGui::SameLine();
	ImGui::Text("Save your map and can use for test, don't forget export the map!");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Load Map"))
	{
		imported_map = App->fs->GetMainDirectory();
		imported_map += "/Maps/";
		imported_map += name_map;
		imported_map += ".mapwalk.json";
		ImportMap();
		map_created = true;
	}
	ImGui::SameLine();
	ImGui::Text("Load the map with the actual name use for map!");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
 	if (ImGui::Button("Export Map"))
	{
		// First complet map to export
		CompletNoWalk();
		vector_map.clear();
		for (int y = 0; y < height_map; y++)
		{
			std::string line = "";
			for (int x = 0; x < width_map; x++)
			{
				if (map[x][y] > IM_ARRAYSIZE(type_Name))
				{
					map[x][y] = 1;
				}
				line += std::to_string(map[x][y]);
			}
			vector_map.push_back(line);
		}
		App->json_seria->SaveMapWalkable(vector_map, height_map, width_map, size_separation, name_map.c_str());
	}
	ImGui::SameLine();
	ImGui::Text("Export Map with -> ('Name Map'.mapwalk.json) it saved in Assets/Maps/...");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
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
	ImGui::SameLine();
	ImGui::Text("Test...");
}

void ModuleMap::ShowCreationMap()
{
	//static bool settings = false;
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
	ImGui::Text("Select Prefabs we will use...");
	ImGui::PushItemWidth(70);
	static bool force_reload = false;
	if (ImGui::InputInt("Num Prefabs", &numPrefabs))
	{
		force_reload = true;
	}
	ImGui::Text("Press to load all prefabs:");

	if (ImGui::Button("Load / Reload Prefabs") || force_reload)
	{
		prefabs.clear();
		all_prefabs.clear();
		App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), all_prefabs, "fbx");
		App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), all_prefabs, "obj");
		App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), all_prefabs, "prefab.json");
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
			ImGui::Text("%i: ", i + 1); ImGui::SameLine();
			if (ImGui::BeginCombo("##Pref", prefabs[i].c_str()))
			{
				static ImGuiTextFilter filter;
				filter.Draw();
				for (int n = 0; n < all_prefabs.size(); n++)
				{
					if (filter.PassFilter(all_prefabs[n].c_str()))
					{
						if (ImGui::Selectable(all_prefabs[n].c_str()))
						{
							prefabs[i] = all_prefabs[n];
							ImGui::SetItemDefaultFocus();//
						}
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopStyleColor();
			ImGui::PopID();
		}
	}
	bool do_numeration = false;
	if (show_numeration)
	{
		do_numeration = true;
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

	if (show_editable_style)
	{
		ShowEditableStyle();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(frame_padding_x, frame_padding_y));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(item_spacing_x, item_spacing_y));
	ImGui::PushItemWidth(10);
	//Frist Select Type
	int total_map = width_map * height_map;
	for (int y = 0; y < height_map; y++)
	{
		if (do_numeration)
		{
			do_numeration = false;
			ShowTextWithColor(ImGuiCol_Button, 20);
			ImGui::ButtonEx("", ImVec2(19, 0), ImGuiButtonFlags_Disabled);
			ImGui::SameLine();
			ImGui::PopStyleColor();
			for (int n = 0; n < width_map; n++)
			{
				ShowTextWithColor(ImGuiCol_Button, 20);
				ImGui::ButtonEx(std::to_string(n).c_str(), ImVec2(19, 0), ImGuiButtonFlags_Disabled);
				if (n + 1 < width_map)
					ImGui::SameLine();
				ImGui::PopStyleColor();
			}
			y--;
		}
		else
		{
			if (show_numeration)
			{
				ShowTextWithColor(ImGuiCol_Button, 20);
				ImGui::ButtonEx(std::to_string(y).c_str(), ImVec2(19, 0), ImGuiButtonFlags_Disabled);
				ImGui::SameLine();
				ImGui::PopStyleColor();
			}
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
				std::string position_tile = "x:" + std::to_string(x);
				position_tile += " - y:" + std::to_string(y);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(450.0f);
					ImGui::TextUnformatted(position_tile.c_str());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				if (ImGui::BeginPopupContextItem("Options##maptile3d"))
				{
					OptionsTile(x, y);
					ImGui::EndPopup();
				}
				ImGui::PopID();
			}
		}
	}
	ImGui::EndColumns();
	ImGui::PopStyleVar(2);
	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Save Map"))
	{
		//map_string = "";
		//for (int y = 0; y < height_map; y++)
		//{
		//	for (int x = 0; x < width_map; x++)
		//	{
		//		if (map[x][y] > 2)
		//		{
		//			map[x][y] = 1;
		//		}
		//		map_string += std::to_string(map[x][y]);
		//	}
		//}
	}
	ImGui::SameLine();
	ImGui::Text("Disabled until further notice...");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Load Map"))
	{
		imported_map = App->fs->GetMainDirectory();
		imported_map += "/Maps/";
		imported_map += name_map;
		imported_map += ".map3d.json";
		ImportMap();
		map_created = true;
	}
	ImGui::SameLine();
	ImGui::Text("Load the map with the actual name use for map!");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Export Map"))
	{
		vector_map.clear();
		int map_temp[MAX_ARRAY][MAX_ARRAY];
		for (int y = 0; y < height_map; y++)
		{
			for (int x = 0; x < width_map; x++)
			{
				map_temp[x][y] = -1;
			}
		}
		for (int y = 0; y < height_map; y++)
		{
			for (int x = 0; x < width_map; x++)
			{
				map_temp[x][y] = map[x][y];
				map_temp[x][y] += 1;
			}
		}
		for (int y = 0; y < height_map; y++)
		{
			std::string line = "";
			for (int x = 0; x < width_map; x++)
			{
				line += std::to_string(map_temp[x][y]);
			}
			vector_map.push_back(line);
		}
		App->json_seria->SaveMapCreation(vector_map, prefabs, height_map, width_map, size_separation, name_map.c_str());
	}
	ImGui::SameLine();
	ImGui::Text("Export Map with -> ('Name Map'.map3d.json) it saved in Assets/Maps/...");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
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
							size_t EndName = prefabs[n].find(".prefab.json");
							if (EndName != std::string::npos)
							{
								//directory_prebaf += ".meta.json";
							}
							else
							{
								directory_prebaf += ".meta.json";
							}
							GameObject* temp = App->json_seria->GetLoadPrefab(directory_prebaf.c_str());
							CompTransform* transform = temp->GetComponentTransform();
							math::float3 pos = transform->GetPos();
							//float min_size = 0;
							//float max_size = 0;
							//GetSizePrefab(temp, min_size, max_size);
							//float t = max_size - min_size;
							pos.x += x * size_separation; pos.z += y * size_separation;
							transform->SetPos(pos);
							obj->AddChildGameObject(temp);
						}
					}
				}
			}
		}
	}
	ImGui::SameLine();
	ImGui::Text("Create a map level with the tile map you have made...");
	//if (settings)
	//{

	//}
	//else
	//{

	//}
}

void ModuleMap::ShowNavigationMap()
{
	// General BeginCombo() API, you have full control over your selection data and display type
	static std::string type_Name[] = { "North", "East", "South", "West" };
	static int paint = 0;
	static const char* current_item_2 = "North";
	ImGui::PushItemWidth(150);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Select Direction (Mode)"); ImGui::SameLine();
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

	if (show_editable_style)
	{
		ShowEditableStyle();
	}

	ImGui::PushItemWidth(10);
	//Frist Select Type
	ImGui::Separator();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(frame_padding_x, frame_padding_y));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(item_spacing_x, item_spacing_y));
	for (int y = 0; y < height_map; y++)
	{

		for (int x = 0; x < width_map; x++)
		{
			if (x > 0) ImGui::SameLine();
			ImGui::PushID(x + y * 1000);


			if (map[x][y] > -1)
			{
				ImTextureID id_direction = 0;
				if (map[x][y] == 0)
				{
					id_direction = (ImTextureID*)icon_arrow_north;
				}
				else if (map[x][y] == 1)
				{
					id_direction = (ImTextureID*)icon_arrow_east;
				}
				else if (map[x][y] == 2)
				{
					id_direction = (ImTextureID*)icon_arrow_south;
				}
				else if (map[x][y] == 3)
				{
					id_direction = (ImTextureID*)icon_arrow_west;
				}
				else
					id_direction = (ImTextureID*)icon_circle;
				if (ImGui::ImageButton(id_direction, ImVec2(11, 15), ImVec2(-1, 1), ImVec2(0, 0)))
				{
					map[x][y] = paint;
				}
			}
			else
			{
				if (ImGui::Button("ID"))
				{
					map[x][y] = paint;
				}
			}


			ImGui::PopID();
		}
	}
	ImGui::PopStyleVar(2);
	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Save Map"))
	{
		map_string = "";
		for (int y = 0; y < height_map; y++)
		{
			for (int x = 0; x < width_map; x++)
			{
				if (map[x][y] > 2)
				{
					map[x][y] = 1;
				}
				map_string += std::to_string(map[x][y]);
			}
		}
	}
	ImGui::SameLine();
	ImGui::Text("Save your map and can use for test, don't forget export the map!");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Load Map"))
	{
		imported_map = App->fs->GetMainDirectory();
		imported_map += "/Maps/";
		imported_map += name_map;
		imported_map += ".mapwalk.json";
		ImportMap();
		map_created = true;
	}
	ImGui::SameLine();
	ImGui::Text("Load the map with the actual name use for map!");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Export Map"))
	{
		// First complet map to export
		CompletNoWalk();
		vector_map.clear();
		for (int y = 0; y < height_map; y++)
		{
			std::string line = "";
			for (int x = 0; x < width_map; x++)
			{
				if (map[x][y] > 2)
				{
					map[x][y] = 1;
				}
				line += std::to_string(map[x][y]);
			}
			vector_map.push_back(line);
		}
		App->json_seria->SaveMapWalkable(vector_map, height_map, width_map, size_separation, name_map.c_str());
	}
	ImGui::SameLine();
	ImGui::Text("Export Map with -> ('Name Map'.mapwalk.json) it saved in Assets/Maps/...");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
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
	ImGui::SameLine();
	ImGui::Text("Test...");

}

void ModuleMap::ShowOclusionMap()
{
	// General BeginCombo() API, you have full control over your selection data and display type
	static std::string type_Name[] = { "Oclusion", "Empty", "Visibled" };
	static int paint = 0;
	static const char* current_item_2 = "Oclusion";
	ImGui::PushItemWidth(150);
	ImGui::AlignTextToFramePadding();
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

	if (show_editable_style)
	{
		ShowEditableStyle();
	}

	ImGui::PushItemWidth(10);
	//Frist Select Type
	ImGui::Separator();
	bool do_numeration = false;
	if (show_numeration)
	{
		do_numeration = true;
	}
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(frame_padding_x, frame_padding_y));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(item_spacing_x, item_spacing_y));
	for (int y = 0; y < height_map; y++)
	{
		if (do_numeration)
		{
			do_numeration = false;
			ShowTextWithColor(ImGuiCol_Button, 20);
			ImGui::ButtonEx("", ImVec2(19, 0), ImGuiButtonFlags_Disabled);
			ImGui::SameLine();
			ImGui::PopStyleColor();
			for (int n = 0; n < width_map; n++)
			{
				ShowTextWithColor(ImGuiCol_Button, 20);
				ImGui::ButtonEx(std::to_string(n).c_str(), ImVec2(19, 0), ImGuiButtonFlags_Disabled);
				if (n + 1 < width_map)
					ImGui::SameLine();
				ImGui::PopStyleColor();
			}
			y--;
		}
		else
		{
			if (show_numeration)
			{
				ShowTextWithColor(ImGuiCol_Button, 20);
				ImGui::ButtonEx(std::to_string(y).c_str(), ImVec2(19, 0), ImGuiButtonFlags_Disabled);
				ImGui::SameLine();
				ImGui::PopStyleColor();
			}
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
				if (ImGui::IsItemHovered() && App->gui->develop_mode)
				{
					if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
					{
						map[x][y] = paint;
					}
					if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT)
					{
						map[x][y] = -1;
					}
				}

				std::string position_tile = "x:" + std::to_string(x);
				position_tile += " - y:" + std::to_string(y);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(450.0f);
					ImGui::TextUnformatted(position_tile.c_str());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				if (ImGui::BeginPopupContextItem("Options##maptile"))
				{
					OptionsTile(x, y);
					ImGui::EndPopup();
				}
				ImGui::PopID();
			}
		}
	}
	ImGui::PopStyleVar(2);
	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Save Map"))
	{
		//map_string = "";
		//for (int y = 0; y < height_map; y++)
		//{
		//	for (int x = 0; x < width_map; x++)
		//	{
		//		if (map[x][y] > IM_ARRAYSIZE(type_Name))
		//		{
		//			map[x][y] = 1;
		//		}
		//		map_string += std::to_string(map[x][y]);
		//	}
		//}
	}
	ImGui::SameLine();
	ImGui::Text("Save your map and can use for test, don't forget export the map!");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Load Map"))
	{
		//imported_map = App->fs->GetMainDirectory();
		//imported_map += "/Maps/";
		//imported_map += name_map;
		//imported_map += ".mapwalk.json";
		//ImportMap();
		//map_created = true;
	}
	ImGui::SameLine();
	ImGui::Text("Load the map with the actual name use for map!");
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	if (ImGui::Button("Export Map"))
	{
		// First complet map to export
		CompletNoWalk();
		vector_map.clear();
		for (int y = 0; y < height_map; y++)
		{
			std::string line = "";
			for (int x = 0; x < width_map; x++)
			{
				line += std::to_string(map[x][y]);
			}
			vector_map.push_back(line);
		}
		App->json_seria->SaveMapOclusion(vector_map, height_map, width_map, size_separation, name_map.c_str());
	}
	ImGui::SameLine();
	ImGui::Text("Export Map with -> ('Name Map'.mapoclusion.json) it saved in Assets/Maps/...");
}

void ModuleMap::OptionsTile(int x, int y)
{
	if (ImGui::MenuItem("Delete"))
	{
		map[x][y] = -1;
	}
}

void ModuleMap::GetSizePrefab(GameObject* obj, float& min_size, float& max_size)
{
	if (obj->GetComponentMesh() != nullptr)
	{
		if (min_size > obj->box_fixed.MinX())
		{
			min_size = obj->box_fixed.MinX();
		}
		if (max_size < obj->box_fixed.MaxX())
		{
			max_size = obj->box_fixed.MaxX();
		}
	}
	for (int i = 0; i < obj->GetNumChilds(); i++)
	{
		GetSizePrefab(obj->GetChildbyIndex(i), min_size, max_size);
	}
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
	else if (id == 9) // Grey
	{
		ImGui::PushStyleColor(type, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	}
	else if (id == 10) // Same Background
	{
		ImGui::PushStyleColor(type, ImVec4(0.59f, 0.91f, 0.94f, 1.0f));
	}
	else if (id == 11) // Same Background
	{
		ImGui::PushStyleColor(type, ImVec4(0.36f, 0.63f, 0.65f, 1.0f));
	}
	else if (id == 12) // Same Background
	{
		ImGui::PushStyleColor(type, ImVec4(0.47f, 0.82f, 0.63f, 1.0f));
	}
	else if (id == 13) // Same Background
	{
		ImGui::PushStyleColor(type, ImVec4(0.36f, 0.58f, 0.47f, 1.0f));
	}
	else if (id == 14) // Same Background
	{
		ImGui::PushStyleColor(type, ImVec4(0.74f, 0.94f, 0.27f, 1.0f));
	}
	else if (id == 15) // Same Background
	{
		ImGui::PushStyleColor(type, ImVec4(0.49f, 0.54f, 0.3f, 1.0f));
	}
	else if (id == 16) // Same Background
	{
		ImGui::PushStyleColor(type, ImVec4(0.55f, 0.45f, 0.3f, 1.0f));
	}
	else if (id >= 17 && id != 20) // ...
	{
		ImGui::PushStyleColor(type, ImVec4(0.2f, 0.81f, 0.31f, 1.0f));
	}
	else if (id == 20) // Same Background
	{
		ImGui::PushStyleColor(type, ImVec4(0.136f, 0.136f, 0.136f, 1.0f));
	}
}

void ModuleMap::DeleteMap()
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

void ModuleMap::ResetMap()
{
	for (int y = 0; y < MAX_ARRAY; y++)
	{
		for (int x = 0; x < MAX_ARRAY; x++)
		{
			map[x][y] = -1;
		}
	}
}

void ModuleMap::CompletNoWalk()
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

void ModuleMap::ShowEditableStyle()
{
	ImGui::PushItemWidth(20);
	ImGui::AlignTextToFramePadding();
	ImGui::Bullet();
	ImGui::Text("Edit Frame Padding: "); ImGui::SameLine(); ImGui::Text("x:"); ImGui::SameLine();
	App->ShowHelpMarker("Disabled until further notice."); 	ImGui::SameLine();
	ImGui::InputInt("##frame_padding_x", &frame_padding_x, 0, 100, ImGuiInputTextFlags_ReadOnly);  ImGui::SameLine(); ImGui::Text("y:"); ImGui::SameLine();
	ImGui::PushItemWidth(70);
	ImGui::InputInt("##frame_padding_y", &frame_padding_y);
	ImGui::Bullet();
	ImGui::Text("Edit Item Spacing: "); ImGui::SameLine(); ImGui::Text("x:"); ImGui::SameLine();
	ImGui::InputInt("##item_spacing_x", &item_spacing_x); ImGui::SameLine(); ImGui::Text("y:"); ImGui::SameLine();
	ImGui::InputInt("##item_spacing_y", &item_spacing_y);
}

void ModuleMap::ImportMap(bool used_in_mono, TypeMap type_map)
{
	if (used_in_mono && type_map == TypeMap::MAP_WALKABLE)
	{
		std::string import_map_tmp;
		import_map_tmp = App->fs->GetMainDirectory();
		import_map_tmp += "/Maps/";
		import_map_tmp += imported_map;
		imported_map = import_map_tmp;
		imported_map += ".mapwalk.json";
	}
	else if (used_in_mono && type_map == TypeMap::MAP_OCLUSION)
	{
		std::string import_map_tmp;
		import_map_tmp = App->fs->GetMainDirectory();
		import_map_tmp += "/Maps/";
		import_map_tmp += imported_map;
		imported_map = import_map_tmp;
		imported_map += ".mapoclusion.json";
	}

	TypeMap type = App->map->CheckTypeMap(imported_map.c_str());
	vector_map.clear();
	for (int y = 0; y < 99; y++)
	{
		for (int x = 0; x < 99; x++)
		{
			map[x][y] = -1;
		}
	}
	switch (type)
	{
	case TypeMap::MAP_WALKABLE:
	{
		if (App->json_seria->LoadMapWalkable(vector_map, height_map, width_map, size_separation, imported_map.c_str(), name_map))
		{
			for (int y = 0; y < height_map; y++)
			{
				std::string line = vector_map[y];
				for (int x = 0; x < width_map; x++)
				{
					std::string number;
					number += line[x];
					map[x][y] = atoi(number.c_str());
					//t += 1;
				}
			}
			map_string = "";
			for (int y = 0; y < height_map; y++)
			{
				for (int x = 0; x < width_map; x++)
				{
					if (map[x][y] > 3)
					{
						map[x][y] = 1;
					}
					map_string += std::to_string(map[x][y]);
				}
			}
			LOG("Walkable Map Loaded -----------");
		}
		else
		{
			LOG("[error] Faild with loading walkable map -----------");
		}
		break;
	}
	case TypeMap::MAP_3D:
	{
		prefabs.clear();
		if (App->json_seria->LoadMapCreation(vector_map, prefabs, height_map, width_map, size_separation, numPrefabs, imported_map.c_str(), name_map))
		{
			all_prefabs.clear();
			App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), all_prefabs, "fbx");
			App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), all_prefabs, "obj");
			App->fs->GetAllFilesByExtension(App->fs->GetMainDirectory(), all_prefabs, "prefab.json");
			App->fs->FixNames_directories(all_prefabs);
			for (int y = 0; y < height_map; y++)
			{
				std::string line = vector_map[y];
				for (int x = 0; x < width_map; x++)
				{
					std::string number;
					number += line[x];
					map[x][y] = atoi(number.c_str())-1;
					//t += 1;
				}
			}
			go_select_prefab = true;
			LOG("Walkable Map Loaded -----------");
		}
		break;
	}
	case TypeMap::MAP_NAVIGATION:
	{

		break;
	}
	case TypeMap::MAP_OCLUSION:
	{
		int tem_height, tem_width;
		if (App->json_seria->LoadMapOclusion(vector_map, tem_height, tem_width, size_separation, imported_map.c_str(), name_map))
		{
			for (int y = 0; y < tem_height; y++)
			{
				std::string line = vector_map[y];
				for (int x = 0; x < tem_width; x++)
				{
					std::string number;
					number += line[x];
					map[x][y] = atoi(number.c_str());
					//t += 1;
				}
			}
			for (int y = 0; y < tem_height; y++)
			{
				for (int x = 0; x < tem_width; x++)
				{
					if (map[x][y] > 3)
					{
						map[x][y] = 1;
					}
				}
			}

			App->scene->oclusion_culling->LoadOclusionMap(map, width_map, height_map);

			LOG("Walkable Map Loaded -----------");
		}
		else
		{
			LOG("[error] Faild with loading walkable map -----------");
		}
		break;
	}
	}
	imported_map.clear();
}

TypeMap ModuleMap::CheckTypeMap(const char* map)
{
	std::string file = map;
	std::string extension = map;

	size_t extension_pos = file.find_last_of(".");

	extension = file.substr(extension_pos + 1);
	std::string extension_map = file.substr(0, extension_pos);;
	if (extension == "json")
	{
		extension_pos = extension_map.find_last_of(".");
		extension_map = extension_map.substr(extension_pos + 1);
		if (extension_map == "mapwalk")
		{
			return TypeMap::MAP_WALKABLE;
		}
		else if (extension_map == "map3d")
		{
			return TypeMap::MAP_3D;
		}
		else if (extension_map == "mapnavi")
		{
			return TypeMap::MAP_NAVIGATION;
		}
		else if (extension_map == "mapoclusion")
		{
			return TypeMap::MAP_OCLUSION;
		}
	}
	else
	{
		return TypeMap::MAP_NON;
	}
	return TypeMap::MAP_NON;
}

int ModuleMap::GetHeightMap()
{
	return height_map;
}

int ModuleMap::GetWidthMap()
{
	return width_map;
}

float ModuleMap::GetSeparation()
{
	return size_separation;
}

bool ModuleMap::SaveConfig(JSON_Object* node)
{
	//Save --------------------------------
	if (App->mode_game == false)
	{
		json_object_set_string(node, "Walkable Map", name_map.c_str());
	}

	// Navigation maps
	//json_object_set_string(node, "Walkable Map", map_string.c_str());
	//json_object_set_string(node, "Walkable Map", map_string.c_str());
	//json_object_set_string(node, "Walkable Map", map_string.c_str());
	// ------------------------------------------------------
	return true;
}
