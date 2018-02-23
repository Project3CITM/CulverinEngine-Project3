#ifndef _SCENE_
#define _SCENE_

#include "Module.h"
#include "Globals.h"
#include "ModuleFramebuffers.h"
#include "CompMesh.h"
#include "Quadtree.h"
#include <vector>

class GameObject;
class SkyBox;
//Event system test
union Event;

class Scene : public Module
{
public:
	Scene(bool start_enabled = false);
	virtual ~Scene();

	//bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	//update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool CleanUp();

	//Scene Loading
	void LoadScene();

	//Event system test
	bool SetEventListenrs();
	void OnEvent(Event& event);

	// Editor outputs 
	void EditorQuadtree();
	void EditorSkybox();

	// Scripting utility functions
	bool CheckNoFails();
	void StartScripts();
	void ClearAllVariablesScript();

	//Remove references of that gameobject in all scripts
	void SetScriptVariablesToNull(GameObject* go);

	GameObject* GetGameObjectfromScene(bool& active);
	GameObject* GetGameObjectbyuid(uint uid);
	GameObject* FindGameObjectWithTag(const char* str);

	void ModificateParent(GameObject* child, GameObject* new_parent);

	
	// DRAWING METHODS ---------
	void DrawPlane();
	void DrawCube(float size);

	// CULLING HELPER FUNCTION -----------
	void FillStaticObjectsVector(bool fill);

	//OBJECTS CREATION / DELETION ---------------------
	GameObject* FindCanvas();
	GameObject* CreateGameObject(GameObject* parent = nullptr);
	GameObject* CreateCube(GameObject* parent = nullptr);
	GameObject* CreatePlane(GameObject* parent = nullptr);
	GameObject* CreateMainCamera(GameObject* parent = nullptr);
	GameObject* CreateMainLight(GameObject* parent = nullptr);
	GameObject* CreateCanvas(GameObject* parent = nullptr);
	GameObject* CreateImage(GameObject* parent = nullptr);
	GameObject* CreateButton(GameObject* parent = nullptr);
	GameObject* CreateCheckBox(GameObject* parent = nullptr);
	GameObject* CreateText(GameObject* parent = nullptr);
	GameObject* CreateEditText(GameObject* parent = nullptr);

	void DeleteAllGameObjects(GameObject* gameobject, bool isMain = true);
	void DeleteGameObject(GameObject* gameobject, bool isImport = false);
	// -------------------------------------

	// UI ----------------------------------
	Component* BlitSceneComponentsAsButtons(Comp_Type type, std::string& current_item);
	// -------------------------------------

public:
	FrameBuffer* scene_buff = nullptr;

	//Container of all Game Objects
	GameObject* root = nullptr;

	//Container Vector of Static Objects (to speeding searches with quadtree)
	std::vector<GameObject*> static_objects;

	// Scene Saved
	bool scene_saved = true; // TODO XAVI - need implementation with Event System

	// Quadtree ----------------
	Quadtree quadtree;
	bool quadtree_draw = false;
	// -------------------------

	// Skybox --------------------
	SkyBox* skybox = nullptr;
	int skybox_index = 0;
	bool draw_skybox = true;
	// ---------------------------

	GameObject* drag = nullptr;
	uint icon_options_transform = 0;

private:

	int size_plane = 0;
	float size_quadtree = 0.0f;
	uint icon_resetAll = 0;
	bool load_scene = true;
	std::vector<Component*> temp_vector;
	Comp_Type actualType = Comp_Type::C_UNKNOWN;
};

#endif