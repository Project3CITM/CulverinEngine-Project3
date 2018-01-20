#include "JSONSerialization.h"
#include "Application.h"
#include "ResourceMaterial.h"
#include "ResourceScript.h"
#include "ModuleFS.h"
#include "ModuleResourceManager.h"
#include "Scene.h"
#include "GameObject.h"

JSONSerialization::JSONSerialization()
{
}


JSONSerialization::~JSONSerialization()
{
	namesScene.clear();
}

void JSONSerialization::SaveScene()
{
	LOG("SAVING SCENE -----");

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file("Scene_1.json");

	uint count = 0;
	uint countResources = 0; // no use!
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Scene");
		json_object_clear(config_node);
		json_object_dotset_number_with_std(config_node, "Info.Number of GameObjects", App->scene->gameobjects.size());

		// Update GameObjects
		for (uint i = 0; i < App->scene->gameobjects.size(); i++)
		{
			std::string name = "GameObject" + std::to_string(count++);
			name += ".";
			// UUID--------
			json_object_dotset_number_with_std(config_node, name + "UUID", App->scene->gameobjects[i]->GetUUID());
			// Parent UUID------------
			json_object_dotset_number_with_std(config_node, name + "Parent", -1);
			// Name --------
			json_object_dotset_string_with_std(config_node, name + "Name", App->scene->gameobjects[i]->GetName());
			// Bounding Box ---------
			json_object_dotset_boolean_with_std(config_node, name + "Bounding Box", App->scene->gameobjects[i]->isAABBActive());
			// Static ---------
			json_object_dotset_boolean_with_std(config_node, name + "Static", App->scene->gameobjects[i]->isStatic());

			// Components  ------------
			std::string components = name;
			json_object_dotset_number_with_std(config_node, components + "Number of Components", App->scene->gameobjects[i]->GetNumComponents());
			if (App->scene->gameobjects[i]->GetNumComponents() > 0)
			{
				components += "Components.";
				App->scene->gameobjects[i]->SaveComponents(config_node, components, true, countResources);
			}
			if (App->scene->gameobjects[i]->GetNumChilds() > 0)
			{
				for (int j = 0; j < App->scene->gameobjects[i]->GetNumChilds(); j++)
				{
					SaveChildGameObject(config_node, *App->scene->gameobjects[i]->GetChildbyIndex(j), count, countResources);
				}
			}
		}
	}
	json_object_dotset_number_with_std(config_node, "Info.Number of GameObjects", count);
	json_serialize_to_file(config_file, "Scene_1.json");
	json_value_free(config_file);
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
	json_object_dotset_boolean_with_std(config_node, name + "Bounding Box", gameObject.isAABBActive());
	// Static ---------
	json_object_dotset_boolean_with_std(config_node, name + "Static", gameObject.isStatic());


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

void JSONSerialization::LoadScene()
{
	LOG("LOADING SCENE -----");

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file("Scene_1.json");
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Scene");
		int NUmberGameObjects = json_object_dotget_number(config_node, "Info.Number of GameObjects");
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

				//Add GameObject
				if (uuid_parent == -1)
				{
					App->scene->gameobjects.push_back(obj);
				}
				else
				{
					for (int x = 0; x < App->scene->gameobjects.size(); x++)
					{
						LoadChilds(*App->scene->gameobjects[x], *obj, uuid_parent);
					}
				}
			}
		}
	}
	json_value_free(config_file);
}

void JSONSerialization::LoadChilds(GameObject& parent, GameObject& child, int uuidParent)
{
	if (parent.GetNumChilds() > 0)
	{
		for (int i = 0; i < parent.GetNumChilds(); i++)
		{
			if (parent.GetUUID() == uuidParent)
			{
				parent.AddChildGameObject_Load(&child);
				return;
			}
			else
			{
				LoadChilds(*parent.GetChildbyIndex(i), child, uuidParent);
			}
		}
	}
	else
	{
		if (parent.GetUUID() == uuidParent)
		{
			parent.AddChildGameObject_Load(&child);
			return;
		}
	}
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
			GetAllNames(App->scene->gameobjects);

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
			App->scene->gameobjects.push_back(mainParent);
		}
	}
	json_value_free(config_file);
}

void JSONSerialization::LoadChildLoadPrefab(GameObject& parent, GameObject& child, int uuidParent)
{
	if (parent.GetNumChilds() > 0)
	{
		for (int i = 0; i < parent.GetNumChilds(); i++)
		{
			if (parent.GetUUID() == uuidParent)
			{
				parent.AddChildGameObject_Load(&child);
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
			parent.AddChildGameObject_Load(&child);
			return;
		}
	}
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

	uint count = 0;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Material.Directory Material", fileName);
		json_object_dotset_number_with_std(config, "Material.UUID Resource", material->GetUUID());
		json_object_dotset_string_with_std(config, "Material.Name", material->name);
		json_serialize_to_file(config_file, nameJson.c_str());
	}
	json_value_free(config_file);
}



// Utilities --------------------------------------------------------------------------

void JSONSerialization::SaveScript(const ResourceScript* script, const char * directory, const char * fileName)
{
	LOG("SAVING Script %s -----", script->name);

	JSON_Value* config_file;
	JSON_Object* config;

	std::string nameJson = directory;
	nameJson += "/";
	nameJson += script->name;
	nameJson += ".meta.json";
	config_file = json_value_init_object();

	uint count = 0;
	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		json_object_dotset_string_with_std(config, "Material.Directory Script", fileName);
		json_object_dotset_number_with_std(config, "Material.UUID Resource", script->GetUUID());
		json_object_dotset_string_with_std(config, "Material.Name", script->name);
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
		info.directoryObj = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Prefab.Info.Directory Prefab"));
		if (id < numResources)
		{
			temp += ".Resource " + std::to_string(id);
			info.uuid = json_object_dotget_number_with_std(config_node, temp + ".UUID Resource");
			info.nameMesh = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config_node, temp + ".Name"));
			if (strcmp(file, info.directoryObj) == 0)
			{
				json_value_free(config_file);
				return info;
			}
			else
			{
				info.directoryObj = nullptr;
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
		info.directoryObj = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Material.Directory Material"));
		info.nameMesh = App->fs->ConverttoConstChar(json_object_dotget_string_with_std(config, "Material.Name"));
		if (strcmp(file, info.directoryObj) == 0)
		{
			json_value_free(config_file);
			return info;
		}
		else
		{
			info.directoryObj = nullptr;
		}
	}
	json_value_free(config_file);
	return info;
}

ReImport JSONSerialization::GetUUIDScript(const char * file)
{
	return ReImport();
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
