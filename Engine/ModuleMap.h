#ifndef __MODULEMAP__
#define __MODULEMAP__

#include "Globals.h"
#include "Module.h"

#define MAX_ARRAY 100

enum TypeMap
{
	MAP_NON = 0,
	MAP_WALKABLE,
	MAP_3D,
	MAP_NAVIGATION
};

class ModuleMap : public Module
{
public:
	ModuleMap();
	virtual ~ModuleMap();

	bool Init(JSON_Object* node);

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void ShowEditorMap(bool& active);

	void ShowWalkableMap();
	void ShowCreationMap();
	void GetSizePrefab(GameObject* obj, float& min_size, float& max_size);
	void ShowNavigationMap();

	void ShowTextWithColor(ImGuiCol_ type, int id);

	void DeleteMap();
	void ResetMap();
	void CompletNoWalk();

	TypeMap CheckTypeMap(const char* map);

	int GetHeightMap();
	int GetWidthMap();


public:
	//Map ----------
	std::string map_string;
	int map[MAX_ARRAY][MAX_ARRAY];
	std::vector<std::string> map_files; // Vector with all fbx/obj in Assets
	
	std::string imported_map;

private:
	int height_map = 10;
	int width_map = 10;
	char* name_map = nullptr;
	float size_separation = 25.4;
	std::vector<std::string> all_prefabs; // Vector with all fbx/obj in Assets
	std::vector<std::string> prefabs; // Vector with all fbx/obj in Assets

	std::vector<std::string> vector_map;
	
	bool map_created = false;
	bool show_numeration = false;

};

#endif // __MODULEMAP__