#ifndef _JSONSERIALIZATION_
#define _JSONSERIALIZATION_

#include "parson.h"
#include "Globals.h"
#include <vector>
#include <filesystem>
#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4.h"

class GameObject;
class Component;
class ResourceMaterial;
class ResourceScript;
class ResourceAnimation;
class ResourceFont;
class InputManager;
class InputAction;
class PlayerActions;
struct ReImport;
struct AnimationJson;

struct LoadSceneSt
{
	GameObject* go = nullptr;
	uint uid_parent;
};

struct LoadSceneCp
{
	Component* comp = nullptr;
	uint position_parent = 0;
	uint uid_parent = 0;
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

	// SAVE & LOAD SCENE BINARY --------------------------
	std::string SaveSceneBinary();
	void SaveChildGameObjectBinary(const GameObject& gameObject, char** cursor);

	void LoadSceneBinary(std::string scene);
	LoadSceneSt LoadGameObejctBinary(char** cursor);
	LoadSceneCp LoadComponentsBinary(char** cursor);
	void GetBufferSizeRoot(uint &buffer_size);

	void SaveIntBinary(char** cursor, int value);
	void SaveFloatBinary(char** cursor, float value);
	void SaveBooleanBinary(char** cursor, bool value);
	void SaveStringBinary(char** cursor, std::string value);
	void SaveConstCharBinary(char** cursor, const char* value);
	void SaveFloat2Binary(char** cursor, math::float2 value);
	void SaveFloat3Binary(char** cursor, math::float3 value);
	void SaveFloat4Binary(char** cursor, math::float4 value);

	int LoadIntBinary(char** cursor);
	float LoadFloatBinary(char** cursor);
	bool LoadBooleanBinary(char** cursor);
	std::string LoadStringBinary(char** cursor);
	void LoadConstCharBinary(char** cursor, const char** value);
	math::float2 LoadFloat2Binary(char** cursor);
	math::float3 LoadFloat3Binary(char** cursor);
	math::float4 LoadFloat4Binary(char** cursor);
	//void SaveChildGameObjectBinary(JSON_Object* config_node, const GameObject& gameObject, uint& count, uint& countResources);
	//void LoadSceneBinary(const char* sceneName);
	// --------------------------------------

	// SAVE & LOAD PREFAB --------------------------
	void SavePrefab(const GameObject& gameObject, const char* directory, const char* fileName, bool is_FBX = true);
	void SaveChildPrefab(JSON_Object* config_node, const GameObject& gameObject, uint& count, uint& countResources);
	void LoadPrefab(const char* prefab);
	GameObject* GetLoadPrefab(const char* prefab, bool is_instantiate = false);
	//GameObject* GetLoadPrefab(std::string prefab, float3 position = float3(0, 0, 0), Quat rotation = Quat(0, 0, 0, 0)); Need...

	void LoadChildLoadPrefab(GameObject& parent, GameObject& child, int uuidParent);
	// --------------------------------------

	// SAVE & LOAD MAP ---------------------------
	void SaveMapWalkable(std::vector<std::string>& map, int height_map, int width_map, float separation, const char* name);
	bool LoadMapWalkable(std::vector<std::string>& map, int& height_map, int& width_map, float& separation, const char* file, std::string& name_map);

	void SaveMapCreation(std::vector<std::string>& map, std::vector<std::string>& prefabs, int height_map, int width_map, float separation, const char* name);
	bool LoadMapCreation(std::vector<std::string>& map, std::vector<std::string>& prefabs, int& height_map, int& width_map, float& separation, int& number_prefabs, const char* file, std::string& name_map);

	void SaveMapNavigation(std::vector<std::string>& map, int height_map, int width_map, float separation, const char* name);
	void LoadMapNavigation(std::vector<std::string>& map, int& height_map, int& width_map, float& separation, const char* name, std::string& file);
	// --------------------------------------

	// SAVE & LOAD MATERIAL --------------------------
	void SaveMaterial(const ResourceMaterial* material, const char* directory, const char* fileName);
	//void LoadMaterial(const char* Material); At the moment no need this
	// --------------------------------------

	// SAVE & LOAD SCRIPT --------------------------
	void SaveScript(const ResourceScript* script, const char* directory, const char* fileName);
	// --------------------------------------

	// SAVE & LOAD ANIMATION --------------------------
	void SaveAnimation(const ResourceAnimation* script, const char* directory, const char* fileName, const char* library);
	// --------------------------------------
	// SAVE & LOAD PLAYER ACTION --------------------------
	void SavePlayerAction(const PlayerActions* player_action, const char* directory, const char* fileName);
	void SaveInputManager(JSON_Object* config_node, const InputManager* input_manager, uint count, uint action_count);
	void SaveInputAction(JSON_Object* config_node, const InputAction* input_action, uint count, uint input_count);
	void LoadPlayerAction(PlayerActions** player_action,const char* fileName);

	// SAVE & LOAD FONT --------------------------
	void SaveFont(const ResourceFont* font, const char* directory, const char* fileName);
	// SAVE & LOAD UI ANIMATION --------------------------
	void SaveUIAnimation(const AnimationJson* font, const char* directory, const char* fileName);

	// Edit Config (only used by Build) ---------------------------
	void SaveConfig(std::string config_path, std::string game_name, std::string inital_scene, bool game_mode, bool full_screen, bool resizable_window, bool borderless, bool full_desktop);
	

	// --------------------------------------

	// Get UUIDs -------------------------------
	ReImport GetUUIDPrefab(const char* file, uint id);
	ReImport GetUUIDMaterial(const char* file);
	ReImport GetUUIDScript(const char* file);
	ReImport GetUUIDFont(const char * file);
	// -----------------------------------------------

	// Get Last Write -------------------------------
	std::time_t GetLastWritePrefab(const char* file);
	std::time_t GetLastWriteMaterial(const char* file);
	std::time_t GetLastWriteScript(const char* file);
	std::time_t GetLastWriteFont(const char * file);
	// -----------------------------------------------

	// Create Resources -----------------------------
	void CreateResourcesLoad(std::vector<std::string>& files_meta);
	// ----------------------------------------------

	// Geu UID if Resources has not imported in Library
	void ResourcesInLibrary(std::string& path, uint type);

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

