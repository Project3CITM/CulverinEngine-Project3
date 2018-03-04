#ifndef _JSONSERIALIZATION_
#define _JSONSERIALIZATION_

#include "parson.h"
#include "Globals.h"
#include <vector>
#include <filesystem>

class GameObject;
class ResourceMaterial;
class ResourceScript;
class ResourceAnimation;
class InputManager;
class InputAction;
class PlayerActions;
struct ReImport;

struct LoadSceneSt
{
	GameObject* go = nullptr;
	int uid_parent;
};

class JSONSerialization
{
public:
	JSONSerialization();
	~JSONSerialization();

	// SAVE & LOAD SCENE --------------------------
	std::string SaveScene();
	void SaveChildGameObject(JSON_Object* config_node, const GameObject& gameObject, uint& count, uint& countResources);
	void LoadScene(const char* sceneName);
	// --------------------------------------

	// SAVE & LOAD PREFAB --------------------------
	void SavePrefab(const GameObject& gameObject, const char* directory, const char* fileName, bool is_FBX = true);
	void SaveChildPrefab(JSON_Object* config_node, const GameObject& gameObject, uint& count, uint& countResources);
	void LoadPrefab(const char* prefab);
	GameObject* GetLoadPrefab(const char* prefab);
	//GameObject* GetLoadPrefab(std::string prefab, float3 position = float3(0, 0, 0), Quat rotation = Quat(0, 0, 0, 0)); Need...

	void LoadChildLoadPrefab(GameObject& parent, GameObject& child, int uuidParent);
	// --------------------------------------

	// SAVE & LOAD MAP ---------------------------
	void SaveMapWalkable(std::vector<std::string>& map, int height_map, int width_map, float separation, const char* name);
	bool LoadMapWalkable(std::vector<std::string>& map, int& height_map, int& width_map, float& separation, const char* name, std::string& name_map);

	void SaveMapCreation(std::vector<std::string>& map, std::vector<std::string>& prefabs, int height_map, int width_map, float separation, const char* name);
	void LoadMapCreation(std::vector<std::string>& map, std::vector<std::string>& prefabs, int height_map, int width_map, float separation, const char* name);

	void SaveMapNavigation(std::vector<std::string>& map, int height_map, int width_map, float separation, const char* name);
	void LoadMapNavigation(std::vector<std::string>& map, int height_map, int width_map, float separation,const char* name);
	// --------------------------------------

	// SAVE & LOAD MATERIAL --------------------------
	void SaveMaterial(const ResourceMaterial* material, const char* directory, const char* fileName);
	//void LoadMaterial(const char* Material); At the moment no need this
	// --------------------------------------

	// SAVE & LOAD SCRIPT --------------------------
	void SaveScript(const ResourceScript* script, const char* directory, const char* fileName);
	// --------------------------------------

	// SAVE & LOAD ANIMATION --------------------------
	void SaveAnimation(const ResourceAnimation* script, const char* directory, const char* fileName);
	// --------------------------------------
	// SAVE & LOAD PLAYER ACTION --------------------------
	void SavePlayerAction(const PlayerActions* player_action, const char* directory, const char* fileName);
	void SaveInputManager(JSON_Object* config_node, const InputManager* input_manager, uint count, uint action_count);
	void SaveInputAction(JSON_Object* config_node, const InputAction* input_action, uint count, uint input_count);
	void LoadPlayerAction(PlayerActions** player_action,const char* fileName);


	// --------------------------------------

	// Get UUIDs -------------------------------
	ReImport GetUUIDPrefab(const char* file, uint id);
	ReImport GetUUIDMaterial(const char* file);
	ReImport GetUUIDScript(const char* file);
	// -----------------------------------------------

	// Get Last Write -------------------------------
	std::time_t GetLastWritePrefab(const char* file);
	std::time_t GetLastWriteMaterial(const char* file);
	std::time_t GetLastWriteScript(const char* file);
	// -----------------------------------------------

	// This function will recive a path and will return a json_obj and json_value pointing to the file, to read and write
	void Create_Json_Doc(JSON_Value **root_value_scene, JSON_Object **root_object_scene, const char* namefile);
	//----------------------------------------------------------------

private:
	// UUID & Name
	void ChangeUUIDs(GameObject& gameObject);
	void CheckChangeName(GameObject& gameObject);
	// Get All Names form Scene -------
	void GetAllNames(const std::vector<GameObject*>& gameobjects);

private:
	std::vector<const char*> namesScene;
};

#endif

