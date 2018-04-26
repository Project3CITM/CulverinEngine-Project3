#include "JSONSerialization.h"
#include "Application.h"
#include "ResourceMaterial.h"
#include "ResourceScript.h"
#include "ResourceFont.h"
#include "ResourceAnimation.h"
#include "CompScript.h"
#include "ModuleFS.h"
#include "ModuleResourceManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "ModuleAudio.h"
#include "InputManager.h"
#include "InputAction.h"
#include "PlayerActions.h"
#include "ModuleAnimation.h"
#include "ResourceMesh.h"

static int malloc_count;
static void *counted_malloc(size_t size);
static void counted_free(void *ptr);

JSONSerialization::JSONSerialization()
{
	json_set_allocation_functions(counted_malloc, counted_free);
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
		json_object_dotset_number_with_std(config, "Scene.Info.Tags.Number of Tags", App->scene->TagsSize());
		std::vector<std::string>* tag_ptr = App->scene->GetTagsVec();
		uint size = tag_ptr->size();
		for (uint k = 0; k < size; k++)
		{
			char buffer[200];
			sprintf(buffer, "Scene.Info.Tags.Tag%i", k);
			json_object_dotset_string_with_std(config, buffer, tag_ptr->at(k).c_str());
		}

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
	// Tag ----------
	json_object_dotset_string_with_std(config_node, name + "Tag", gameObject.GetTag());
	// Bounding Box ---------
	json_object_dotset_boolean_with_std(config_node, name + "Bounding Box", gameObject.IsAABBActive());
	// Static ---------
	json_object_dotset_boolean_with_std(config_node, name + "Static", gameObject.IsStatic());

	json_object_dotset_boolean_with_std(config_node, name + "Animation_Translations", gameObject.AreTranslationsActivateds());

	json_object_dotset_boolean_with_std(config_node, name + "Animation_Rotations", gameObject.AreRotationsActivateds());

	json_object_dotset_boolean_with_std(config_node, name + "Animation_Scale", gameObject.AreScalesActivateds());

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

	Timer timetp;
	timetp.Start();
	config_file = json_parse_file(sceneName);
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Scene");
		int NUmberGameObjects = json_object_dotget_number(config_node, "Info.Number of GameObjects");
		int number_of_tags = json_object_dotget_number(config_node, "Info.Tags.Number of Tags");
		for (uint k = 0; k < number_of_tags; k++)
		{
			char buffer[200];
			sprintf(buffer, "Info.Tags.Tag%i", k);
			if (!App->scene->FindTag(json_object_dotget_string(config_node, buffer)))App->scene->AddTag(json_object_dotget_string(config_node, buffer));
		}
		App->scene->root->SetUUID(json_object_dotget_number(config_node, "Scene.Properties.UUID"));
		App->scene->root->SetName(json_object_dotget_string_with_std(config_node, "Scene.Properties.Name"));
		std::vector<LoadSceneSt> templist;
		std::vector<GameObject*> mesh_gos;
		if (NUmberGameObjects > 0)
		{
			templist.reserve(NUmberGameObjects);
			for (int i = 0; i < NUmberGameObjects; i++)
			{
				std::string name = "GameObject" + std::to_string(i);
				name += ".";
				const char* nameGameObject = json_object_dotget_string_with_std(config_node, name + "Name");
				const char* tagGameObject = json_object_dotget_string_with_std(config_node, name + "Tag");
				uint uid = json_object_dotget_number_with_std(config_node, name + "UUID");
				GameObject* obj = new GameObject(nameGameObject, uid);
				if (App->scene->FindTag(tagGameObject))obj->SetTag(tagGameObject);
				bool static_obj = json_object_dotget_boolean_with_std(config_node, name + "Static");
				obj->SetStatic(static_obj);
				bool aabb_active = json_object_dotget_boolean_with_std(config_node, name + "Bounding Box");
				obj->SetAABBActive(aabb_active);


				bool anim_translations = json_object_dotget_boolean_with_std(config_node, name + "Animation_Translations");
				obj->ToggleAnimationTranslations(anim_translations);

				bool anim_rotations = json_object_dotget_boolean_with_std(config_node, name + "Animation_Rotations");
				obj->ToggleAnimationRotation(anim_rotations);

				bool anim_scales = json_object_dotget_boolean_with_std(config_node, name + "Animation_Scale");
				obj->ToggleAnimationScale(anim_scales);

				//Load Components
				int NumberofComponents = json_object_dotget_number_with_std(config_node, name + "Number of Components");
				if (NumberofComponents > 0)
				{
					obj->LoadComponents(config_node, name + "Components.", NumberofComponents);
					if (obj->GetComponentMesh() != nullptr)
						mesh_gos.push_back(obj);
				}
				int uuid_parent = json_object_dotget_number_with_std(config_node, name + "Parent");

				LoadSceneSt temp;
				temp.go = obj;
				temp.uid_parent = uuid_parent;
				templist.push_back(temp);
			}
		}

		const int num_objects = templist.size();
		LoadSceneSt* scene_ptr = (num_objects > 0) ? templist.data() : nullptr;

		// Now with uid parent add childs.
		for (int i = 0; i < num_objects; i++)
		{
			if (scene_ptr[i].uid_parent != -1)
			{
				for (int j = 0; j < num_objects; j++)
				{
					if (scene_ptr[i].uid_parent == scene_ptr[j].go->GetUUID())
					{
						scene_ptr[j].go->AddChildGameObject(scene_ptr[i].go);
					}
				}
			}
		}
		// Now pass vector to root in scene
		for (int i = 0; i < num_objects; i++)
		{
			if (scene_ptr[i].uid_parent == -1)
			{
				App->scene->root->AddChildGameObject(scene_ptr[i].go);
			}
		}
		//Sync components
		for (int i = 0; i < num_objects; i++)
		{
			scene_ptr[i].go->SyncComponents(nullptr);
		}

		//Add static objects to scene
		for (int i = 0; i < num_objects; i++)
		{
			if (scene_ptr[i].go->IsStatic())
			{
				App->scene->octree.Insert(scene_ptr[i].go);
			}
			else
			{
				App->scene->dynamic_objects.push_back(scene_ptr[i].go);
			}
		}

		templist.clear();
		
		App->scene->root->SetName(App->fs->GetOnlyName(sceneName).c_str());

		//Link Skeletons
		for (std::vector<GameObject*>::iterator it = mesh_gos.begin(); it != mesh_gos.end(); it++)
		{
			(*it)->GetComponentMesh()->LinkSkeleton();
		}
				

		//Load Audio Banks
		int number_of_audio_banks = json_object_dotget_number(config_node, "Info.Number of Audio Banks");
		App->audio->LoadAudioBanksFromScene(number_of_audio_banks, config);
	}
	json_value_free(config_file);
	LOG("Load Time %d, [red]", timetp.Read());
}




void JSONSerialization::SavePrefab(const GameObject& gameObject, const char* directory, const char* fileName, bool is_FBX)
{
	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	std::string nameJson = directory;
	nameJson += "/";
	nameJson +=	gameObject.GetName();
	if (is_FBX)
	{
		nameJson += ".meta.json";
	}
	else
	{
		nameJson += ".prefab.json";
	}
	config_file = json_value_init_object();

	uint count = 0;
	uint countResources = 0;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_number_with_std(config, "Prefab.Info.Number of GameObjects", count);
		json_object_dotset_string_with_std(config, "Prefab.Info.Directory Prefab", App->fs->GetToAsstes(fileName).c_str());
		json_object_dotset_number_with_std(config, "Prefab.Info.Resources.Number of Resources", countResources);
		json_object_dotset_number_with_std(config, "Prefab.Info.Resources.Type", (int)Resource::Type::MESH);
		if (is_FBX)
		{
			std::experimental::filesystem::file_time_type temp = std::experimental::filesystem::last_write_time(fileName);
			std::time_t cftime = decltype(temp)::clock::to_time_t(temp);
			json_object_dotset_number_with_std(config, "Prefab.Info.Resources.Last Write", cftime);
		}
		config_node = json_object_get_object(config, "Prefab");
		std::string name = "GameObject" + std::to_string(count++);
		name += ".";
		// UUID--------
		json_object_dotset_number_with_std(config_node, name + "UUID", gameObject.GetUUID());
		// Parent UUID------------
		json_object_dotset_number_with_std(config_node, name + "Parent", -1);
		// Name- --------
		json_object_dotset_string_with_std(config_node, name + "Name", gameObject.GetName());
		// Tag ----------
		json_object_dotset_string_with_std(config_node, name + "Tag", gameObject.GetTag());

		json_object_dotset_boolean_with_std(config_node, name + "AnimRotations", gameObject.animation_rotations);
		json_object_dotset_boolean_with_std(config_node, name + "AnimScales", gameObject.animation_scales);
		json_object_dotset_boolean_with_std(config_node, name + "AnimTranslations", gameObject.animation_translations);

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
	// Tag ----------
	json_object_dotset_string_with_std(config_node, name + "Tag", gameObject.GetTag());

	json_object_dotset_boolean_with_std(config_node, name + "AnimRotations", gameObject.animation_rotations);
	json_object_dotset_boolean_with_std(config_node, name + "AnimScales", gameObject.animation_scales);
	json_object_dotset_boolean_with_std(config_node, name + "AnimTranslations", gameObject.animation_translations);

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
		std::vector<LoadSceneSt> templist;
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
				const char* nameGameObject = json_object_dotget_string_with_std(config_node, name + "Name");
				const char* tagGameObject = nullptr;
				tagGameObject = json_object_dotget_string_with_std(config_node, name + "Tag");
				

				uint uid = json_object_dotget_number_with_std(config_node, name + "UUID");
				GameObject* obj = new GameObject(nameGameObject, uid);
				if(tagGameObject != nullptr)obj->SetTag(tagGameObject);

				obj->animation_rotations = json_object_dotget_boolean_with_std(config_node, name + "AnimRotations");
				obj->animation_scales = json_object_dotget_boolean_with_std(config_node, name + "AnimScales");
				obj->animation_translations = json_object_dotget_boolean_with_std(config_node, name + "AnimTranslations");

				// Now Check that the name is not repet
				CheckChangeName(*obj);
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
				////Add GameObject
				if (uuid_parent == -1)
				{
					//App->scene->gameobjects.push_back(obj);
					mainParent = obj;
				}
				//else
				//{
				//	LoadChildLoadPrefab(*mainParent, *obj, uuid_parent);
				//}
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
				templist[i].go->SyncComponents(mainParent);
			}

			for (uint i = 0; i < templist.size(); i++)
			{
				if (templist[i].go->IsStatic())
				{
					App->scene->octree.Insert(templist[i].go);
				}
				else
				{
					App->scene->dynamic_objects.push_back(templist[i].go);
				}
			}

			// Now Iterate All GameObjects and Components and create a new UUID!
			if (mainParent != nullptr)
			{
				ChangeUUIDs(*mainParent);
			}

			//// Finaly, add gameObject in Scene.
			App->scene->root->AddChildGameObject(mainParent);
			templist.clear();
		}
	}
	json_value_free(config_file);
}

GameObject* JSONSerialization::GetLoadPrefab(const char* prefab, bool is_instantiate)
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
				const char* nameGameObject = json_object_dotget_string_with_std(config_node, name + "Name");
				const char* tagGameObject = nullptr;
				tagGameObject = json_object_dotget_string_with_std(config_node, name + "Tag");
				uint uid = json_object_dotget_number_with_std(config_node, name + "UUID");
				GameObject* obj = new GameObject(nameGameObject, uid);
				if (tagGameObject != nullptr) obj->SetTag(tagGameObject);

				obj->animation_rotations = json_object_dotget_boolean_with_std(config_node, name + "AnimRotations");
				obj->animation_scales = json_object_dotget_boolean_with_std(config_node, name + "AnimScales");
				obj->animation_translations = json_object_dotget_boolean_with_std(config_node, name + "AnimTranslations");

				// Now Check that the name is not repet
				CheckChangeName(*obj);
				//Load Components
				int NumberofComponents = json_object_dotget_number_with_std(config_node, name + "Number of Components");
				if (NumberofComponents > 0)
				{
					obj->LoadComponents(config_node, name + "Components.", NumberofComponents);
				}
				if (is_instantiate)
				{
					//do_start
					for (int i = 0; i < obj->GetNumComponents(); i++)
					{
						Component* temp = obj->GetComponentbyIndex(i);
						if (temp->GetType() == Comp_Type::C_SCRIPT)
						{
							((CompScript*)temp)->do_start = true;
						}
					}
				}
				int uuid_parent = json_object_dotget_number_with_std(config_node, name + "Parent");

				// Add GameObject to Dynamicvector
				if (!obj->IsStatic())
				{
					App->scene->dynamic_objects.push_back(obj);
				}

				//Add GameObject
				if (uuid_parent == -1)
				{
					//App->scene->gameobjects.push_back(obj);
					mainParent = obj;
					mainParent->SetParent(nullptr);
				}
				else
				{
					LoadChildLoadPrefab(*mainParent, *obj, uuid_parent);
				}
			}

			//Sync components Recursive
			if (mainParent!= nullptr)
			{
				mainParent->SyncComponentsRecursive(mainParent);
			}

			// Now Iterate All GameObjects and Components and create a new UUID!
			ChangeUUIDs(*mainParent);
			
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

bool JSONSerialization::LoadMapCreation(std::vector<std::string>& map, std::vector<std::string>& prefabs, int& height_map, int& width_map, float& separation, int& number_prefabs, const char* file, std::string& name)
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
		name = json_object_dotget_string_with_std(config_node, "Info.Name Map");
		height_map = json_object_dotget_number_with_std(config_node, "Info.Height Map");
		width_map = json_object_dotget_number_with_std(config_node, "Info.Width Map");
		separation = json_object_dotget_number_with_std(config_node, "Info.Separation");

		number_prefabs = json_object_dotget_number_with_std(config_node, "Prefabs.Number of Prefabs");
		if (number_prefabs > 0)
		{
			for (int i = 0; i < number_prefabs; i++)
			{
				std::string pref = "Prefabs.Prefab " + std::to_string(i);
				//line += ".";
				prefabs.push_back(json_object_dotget_string_with_std(config_node, pref));
			}
		}
		else
		{
			json_value_free(config_file);
			return false;
		}
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

void JSONSerialization::SaveMaterial(const ResourceMaterial* material, const char* directory, const char* fileName)
{
	LOG("SAVING Material %s -----", material->name.c_str());

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
		json_object_dotset_string_with_std(config, "Material.Directory Material", App->fs->GetToAsstes(fileName).c_str());
		json_object_dotset_number_with_std(config, "Material.UUID Resource", material->GetUUID());
		json_object_dotset_string_with_std(config, "Material.Name", material->name.c_str());
		json_object_dotset_number_with_std(config, "Material.Type", (int)material->GetType());
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
	LOG("SAVING Script %s -----", script->name.c_str());

	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = fileName;
	nameJson += ".meta.json";
	config_file = json_value_init_object();

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Script.Directory Script", App->fs->GetToAsstes(fileName).c_str());
		json_object_dotset_number_with_std(config, "Script.UUID Resource", script->GetUUID());
		json_object_dotset_string_with_std(config, "Script.Name", script->name.c_str());
		json_object_dotset_string_with_std(config, "Script.PathDLL", script->GetPathdll().c_str());
		json_object_dotset_number_with_std(config, "Script.Type", (int)script->GetType());
		std::experimental::filesystem::file_time_type temp = std::experimental::filesystem::last_write_time(fileName);
		std::time_t cftime = decltype(temp)::clock::to_time_t(temp);
		json_object_dotset_number_with_std(config, "Script.Last Write", cftime);
		json_serialize_to_file(config_file, nameJson.c_str());
	}
	json_value_free(config_file);
}

void JSONSerialization::SaveAnimation(const ResourceAnimation * animation, const char * directory, const char* fileName, const char* library)
{
	LOG("SAVING Animation %s -----", animation->name.c_str());

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
		json_object_dotset_string_with_std(config, "Animation.Directory Animation", App->fs->GetToAsstes(fileName).c_str());
		json_object_dotset_string_with_std(config, "Animation.Directory Library", library);
		json_object_dotset_number_with_std(config, "Animation.UUID Resource", animation->GetUUID());
		json_object_dotset_string_with_std(config, "Animation.Name", animation->name.c_str());
		json_object_dotset_number_with_std(config, "Animation.Type", (int)animation->GetType());
		json_serialize_to_file(config_file, nameJson.c_str());
	}
	json_value_free(config_file);
}

void JSONSerialization::SavePlayerAction(const PlayerActions * player_action, const char * directory, const char * fileName)
{
	JSON_Value* config_file;
	JSON_Object* config;


	std::string nameJson = directory;
	nameJson += "/";
	nameJson += fileName;
	nameJson += ".json";
	config_file = json_value_init_object();

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Input.Directory", fileName);
		json_object_dotset_number_with_std(config, "Input.InputManager.Size", player_action->GetInteractiveVector().size());

		if (!player_action->GetInteractiveVector().empty())
		{

			for (uint i = 0; i < player_action->GetInteractiveVector().size(); i++)
			{
				InputManager* item = player_action->GetInteractiveVector()[i];
				SaveInputManager(config, item, i, item->GetActionVector().size());
			}
		}
		json_serialize_to_file(config_file, nameJson.c_str());

	}
	json_value_free(config_file);
}

void JSONSerialization::SaveInputManager(JSON_Object * config_node, const InputManager* input_manager, uint count, uint action_count)
{
	std::string name = "Input.InputManager" + std::to_string(count);
	name += ".";
	json_object_dotset_number_with_std(config_node, name + "InputManagerActionSize", input_manager->GetActionVector().size());
	json_object_dotset_string_with_std(config_node, name + "InputManagerName", input_manager->GetName());
	json_object_dotset_boolean_with_std(config_node, name + "InputManagerActive", input_manager->GetActiveInput());
	json_object_dotset_boolean_with_std(config_node, name + "InputManagerBlock", input_manager->GetBlockAction());

	if (!input_manager->GetActionVector().empty())
	{
	
		for (uint i = 0; i < input_manager->GetActionVector().size(); i++)
		{
	
			InputAction* item = input_manager->GetActionVector()[i];
			SaveInputAction(config_node, item, i, count);
		}
	}

}

void JSONSerialization::SaveInputAction(JSON_Object * config_node, const InputAction * input_action, uint count,uint input_count)
{
	std::string name = "Input.InputManager" + std::to_string(input_count)+".InputAction" + std::to_string(count);
	name += ".";
	json_object_dotset_string_with_std(config_node, name+"InputActionName", input_action->name.c_str());
	json_object_dotset_string_with_std(config_node, name + "KeyRelationNamePositive", input_action->positive_button->name.c_str());
	json_object_dotset_string_with_std(config_node, name + "KeyRelationNameNegative", input_action->negative_button->name.c_str());
	json_object_dotset_number_with_std(config_node, name + "ActionType", input_action->action_type);

}

void JSONSerialization::LoadPlayerAction(PlayerActions** player_action,const char * fileName)
{

	JSON_Value* config_file;
	JSON_Object* config;

	config_file = json_parse_file(fileName);
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);

		int input_manager_size = json_object_dotget_number_with_std(config, "Input.InputManager.Size");
		(*player_action)->number_of_inputs = input_manager_size;
		for (uint i = 0; i < input_manager_size; i++)
		{
			std::string name = "Input.InputManager" + std::to_string(i);
			name += ".";
			InputManager* input_manager = new InputManager((*player_action));
			(*player_action)->interactive_vector.push_back(input_manager);
			input_manager->SetName(json_object_dotget_string_with_std(config, name + "InputManagerName"));
			input_manager->SetActiveInput(json_object_dotget_boolean_with_std(config, name + "InputManagerActive"));
			input_manager->SetBlockAction(json_object_dotget_boolean_with_std(config, name + "InputManagerBlock"));

			int input_action_size = json_object_dotget_number_with_std(config, name + "InputManagerActionSize");

			input_manager->number_of_action = input_action_size;
			for (uint j = 0; j < input_action_size; j++)
			{
				std::string name = "Input.InputManager" + std::to_string(i) + ".InputAction" + std::to_string(j);
				name += ".";
				InputAction* input_action = nullptr;
				std::string action_name = json_object_dotget_string_with_std(config, name + "InputActionName");
				std::string key_relation_positive = json_object_dotget_string_with_std(config, name + "KeyRelationNamePositive");
				std::string key_relation_negative = json_object_dotget_string_with_std(config, name + "KeyRelationNameNegative");
				int action_type = json_object_dotget_number_with_std(config, name + "ActionType");
				input_action = input_manager->CreateNewAction(action_name.c_str(), key_relation_positive.c_str(), key_relation_negative.c_str(),static_cast<ActionInputType>(action_type));
				input_manager->action_vector.push_back(input_action);

			}
		}
		json_object_clear(config);
	}
	json_value_free(config_file);
}

void JSONSerialization::SaveFont(const ResourceFont * font, const char * directory, const char * fileName)
{
	LOG("SAVING Font %s -----", font->name.c_str());

	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = directory;
	nameJson += "/";
	nameJson += font->name;
	nameJson += ".meta.json";
	config_file = json_value_init_object();

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Font.Directory Font", App->fs->GetToAsstes(fileName).c_str());
		json_object_dotset_number_with_std(config, "Font.UUID Resource", font->GetUUID());
		json_object_dotset_string_with_std(config, "Font.Name", font->name.c_str());
		json_object_dotset_number_with_std(config, "Font.Type", (int)font->GetType());
		std::experimental::filesystem::file_time_type temp = std::experimental::filesystem::last_write_time(fileName);
		std::time_t cftime = decltype(temp)::clock::to_time_t(temp);
		json_object_dotset_number_with_std(config, "Font.Last Write", cftime);
		json_serialize_to_file(config_file, nameJson.c_str());
	}
	json_value_free(config_file);
}

void JSONSerialization::SaveUIAnimation(const AnimationJson * animation, const char * directory, const char * fileName)
{
	LOG("SAVING UIAnimation %s -----", animation->name.c_str());

	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = directory;
	nameJson += "/";
	nameJson += animation->name;
	config_file = json_value_init_object();
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_number_with_std(config, "UIAnimation.Size ", animation->animations.size());
		json_object_dotset_number_with_std(config, "UIAnimation.Max_Key ", animation->max_keys);
		json_object_dotset_number_with_std(config, "UIAnimation.Samples ", animation->sample_rate);
		json_object_dotset_string_with_std(config, "UIAnimation.Name ", animation->name.c_str());

		for (int i = 0; i < animation->animations.size(); i++)
		{
			std::string animations = std::to_string(i);
			Component* component = dynamic_cast<Component*>(animation->animations[i]->data);
			
			json_object_dotset_string_with_std(config, "UIAnimation.Name ", App->fs->GetToAsstes(fileName).c_str());
			json_object_dotset_number_with_std(config, "UIAnimation.Type " + animations, component->GetType());
			json_object_dotset_number_with_std(config, "UIAnimation.Keyframe Size " + animations , animation->animations[i]->key_frame_data.size());

			for (int j = 0; j < animation->animations[i]->key_frame_data.size(); j++)
			{
				std::string key_frame = std::to_string(j);

				json_object_dotset_number_with_std(config, "UIAnimation " + animations+".Animations.Max_Key " + key_frame, animation->animations[i]->key_frame_data[j].max_keys);
				json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.Samples " + key_frame, animation->animations[i]->key_frame_data[j].sample_rate);
				json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.Initial " + key_frame, animation->animations[i]->key_frame_data[j].initial);
				json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.Destination " + key_frame, animation->animations[i]->key_frame_data[j].destination);
				json_object_dotset_boolean_with_std(config, "UIAnimation " + animations + ".Animations.Invalid Key " + key_frame, animation->animations[i]->key_frame_data[j].invalid_key);
				json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.Parameter " + key_frame, animation->animations[i]->key_frame_data[j].parameter);


				json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData Size " + key_frame, animation->animations[i]->key_frame_data[j].key_data.size());

				for (int k = 0; k < animation->animations[i]->key_frame_data[j].key_data.size(); k++)
				{
					std::string key_data = std::to_string(k);
					json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Key on time " + key_data, animation->animations[i]->key_frame_data[j].key_data[k].key_on_time);
					json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Keyframe " + key_data, animation->animations[i]->key_frame_data[j].key_data[k].key_frame);
					switch (animation->animations[i]->key_frame_data[j].parameter)
					{
					case ParameterValue::RECT_TRANSFORM_POSITION:
			
						App->fs->json_array_dotset_float3(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Value " + key_data, animation->animations[i]->key_frame_data[j].key_data[k].key_values.f3_value);
						break;
					case ParameterValue::RECT_TRANSFORM_ROTATION:
						App->fs->json_array_dotset_float3(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Value " + key_data, animation->animations[i]->key_frame_data[j].key_data[k].key_values.f3_value);
						break;
					case ParameterValue::RECT_TRANSFORM_SCALE:
						App->fs->json_array_dotset_float3(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Value " + key_data, animation->animations[i]->key_frame_data[j].key_data[k].key_values.f3_value);
						break;
					case ParameterValue::RECT_TRANSFORM_WIDTH:
						json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Key on time " + key_data, animation->animations[i]->key_frame_data[j].key_data[k].key_values.f_value);
						break;
					case ParameterValue::RECT_TRANSFORM_HEIGHT:
						json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Key on time " + key_data, animation->animations[i]->key_frame_data[j].key_data[k].key_values.f_value);
						break;
					case ParameterValue::IMAGE_ALPHA_VALUE:
						json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Alpha on time " + key_data, animation->animations[i]->key_frame_data[j].key_data[k].key_values.f_value);
						break;
					case ParameterValue::IMAGE_SPRITE_ANIM:
						json_object_dotset_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "UUID Sprite on time " + key_data, animation->animations[i]->key_frame_data[j].key_data[k].key_values.sprite->GetUUID());
						break;
					default:
						break;
					}

				}
			}
		}
		json_serialize_to_file(config_file, nameJson.c_str());
	}
	json_value_free(config_file);
}


void JSONSerialization::SaveConfig(std::string config_path, std::string game_name, std::string inital_scene, bool game_mode, bool full_screen, bool resizable_window, bool borderless, bool full_desktop)
{
	JSON_Value* config_file;
	JSON_Object* config;

	config_file = json_parse_file(config_path.c_str());
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		// Application
		json_object_dotset_boolean_with_std(config, "Application.Mode Game", game_mode);
		std::string actual_scene = "Assets/";
		actual_scene += inital_scene;
		json_object_dotset_string_with_std(config, "Application.ActualScene", actual_scene.c_str());
		// Application
		json_object_dotset_string_with_std(config, "Window.Window Name", game_name.c_str());
		json_object_dotset_boolean_with_std(config, "Window.Fullscreen", full_screen);
		json_object_dotset_boolean_with_std(config, "Window.Resizable", resizable_window);
		json_object_dotset_boolean_with_std(config, "Window.Borderless", borderless);
		json_object_dotset_boolean_with_std(config, "Window.Full Desktop", full_desktop);
	}
	json_serialize_to_file(config_file, config_path.c_str());
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
	return info;
	//json_value_free(config_file);
}
ReImport JSONSerialization::GetUUIDFont(const char* file)
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
		info.uuid = json_object_dotget_number_with_std(config, "Font.UUID Resource");
		info.directory_obj = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Font.Directory Material"));
		info.name_mesh = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Font.Name"));
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
	//json_value_free(config_file);
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
std::time_t JSONSerialization::GetLastWriteFont(const char* file)
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
		last_write = json_object_dotget_number(config, "Font.Last Write");
	}
	json_value_free(config_file);
	return last_write;
}

void JSONSerialization::CreateResourcesLoad(std::vector<std::string>& files_meta)
{
	for (std::vector<std::string>::iterator it = files_meta.begin(); it != files_meta.end(); it++)
	{
		JSON_Value* config_file;
		JSON_Object* config;
		JSON_Object* config_node;
		std::string tr = it->c_str();
		config_file = json_parse_file(it._Ptr->c_str());
		if (config_file)
		{
			config = json_value_get_object(config_file);
			Resource::Type type = Resource::Type::UNKNOWN;
			while (type == Resource::Type::UNKNOWN)
			{
				// Check what is the type of resources
				// Material (png, jpg) -----------------------------------
				type = (Resource::Type)(int)json_object_dotget_number_with_std(config, "Material.Type");
				if (type != Resource::Type::UNKNOWN)
				{
					config_node = json_object_get_object(config, "Material");
					break;
				}
				// Prefab (fbx, dae, obj) -----------------------------------
				type = (Resource::Type)(int)json_object_dotget_number_with_std(config, "Prefab.Info.Resources.Type");
				if (type != Resource::Type::UNKNOWN)
				{
					config_node = json_object_get_object(config, "Prefab");
					break;
				}	
				// Script (cs) -----------------------------------
				type = (Resource::Type)(int)json_object_dotget_number_with_std(config, "Script.Type");
				if (type != Resource::Type::UNKNOWN)
				{
					config_node = json_object_get_object(config, "Script");
					break;
				}
				// Animation ------------------------------------
				type = (Resource::Type)(int)json_object_dotget_number_with_std(config, "Animation.Type");
				if (type != Resource::Type::UNKNOWN)
				{
					config_node = json_object_get_object(config, "Animation");
					break;
				}
				// Font  -----------------------------------------
				type = (Resource::Type)(int)json_object_dotget_number_with_std(config, "Font.Type");
				if (type != Resource::Type::UNKNOWN)
				{
					config_node = json_object_get_object(config, "Font");
					break;
				}
			}


			switch (type)
			{
			case Resource::Type::MESH:
			{
				uint number_of_resources = json_object_dotget_number_with_std(config_node, "Info.Resources.Number of Resources");
				for (int i = 0; i < number_of_resources; i++)
				{
					std::string name = "Info.Resources.Resource " + std::to_string(i) + ".";
					uint uid = json_object_dotget_number_with_std(config_node, name + "UUID Resource");
					ResourceMesh* mesh = (ResourceMesh*)App->resource_manager->CreateNewResource(type, uid);
					mesh->InitInfo(json_object_dotget_string_with_std(config_node, name + "Name"), json_object_dotget_string_with_std(config_node, "Info.Directory Prefab"));
				}
				break;
			}
			case Resource::Type::MATERIAL:
			{
				uint uid = json_object_dotget_number_with_std(config_node, "UUID Resource");
				ResourceMaterial* material = (ResourceMaterial*)App->resource_manager->CreateNewResource(type, uid);
				material->InitInfo(json_object_dotget_string_with_std(config_node, "Name"), json_object_dotget_string_with_std(config_node, "Directory Material"));
				break;
			}
			case Resource::Type::SCRIPT:
			{
				uint uid = json_object_dotget_number_with_std(config_node, "UUID Resource");
				ResourceScript* script = (ResourceScript*)App->resource_manager->CreateNewResource(type, uid);
				script->InitInfo(json_object_dotget_string_with_std(config_node, "PathDLL"), json_object_dotget_string_with_std(config_node, "Directory Script"));
				break;
			}
			case Resource::Type::ANIMATION:
			{
				uint uid = json_object_dotget_number_with_std(config_node, "UUID Resource");
				ResourceAnimation* animation = (ResourceAnimation*)App->resource_manager->CreateNewResource(type, uid);
				animation->InitInfo(json_object_dotget_string_with_std(config_node, "Name"), json_object_dotget_string_with_std(config_node, "Directory Animation"), json_object_dotget_string_with_std(config_node, "Directory Library"));
				break;
			}
			case Resource::Type::FONT:
			{
				uint uid = json_object_dotget_number_with_std(config_node, "UUID Resource");
				ResourceFont* font = (ResourceFont*)App->resource_manager->CreateNewResource(type, uid);
				font->InitInfo(json_object_dotget_string_with_std(config_node, "Name"), json_object_dotget_string_with_std(config_node, "Directory Font"));
				break;
			}
			case Resource::Type::UNKNOWN:
			{
				LOG("Error load resoruce");
				break;
			}
			}
		}
		json_value_free(config_file);
	}
}

void JSONSerialization::ResourcesInLibrary(std::string& path, uint type)
{
	switch (type)
	{
	case Resource::Type::MATERIAL:
	{
		path = DIRECTORY_LIBRARY_MATERIALS;
		break;
	}
	case Resource::Type::SCRIPT:
	{
		path = DIRECTORY_LIBRARY_SCRIPTS;
		break;
	}
	case Resource::Type::MESH:
	{
		path = DIRECTORY_LIBRARY_MESHES;
		break;
	}
	case Resource::Type::FONT:
	{
		path = DIRECTORY_LIBRARY_FONTS;
		break;
	}
	}
}

void JSONSerialization::Create_Json_Doc(JSON_Value **root_value_scene, JSON_Object **root_object_scene, const char* namefile)
{
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
	gameObject.SetUUIDRandom();

	for (uint i = 0; i < gameObject.GetNumComponents(); i++)
	{
		gameObject.GetComponentbyIndex(i)->SetUUIDRandom();
	}

	for (int i = 0; i < gameObject.GetNumChilds(); i++)
	{
		ChangeUUIDs(*gameObject.GetChildbyIndex(i));
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
					gameObject.SetName(temp.c_str());
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