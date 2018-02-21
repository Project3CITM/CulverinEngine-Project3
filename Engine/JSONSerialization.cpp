#include "JSONSerialization.h"
#include "Application.h"
#include "ResourceMaterial.h"
#include "ResourceScript.h"
#include "ResourceAnimation.h"
#include "ModuleFS.h"
#include "ModuleResourceManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "ModuleAudio.h"

static int malloc_count;
static void *counted_malloc(size_t size);
static void counted_free(void *ptr);

JSONSerialization::JSONSerialization()
{
}


JSONSerialization::~JSONSerialization()
{
	namesScene.clear();
}

std::string JSONSerialization::SaveScene()
{
	LOG("SAVING SCENE -----");

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	std::string nameJson = App->fs->GetMainDirectory();
	nameJson += "/";
	nameJson += App->scene->root->GetName();
	nameJson += ".scene.json";
	config_file = json_value_init_object();

	uint count = 0;
	uint countResources = 0;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_number_with_std(config, "Scene.Info.Number of GameObjects", count);
		config_node = json_object_get_object(config, "Scene");

		std::string nameScene = "Scene.Properties.";
		// UUID--------
		json_object_dotset_number_with_std(config_node, nameScene + "UUID", App->scene->root->GetUUID());
		// Name --------
		json_object_dotset_string_with_std(config_node, nameScene + "Name", App->scene->root->GetName());

		// Scene properties
		std::string name = "GameObject" + std::to_string(count);
		name += ".";

		if (App->scene->root->GetNumChilds() > 0)
		{
			for (int i = 0; i < App->scene->root->GetNumChilds(); i++)
			{
				SaveChildGameObject(config_node, *App->scene->root->GetChildbyIndex(i), count, countResources);
			}
		}

		//AudioBanks
		App->audio->SaveAudioBanks(config);


	}
	json_object_dotset_number_with_std(config_node, "Info.Number of GameObjects", count);
	json_serialize_to_file(config_file, nameJson.c_str());
	json_value_free(config_file);
	return nameJson;
}

void JSONSerialization::SaveChildGameObject(JSON_Object* config_node, const GameObject& gameObject, uint& count, uint& countResources)
{
	// Update GameObjects
	std::string name = "GameObject" + std::to_string(count++);
	name += ".";
	// UUID--------
	json_object_dotset_number_with_std(config_node, name + "UUID", gameObject.GetUUID());
	// Parent UUID------------
	int uuidParent = -1;
	if (gameObject.GetParent() != nullptr)
	{
		uuidParent = gameObject.GetParent()->GetUUID();
	}
	json_object_dotset_number_with_std(config_node, name + "Parent", uuidParent);
	// Name- --------
	json_object_dotset_string_with_std(config_node, name + "Name", gameObject.GetName());
	// Bounding Box ---------
	json_object_dotset_boolean_with_std(config_node, name + "Bounding Box", gameObject.IsAABBActive());
	// Static ---------
	json_object_dotset_boolean_with_std(config_node, name + "Static", gameObject.IsStatic());


	// Components  ------------
	std::string components = name;
	json_object_dotset_number_with_std(config_node, components + "Number of Components", gameObject.GetNumComponents());
	if (gameObject.GetNumComponents() > 0)
	{
		components += "Components.";
		gameObject.SaveComponents(config_node, components, true, countResources);
	}
	if (gameObject.GetNumChilds() > 0)
	{
		for (int i = 0; i < gameObject.GetNumChilds(); i++)
		{
			SaveChildGameObject(config_node, *gameObject.GetChildbyIndex(i), count, countResources);
		}
	}
}

void JSONSerialization::LoadScene(const char* sceneName)
{
	LOG("LOADING SCENE -----");

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file(sceneName);
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Scene");
		int NUmberGameObjects = json_object_dotget_number(config_node, "Info.Number of GameObjects");
		App->scene->root->SetUUID(json_object_dotget_number(config_node, "Scene.Properties.UUID"));
		App->scene->root->SetName(App->GetCharfromConstChar(json_object_dotget_string_with_std(config_node, "Scene.Properties.Name")));
		std::vector<LoadSceneSt> templist;
		if (NUmberGameObjects > 0)
		{
			for (int i = 0; i < NUmberGameObjects; i++)
			{
				std::string name = "GameObject" + std::to_string(i);
				name += ".";
				char* nameGameObject = App->GetCharfromConstChar(json_object_dotget_string_with_std(config_node, name + "Name"));
				uint uid = json_object_dotget_number_with_std(config_node, name + "UUID");
				GameObject* obj = new GameObject(nameGameObject, uid);
				bool static_obj = json_object_dotget_boolean_with_std(config_node, name + "Static");
				obj->SetStatic(static_obj);
				bool aabb_active = json_object_dotget_boolean_with_std(config_node, name + "Bounding Box");
				obj->SetAABBActive(aabb_active);

				//Load Components
				int NumberofComponents = json_object_dotget_number_with_std(config_node, name + "Number of Components");
				if (NumberofComponents > 0)
				{
					obj->LoadComponents(config_node, name + "Components.", NumberofComponents);
				}
				int uuid_parent = json_object_dotget_number_with_std(config_node, name + "Parent");

				LoadSceneSt temp;
				temp.go = obj;
				temp.uid_parent = uuid_parent;
				templist.push_back(temp);
			}
		}
	
		// Now with uid parent add childs.
		for (int i = 0; i < templist.size(); i++)
		{
			if (templist[i].uid_parent != -1)
			{
				for (int j = 0; j < templist.size(); j++)
				{
					if (templist[i].uid_parent == templist[j].go->GetUUID())
					{
						templist[j].go->AddChildGameObject(templist[i].go);
					}
				}
			}
		}
		//Sync components
		for (int i = 0; i < templist.size(); i++)
		{
			templist[i].go->SyncComponents();
		}
		// Now pass vector to root in scene
		for (int i = 0; i < templist.size(); i++)
		{
			if (templist[i].uid_parent == -1)
			{
				App->scene->root->AddChildGameObject(templist[i].go);
			}
		}
		templist.clear();


		//Load Audio Banks
		int number_of_audio_banks = json_object_dotget_number(config_node, "Info.Number of Audio Banks");
		App->audio->LoadAudioBanksFromScene(number_of_audio_banks, config);
	}
	json_value_free(config_file);
}


void JSONSerialization::SavePrefab(const GameObject& gameObject, const char* directory, const char* fileName)
{
	LOG("SAVING PREFAB %s -----", gameObject.GetName());

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	std::string nameJson = directory;
	nameJson += "/";
	nameJson +=	gameObject.GetName();
	nameJson += ".meta.json";
	config_file = json_value_init_object();

	uint count = 0;
	uint countResources = 0;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_number_with_std(config, "Prefab.Info.Number of GameObjects", count);
		json_object_dotset_string_with_std(config, "Prefab.Info.Directory Prefab", fileName);
		json_object_dotset_number_with_std(config, "Prefab.Info.Resources.Number of Resources", countResources);
		std::experimental::filesystem::file_time_type temp = std::experimental::filesystem::last_write_time(fileName);
		std::time_t cftime = decltype(temp)::clock::to_time_t(temp);
		json_object_dotset_number_with_std(config, "Prefab.Info.Resources.Last Write", cftime);
		config_node = json_object_get_object(config, "Prefab");
		std::string name = "GameObject" + std::to_string(count++);
		name += ".";
		// UUID--------
		json_object_dotset_number_with_std(config_node, name + "UUID", gameObject.GetUUID());
		// Parent UUID------------
		json_object_dotset_number_with_std(config_node, name + "Parent", -1);
		// Name- --------
		json_object_dotset_string_with_std(config_node, name + "Name", gameObject.GetName());

		// Components  ------------
		std::string components = name;
		json_object_dotset_number_with_std(config_node, components + "Number of Components", gameObject.GetNumComponents());
		if (gameObject.GetNumComponents() > 0)
		{
			components += "Components.";
			gameObject.SaveComponents(config_node, components, false, countResources);
		}
		// Childs --------------
		if (gameObject.GetNumChilds() > 0)
		{
			for (int j = 0; j < gameObject.GetNumChilds(); j++)
			{
				SaveChildPrefab(config_node, *gameObject.GetChildbyIndex(j), count, countResources);
			}
		}
		json_object_dotset_number_with_std(config_node, "Info.Number of GameObjects", count);
		json_object_dotset_number_with_std(config, "Prefab.Info.Resources.Number of Resources", countResources);
		json_serialize_to_file(config_file, nameJson.c_str());
	}
	json_value_free(config_file);
}

void JSONSerialization::SaveChildPrefab(JSON_Object* config_node, const GameObject& gameObject, uint& count, uint& countResources)
{
	// Update GameObjects
	std::string name = "GameObject" + std::to_string(count++);
	name += ".";
	// UUID--------
	json_object_dotset_number_with_std(config_node, name + "UUID", gameObject.GetUUID());
	// Parent UUID------------
	int uuidParent = -1;
	if (gameObject.GetParent() != nullptr)
		uuidParent = gameObject.GetParent()->GetUUID();

	json_object_dotset_number_with_std(config_node, name + "Parent", uuidParent);
	// Name- --------
	json_object_dotset_string_with_std(config_node, name + "Name", gameObject.GetName());

	// Components  ------------
	std::string components = name;
	json_object_dotset_number_with_std(config_node, components + "Number of Components", gameObject.GetNumComponents());
	if (gameObject.GetNumComponents() > 0)
	{
		components += "Components.";
		gameObject.SaveComponents(config_node, components, false, countResources);
	}
	if (gameObject.GetNumChilds() > 0)
	{
		for (int i = 0; i < gameObject.GetNumChilds(); i++)
		{
			SaveChildPrefab(config_node, *gameObject.GetChildbyIndex(i), count, countResources);
		}
	}
}

void JSONSerialization::LoadPrefab(const char* prefab)
{
	LOG("LOADING PREFAB %s -----", prefab);

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file(prefab);
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Prefab");
		int NUmberGameObjects = json_object_dotget_number(config_node, "Info.Number of GameObjects");
		if (NUmberGameObjects > 0)
		{
			// First, check name is not repete.
			// Frist reset Vector Names
			//for (int i = 0; i < namesScene.size(); i++)
			//{
			//	RELEASE_ARRAY(namesScene[i]);
			//}
			namesScene.clear();
			// Now GetAll Names from Scene
			GetAllNames(App->scene->root->GetChildsVec());

			GameObject* mainParent = nullptr;
			for (int i = 0; i < NUmberGameObjects; i++)
			{
				std::string name = "GameObject" + std::to_string(i);
				name += ".";
				char* nameGameObject = App->GetCharfromConstChar(json_object_dotget_string_with_std(config_node, name + "Name"));
				uint uid = json_object_dotget_number_with_std(config_node, name + "UUID");
				GameObject* obj = new GameObject(nameGameObject, uid);
				// Now Check that the name is not repet
				CheckChangeName(*obj);
				//Load Components
				int NumberofComponents = json_object_dotget_number_with_std(config_node, name + "Number of Components");
				if (NumberofComponents > 0)
				{
					obj->LoadComponents(config_node, name + "Components.", NumberofComponents);
				}
				int uuid_parent = json_object_dotget_number_with_std(config_node, name + "Parent");

				//Add GameObject
				if (uuid_parent == -1)
				{
					//App->scene->gameobjects.push_back(obj);
					mainParent = obj;
				}
				else
				{
					LoadChildLoadPrefab(*mainParent, *obj, uuid_parent);
				}
			}
			// Now Iterate All GameObjects and Components and create a new UUID!
			mainParent->SetUUIDRandom();
			if (mainParent->GetNumChilds() > 0)
			{
				ChangeUUIDs(*mainParent);
			}
			// Finaly, add gameObject in Scene.
			App->scene->root->AddChildGameObject(mainParent);
		}
	}
	json_value_free(config_file);
}

GameObject* JSONSerialization::GetLoadPrefab(const char* prefab)
{
	LOG("LOADING PREFAB %s -----", prefab);

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file(prefab);
	GameObject* mainParent = nullptr;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Prefab");
		int NUmberGameObjects = json_object_dotget_number(config_node, "Info.Number of GameObjects");
		if (NUmberGameObjects > 0)
		{
			// First, check name is not repete.
			// Frist reset Vector Names
			//for (int i = 0; i < namesScene.size(); i++)
			//{
			//	RELEASE_ARRAY(namesScene[i]);
			//}
			namesScene.clear();
			// Now GetAll Names from Scene
			GetAllNames(App->scene->root->GetChildsVec());

			for (int i = 0; i < NUmberGameObjects; i++)
			{
				std::string name = "GameObject" + std::to_string(i);
				name += ".";
				char* nameGameObject = App->GetCharfromConstChar(json_object_dotget_string_with_std(config_node, name + "Name"));
				uint uid = json_object_dotget_number_with_std(config_node, name + "UUID");
				GameObject* obj = new GameObject(nameGameObject, uid);
				// Now Check that the name is not repet
				CheckChangeName(*obj);
				//Load Components
				int NumberofComponents = json_object_dotget_number_with_std(config_node, name + "Number of Components");
				if (NumberofComponents > 0)
				{
					obj->LoadComponents(config_node, name + "Components.", NumberofComponents);
				}
				int uuid_parent = json_object_dotget_number_with_std(config_node, name + "Parent");

				//Add GameObject
				if (uuid_parent == -1)
				{
					//App->scene->gameobjects.push_back(obj);
					mainParent = obj;
				}
				else
				{
					LoadChildLoadPrefab(*mainParent, *obj, uuid_parent);
				}
			}
			// Now Iterate All GameObjects and Components and create a new UUID!
			mainParent->SetUUIDRandom();
			if (mainParent->GetNumChilds() > 0)
			{
				ChangeUUIDs(*mainParent);
			}
		}
	}
	json_value_free(config_file);
	return mainParent;
}

void JSONSerialization::LoadChildLoadPrefab(GameObject& parent, GameObject& child, int uuidParent)
{
	if (parent.GetNumChilds() > 0)
	{
		for (int i = 0; i < parent.GetNumChilds(); i++)
		{
			if (parent.GetUUID() == uuidParent)
			{
				parent.AddChildGameObject(&child);
				return;
			}
			else
			{
				LoadChildLoadPrefab(*parent.GetChildbyIndex(i), child, uuidParent);
			}
		}
	}
	else
	{
		if (parent.GetUUID() == uuidParent)
		{
			parent.AddChildGameObject(&child);
			return;
		}
	}
}

void JSONSerialization::SaveMapWalkable(std::vector<std::string>& map, int height_map, int width_map, float separation, const char* name)
{
	LOG("SAVING Map %s -----", name);

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	std::string nameJson = App->fs->GetMainDirectory();
	nameJson += "/Maps/";
	nameJson += name;
	nameJson += ".mapwalk.json";
	config_file = json_value_init_object();

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Map.Info.Name Map", name);
		json_object_dotset_number_with_std(config, "Map.Info.Height Map", height_map);
		json_object_dotset_number_with_std(config, "Map.Info.Width Map", width_map);
		json_object_dotset_number_with_std(config, "Map.Info.Separation", separation);
		config_node = json_object_get_object(config, "Map");

		if (height_map == map.size())
		{
			for (int i = 0; i < map.size(); i++)
			{
				std::string line = "Line_" + std::to_string(i);
				//line += ".";
				json_object_dotset_string_with_std(config_node, line, map[i].c_str());
			}
		}
		else
		{
			LOG("[error]Error with Save Map!");
		}
	}
	json_serialize_to_file(config_file, nameJson.c_str());
	json_value_free(config_file);
}

bool JSONSerialization::LoadMapWalkable(std::vector<std::string>& map, int& height_map, int& width_map, float& separation, const char* file, std::string& name_map)
{
	//LOG("LOADING MAP %s -----", file);

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file(file);
	if (config_file)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Map");
		height_map = json_object_dotget_number_with_std(config_node, "Info.Height Map");
		width_map = json_object_dotget_number_with_std(config_node, "Info.Width Map");
		separation = json_object_dotget_number_with_std(config_node, "Info.Separation");
		name_map = json_object_dotget_string_with_std(config_node, "Info.Name Map");

		for (int i = 0; i < height_map; i++)
		{
			std::string line = "Line_" + std::to_string(i);
			//line += ".";
			map.push_back(json_object_dotget_string_with_std(config_node, line));
		}
		json_value_free(config_file);
		return true;
	}
	json_value_free(config_file);
	return false;
}

void JSONSerialization::SaveMapCreation(std::vector<std::string>& map, std::vector<std::string>& prefabs, int height_map, int width_map, float separation, const char * name)
{
	LOG("SAVING Map %s -----", name);

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	std::string nameJson = App->fs->GetMainDirectory();
	nameJson += "/Maps/";
	nameJson += name;
	nameJson += ".map3d.json";
	config_file = json_value_init_object();

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Map.Info.Name Map", name);
		json_object_dotset_number_with_std(config, "Map.Info.Height Map", height_map);
		json_object_dotset_number_with_std(config, "Map.Info.Width Map", width_map);
		json_object_dotset_number_with_std(config, "Map.Info.Separation", separation);
		if (prefabs.size() > 0)
		{
			json_object_dotset_number_with_std(config, "Map.Prefabs.Number of Prefabs", prefabs.size());
			for (int i = 0; i < prefabs.size(); i++)
			{
				std::string pref = "Map.Prefabs.Prefab " + std::to_string(i);
				//line += ".";
				json_object_dotset_string_with_std(config, pref, prefabs[i].c_str());
			}
		}
		else
		{
			LOG("[error]Can't save this maps, you have to select Prefabs");
		}
		config_node = json_object_get_object(config, "Map");
		if (height_map == map.size())
		{
			for (int i = 0; i < map.size(); i++)
			{
				std::string line = "Line_" + std::to_string(i);
				//line += ".";
				json_object_dotset_string_with_std(config_node, line, map[i].c_str());
			}
		}
		else
		{
			LOG("[error]Error with Save Map!");
		}
	}
	json_serialize_to_file(config_file, nameJson.c_str());
	json_value_free(config_file);
}

void JSONSerialization::SaveMaterial(const ResourceMaterial* material, const char* directory, const char* fileName)
{
	LOG("SAVING Material %s -----", material->name);

	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = directory;
	nameJson += "/";
	nameJson += material->name;
	nameJson += ".meta.json";
	config_file = json_value_init_object();

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Material.Directory Material", fileName);
		json_object_dotset_number_with_std(config, "Material.UUID Resource", material->GetUUID());
		json_object_dotset_string_with_std(config, "Material.Name", material->name);
		std::experimental::filesystem::file_time_type temp = std::experimental::filesystem::last_write_time(fileName);
		std::time_t cftime = decltype(temp)::clock::to_time_t(temp);
		json_object_dotset_number_with_std(config, "Material.Last Write", cftime);
		json_serialize_to_file(config_file, nameJson.c_str());
	}
	json_value_free(config_file);
}



// Utilities --------------------------------------------------------------------------

void JSONSerialization::SaveScript(const ResourceScript* script, const char* directory, const char* fileName)
{
	LOG("SAVING Script %s -----", script->name);

	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = fileName;
	nameJson += ".meta.json";
	config_file = json_value_init_object();

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Script.Directory Script", fileName);
		json_object_dotset_number_with_std(config, "Script.UUID Resource", script->GetUUID());
		json_object_dotset_string_with_std(config, "Script.Name", script->name);
		json_object_dotset_string_with_std(config, "Script.PathDLL", script->GetPathdll().c_str());
		std::experimental::filesystem::file_time_type temp = std::experimental::filesystem::last_write_time(fileName);
		std::time_t cftime = decltype(temp)::clock::to_time_t(temp);
		json_object_dotset_number_with_std(config, "Script.Last Write", cftime);
		json_serialize_to_file(config_file, nameJson.c_str());
	}
	json_value_free(config_file);
}

void JSONSerialization::SaveAnimation(const ResourceAnimation * animation, const char * directory, const char * fileName)
{
	LOG("SAVING Animation %s -----", animation->name);

	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = directory;
	nameJson += ".meta.json";
	config_file = json_value_init_object();

	uint count = 0;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Material.Directory Script", fileName);
		json_object_dotset_number_with_std(config, "Material.UUID Resource", animation->GetUUID());
		json_object_dotset_string_with_std(config, "Material.Name", animation->name);
		json_serialize_to_file(config_file, nameJson.c_str());
	}
	json_value_free(config_file);
}

ReImport JSONSerialization::GetUUIDPrefab(const char* file, uint id)
{
	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	std::string nameJson = file;
	nameJson += ".meta.json";
	config_file = json_parse_file(nameJson.c_str());

	ReImport info;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Prefab");
		std::string temp = "Info.Resources";
		int numResources = json_object_dotget_number_with_std(config_node, temp + ".Number of Resources");
		info.directory_obj = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Prefab.Info.Directory Prefab"));
		if (id < numResources)
		{
			temp += ".Resource " + std::to_string(id);
			info.uuid = json_object_dotget_number_with_std(config_node, temp + ".UUID Resource");
			info.name_mesh = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config_node, temp + ".Name"));
			if (strcmp(file, info.directory_obj) == 0)
			{
				json_value_free(config_file);
				return info;
			}
			else
			{
				info.directory_obj = nullptr;
			}

		}
	}
	json_value_free(config_file);
	return info;
}

ReImport JSONSerialization::GetUUIDMaterial(const char* file)
{
	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = file;
	nameJson += ".meta.json";
	config_file = json_parse_file(nameJson.c_str());

	ReImport info;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		//config_node = json_object_get_object(config, "Material");
		info.uuid = json_object_dotget_number_with_std(config, "Material.UUID Resource");
		info.directory_obj = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Material.Directory Material"));
		info.name_mesh = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Material.Name"));
		if (strcmp(file, info.directory_obj) == 0)
		{
			json_value_free(config_file);
			return info;
		}
		else
		{
			info.directory_obj = nullptr;
		}
	}
	json_value_free(config_file);
	return info;
}

ReImport JSONSerialization::GetUUIDScript(const char* file)
{
	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = file;
	std::string name_path = file;
	App->fs->NormalitzatePath(name_path);
	nameJson += ".meta.json";
	config_file = json_parse_file(nameJson.c_str());

	ReImport info;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		//config_node = json_object_get_object(config, "Material");
		info.uuid = json_object_dotget_number_with_std(config, "Script.UUID Resource");
		info.directory_obj = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Script.Directory Script"));
		std::string dir = info.directory_obj;
		App->fs->NormalitzatePath(dir);
		info.name_mesh = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Script.Name"));
		info.path_dll = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Script.PathDLL"));
		if (strcmp(name_path.c_str(), dir.c_str()) == 0)
		{
			json_value_free(config_file);
			return info;
		}
		else
		{
			info.directory_obj = nullptr;
		}
	}
	json_value_free(config_file);
}

std::time_t JSONSerialization::GetLastWritePrefab(const char* file)
{
	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = file;
	nameJson += ".meta.json";
	config_file = json_parse_file(nameJson.c_str());

	std::time_t last_write = 0;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		last_write = json_object_dotget_number(config, "Prefab.Info.Resources.Last Write");
	}
	json_value_free(config_file);
	return last_write;
}

std::time_t JSONSerialization::GetLastWriteMaterial(const char* file)
{
	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = file;
	nameJson += ".meta.json";
	config_file = json_parse_file(nameJson.c_str());

	std::time_t last_write = 0;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		last_write = json_object_dotget_number(config, "Material.Last Write");
	}
	json_value_free(config_file);
	return last_write;
}

std::time_t JSONSerialization::GetLastWriteScript(const char* file)
{
	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = file;
	nameJson += ".meta.json";
	config_file = json_parse_file(nameJson.c_str());

	std::time_t last_write = 0;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		last_write = json_object_dotget_number(config, "Script.Last Write");
	}
	json_value_free(config_file);
	return last_write;
}

void JSONSerialization::Create_Json_Doc(JSON_Value **root_value_scene, JSON_Object **root_object_scene, const char* namefile) 
{
	json_set_allocation_functions(counted_malloc, counted_free);

	*root_value_scene = json_value_init_object();
	*root_object_scene = json_value_get_object(*root_value_scene);

	*root_value_scene = json_parse_file(namefile);
	if (*root_value_scene == NULL) {
		*root_value_scene = json_value_init_object();
		json_serialize_to_file(*root_value_scene, namefile);
		*root_object_scene = json_value_get_object(*root_value_scene);
	}
	else {
		*root_object_scene = json_value_get_object(*root_value_scene);
	}

}

void JSONSerialization::ChangeUUIDs(GameObject& gameObject)
{
	for (int i = 0; i < gameObject.GetNumChilds(); i++)
	{
		gameObject.GetChildbyIndex(i)->SetUUIDRandom();

		if (gameObject.GetChildbyIndex(i)->GetNumChilds() > 0)
		{
			ChangeUUIDs(*gameObject.GetChildbyIndex(i));
		}
	}
}

void JSONSerialization::CheckChangeName(GameObject& gameObject)
{
	for (int i = 0; i < namesScene.size(); i++)
	{
		if (strcmp(namesScene[i], gameObject.GetName()) == 0)
		{
			bool stop = false;
			int it = 0;
			std::string temp;
			while (stop == false)
			{
				it++;
				temp = gameObject.GetName();
				temp += " (" + std::to_string(it) + ")";
				bool unique = true;
				for (int ds = 0; ds < namesScene.size(); ds++)
				{
					if (strcmp(namesScene[ds], temp.c_str()) == 0)
					{
						unique = false;
						continue;
					}
				}
				if (unique)
				{
					gameObject.SetName(App->GetCharfromConstChar(temp.c_str()));
					stop = true;
				}
			}
		}
	}
}

void JSONSerialization::GetAllNames(const std::vector<GameObject*>& gameobjects)
{
	for (int i = 0; i < gameobjects.size(); i++)
	{
		namesScene.push_back(gameobjects[i]->GetName());
		if (gameobjects[i]->GetNumChilds() > 0)
		{
			GetAllNames(gameobjects[i]->GetChildsVec());
		}
	}
}

//Functions to support the function Create_Json_Doc-------------------

static void *counted_malloc(size_t size) 
{
	void *res = malloc(size);
	if (res != NULL) {
		malloc_count++;
	}
	return res;
}

static void counted_free(void *ptr) 
{
	if (ptr != NULL) {
		malloc_count--;
	}
	free(ptr);
}

//---------------------------------------------------------------------------